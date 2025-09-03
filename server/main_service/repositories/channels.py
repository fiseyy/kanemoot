from models import SessionLocal, Channel

class ChannelRepository:
    def __init__(self):
        self.db = SessionLocal

    def get_by_server(self, server_id: int):
        db = self.db()
        channels = db.query(Channel).filter(Channel.server_id == server_id).all()
        db.close()
        return channels
