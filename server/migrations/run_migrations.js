const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

const scriptDir = __dirname;
const dbHost = process.env.CHATAPP_DB_HOST || 'localhost';
const dbPort = process.env.CHATAPP_DB_PORT || '5432';
const dbName = process.argv[2] || process.env.CHATAPP_DB_NAME || 'chatapp';
const dbUser = process.env.CHATAPP_DB_USER || 'postgres';

const env = {
    ...process.env,
    PGUSER: process.env.PGUSER || dbUser,
    PGPASSWORD: process.env.PGPASSWORD || process.env.CHATAPP_DB_PASSWORD || ''
};

function runPsql(args, options = {}) {
    const result = spawnSync('psql', args, {
        env,
        encoding: 'utf8',
        stdio: options.capture ? ['ignore', 'pipe', 'pipe'] : 'inherit'
    });

    if (result.error) {
        if (result.error.code === 'ENOENT') {
            console.error('[ERROR] psql command not found');
            console.error('Ensure PostgreSQL is installed and added to PATH');
            process.exit(1);
        }
        throw result.error;
    }

    if (result.status !== 0 && options.allowFailure !== true) {
        process.exit(result.status || 1);
    }

    return options.capture ? result.stdout.trim() : '';
}

function baseArgs() {
    return ['-h', dbHost, '-p', dbPort, '-U', dbUser, '-d', dbName];
}

function getMigrationFiles() {
    return fs.readdirSync(scriptDir)
        .filter(name => /^\d{3}_.+\.sql$/i.test(name))
        .sort();
}

function isVersionApplied(version) {
    const output = runPsql(
        [...baseArgs(), '-t', '-A', '-c', `SELECT version FROM schema_migrations WHERE version = '${version}' LIMIT 1;`],
        { capture: true, allowFailure: true }
    );
    return output === version;
}

function main() {
    console.log('========================================');
    console.log('   ChatApp Database Migration Tool');
    console.log('========================================');
    console.log('');

    console.log('[CHECK] Connecting to database...');
    const ping = spawnSync('psql', [...baseArgs(), '-c', 'SELECT 1'], {
        env,
        encoding: 'utf8',
        stdio: 'ignore'
    });

    if (ping.error) {
        if (ping.error.code === 'ENOENT') {
            console.error('[ERROR] psql command not found');
            console.error('Ensure PostgreSQL is installed and added to PATH');
        } else {
            console.error('[ERROR] Failed to invoke psql:', ping.error.message);
        }
        process.exit(1);
    }

    if (ping.status !== 0) {
        console.error('[ERROR] Unable to connect to database');
        console.error(`Host: ${dbHost}:${dbPort}`);
        console.error(`Database: ${dbName}`);
        console.error(`User: ${dbUser}`);
        process.exit(1);
    }

    console.log('[OK] Database connection successful');
    console.log('');

    console.log('[CHECK] Ensuring schema_migrations exists...');
    runPsql([...baseArgs(), '-f', path.join(scriptDir, '000_create_schema_migrations_table.sql')], {
        allowFailure: true
    });

    const migrationFiles = getMigrationFiles();
    if (migrationFiles.length === 0) {
        console.log('[INFO] No migration files found');
        return;
    }

    console.log('[START] Applying migrations...');
    console.log('');

    for (const fileName of migrationFiles) {
        const version = fileName.slice(0, 3);

        if (version === '000') {
            console.log(`[SKIP] ${fileName} (bootstrap script)`);
            continue;
        }

        if (isVersionApplied(version)) {
            console.log(`[SKIP] ${fileName} (already applied)`);
            continue;
        }

        console.log(`[APPLY] ${fileName}`);
        runPsql([...baseArgs(), '-f', path.join(scriptDir, fileName)]);
        console.log(`[OK] ${fileName}`);
        console.log('');
    }

    console.log('========================================');
    console.log('   Migration completed');
    console.log('========================================');
    console.log('');
    console.log('[STATE] Current migration versions:');
    runPsql([...baseArgs(), '-c', 'SELECT version, applied_at FROM schema_migrations ORDER BY version;']);
}

main();