from repositories.messages import MessageRepository
from repositories.channels import ChannelRepository
from repositories.memberships import MembershipRepository

class ChatService:
    def __init__(self):
        self.repo = MessageRepository()
        self.channel_repo = ChannelRepository()
        self.membership_repo = MembershipRepository()

    def send_message(self, guild_id: int, channel_id: int, user_id: int, content: str):
        channel = self.channel_repo.get(channel_id)
        if not channel or channel.server_id != guild_id:
            return False, "Channel does not belong to the guild"

        if not self.membership_repo.is_member(user_id, guild_id):
            return False, "User is not a member of the guild"

        msg = self.repo.save(channel_id, user_id, content)
        return True, msg

    def get_messages(self, channel_id: int, limit: int = 50):
        return self.repo.get_last(channel_id, limit)
    
    def check_user_access(self, user_id: int, guild_id: int) -> bool:
        """Проверка доступа пользователя к guild"""
        return self.membership_repo.is_member(user_id, guild_id)
