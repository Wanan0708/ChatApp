BEGIN;

ALTER TABLE messages
ADD COLUMN IF NOT EXISTS recalled BOOLEAN DEFAULT FALSE;

UPDATE messages
SET recalled = FALSE
WHERE recalled IS NULL;

INSERT INTO schema_migrations (version, description)
VALUES ('004', 'Add message recalled flag')
ON CONFLICT (version) DO NOTHING;

COMMIT;