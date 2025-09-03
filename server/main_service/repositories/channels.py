from models import SessionLocal, Channel

class ChannelRepository:
    def get(self, channel_id: int):
        db = SessionLocal()
        channel = db.query(Channel).filter(Channel.id == channel_id).first()
        db.close()
        return channel
    def get_by_server(self, channel_id: int):
        db = SessionLocal()
        channel = db.query(Channel).filter(Channel.id == channel_id).first()
        db.close()
        return channel
