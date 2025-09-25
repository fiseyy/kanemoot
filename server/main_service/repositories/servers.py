from models import SessionLocal, Server, Membership, Channel

class ServerRepository:
    def __init__(self):
        self.db = SessionLocal

    def get_user_servers_with_role(self, user_id: int):
        db = self.db()
        try:
            servers = (
                db.query(Server, Membership.role)
                .join(Membership, Membership.guild_id == Server.id)
                .filter(Membership.user_id == user_id)
                .all()
            )
            return servers
        finally:
            db.close()

    def get_server_channels(self, server_id: int):
        db = self.db()
        try:
            return db.query(Channel).filter(Channel.server_id == server_id).all()
        finally:
            db.close()

    def get_by_url(self, url: str):
        db = self.db()
        try:
            return db.query(Server).filter(Server.url == url).first()
        finally:
            db.close()

    def add_member(self, server_id: int, user_id: int, role="member"):
        db = self.db()
        try:
            exists = db.query(Membership).filter(
                Membership.guild_id == server_id,
                Membership.user_id == user_id
            ).first()
            if not exists:
                membership = Membership(guild_id=server_id, user_id=user_id, role=role)
                db.add(membership)
                db.commit()
        finally:
            db.close()
    def get_membership(self, user_id: int, server_id: int):
        db = self.db()
        try:
            return db.query(Membership).filter(
                Membership.user_id == user_id,
                Membership.guild_id == server_id
            ).first()
        finally:
            db.close()

    def create_channel(self, server_id: int, name: str, type_: str):
        db = self.db()
        try:
            channel = Channel(server_id=server_id, name=name, type=type_)
            db.add(channel)
            db.commit()
            db.refresh(channel)
            return channel
        finally:
            db.close()
    def delete_channel(self, channel_id: int):
        db = self.db()
        try:
            channel = db.query(Channel).filter(Channel.id == channel_id).first()
            if not channel:
                return False, "Channel not found"
            db.delete(channel)
            db.commit()
            return True, None
        finally:
            db.close()
