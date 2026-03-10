BEGIN;

ALTER TABLE messages
ADD COLUMN IF NOT EXISTS is_read BOOLEAN DEFAULT FALSE;

UPDATE messages
SET is_read = FALSE
WHERE is_read IS NULL;

CREATE INDEX IF NOT EXISTS idx_messages_conversation_is_read
ON messages(conversation_id, is_read);

INSERT INTO schema_migrations (version, description)
VALUES ('005', 'Add message read flag')
ON CONFLICT (version) DO NOTHING;

COMMIT;