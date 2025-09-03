from models import SessionLocal, Server, Channel, Message, Membership
class ServerRepository:
    def __init__(self):
        self.db = SessionLocal

    def get_user_servers(self, user_id: int):
        db = SessionLocal()
        servers = (
            db.query(Server)
            .join(Membership, Membership.guild_id == Server.id)
            .filter(Membership.user_id == user_id)
            .all()
        )
        db.close()
        return servers

    def get_server_channels(self, server_id: int):
        db = self.db()
        channels = db.query(Channel).filter(Channel.server_id == server_id).all()
        db.close()
        return channels
    def get_by_url(self, url: str):
        db = self.db()
        server = db.query(Server).filter(Server.url == url).first()
        db.close()
        return server

    def add_member(self, server_id: int, user_id: int):
        db = self.db()
        # проверяем, есть ли уже
        exists = db.query(Membership).filter(
            Membership.guild_id == server_id,
            Membership.user_id == user_id
        ).first()
        if not exists:
            membership = Membership(guild_id=server_id, user_id=user_id)
            db.add(membership)
            db.commit()
        db.close()