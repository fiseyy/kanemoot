from models import SessionLocal, Message

class MessageRepository:
    def __init__(self):
        self.db = SessionLocal

    def save(self, channel_id: int, user_id: int, content: str) -> Message:
        db = self.db()
        msg = Message(channel_id=channel_id, user_id=user_id, content=content)
        db.add(msg)
        db.commit()
        db.refresh(msg)
        db.close()
        return msg

    def get_last(self, channel_id: int, limit: int = 50):
        db = self.db()
        messages = (
            db.query(Message)
            .filter(Message.channel_id == channel_id)
            .order_by(Message.timestamp.desc())
            .limit(limit)
            .all()
        )
        db.close()
        return list(reversed(messages))
