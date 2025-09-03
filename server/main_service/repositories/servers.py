from models import SessionLocal, Server, Channel, Message

class ServerRepository:
    def __init__(self):
        self.db = SessionLocal

    def get_user_servers(self, user_id: int):
        db = self.db()
        # Здесь пока простой MVP — вернем все сервера, где user_id есть в memberships
        # Для MVP можно позже добавить таблицу memberships
        servers = db.query(Server).all()
        db.close()
        return servers

    def get_server_channels(self, server_id: int):
        db = self.db()
        channels = db.query(Channel).filter(Channel.server_id == server_id).all()
        db.close()
        return channels
