from models import SessionLocal, Membership

class MembershipRepository:
    def __init__(self):
        self.db = SessionLocal

    def is_member(self, user_id: int, guild_id: int) -> bool:
        db = self.db()
        membership = db.query(Membership).filter(
            Membership.user_id == user_id,
            Membership.guild_id == guild_id
        ).first()
        db.close()
        return membership is not None
