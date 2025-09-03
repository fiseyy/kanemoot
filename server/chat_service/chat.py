from models import SessionLocal, Message

def save_message(channel_id, user_id, content):
    db = SessionLocal()
    msg = Message(channel_id=channel_id, user_id=user_id, content=content)
    db.add(msg)
    db.commit()
    db.refresh(msg)
    db.close()
    return msg

def get_last_messages(channel_id, limit=50):
    db = SessionLocal()
    messages = (
        db.query(Message)
        .filter(Message.channel_id == channel_id)
        .order_by(Message.timestamp.desc())
        .limit(limit)
        .all()
    )
    db.close()
    return list(reversed(messages))  # чтобы были по возрастанию
