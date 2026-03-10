BEGIN;

ALTER TABLE messages
    ADD COLUMN IF NOT EXISTS status INTEGER DEFAULT 1,
    ADD COLUMN IF NOT EXISTS file_id VARCHAR(255),
    ADD COLUMN IF NOT EXISTS file_name TEXT,
    ADD COLUMN IF NOT EXISTS file_size TEXT,
    ADD COLUMN IF NOT EXISTS file_url TEXT,
    ADD COLUMN IF NOT EXISTS thumbnail_url TEXT;

UPDATE messages
SET status = COALESCE(status, 1),
    type = CASE
        WHEN LOWER(COALESCE(type, '')) IN ('text', 'image', 'file', 'system') THEN LOWER(type)
        WHEN type = '0' THEN 'text'
        WHEN type = '1' THEN 'image'
        WHEN type = '2' THEN 'file'
        WHEN type = '3' THEN 'system'
        ELSE 'text'
    END;

ALTER TABLE messages
    ALTER COLUMN status SET DEFAULT 1;

INSERT INTO schema_migrations (version, description)
VALUES ('003', 'Add message media metadata columns')
ON CONFLICT (version) DO NOTHING;

COMMIT;