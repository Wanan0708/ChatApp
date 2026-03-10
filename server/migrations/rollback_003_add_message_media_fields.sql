BEGIN;

ALTER TABLE messages
    DROP COLUMN IF EXISTS thumbnail_url,
    DROP COLUMN IF EXISTS file_url,
    DROP COLUMN IF EXISTS file_size,
    DROP COLUMN IF EXISTS file_name,
    DROP COLUMN IF EXISTS file_id,
    DROP COLUMN IF EXISTS status;

DELETE FROM schema_migrations WHERE version = '003';

COMMIT;