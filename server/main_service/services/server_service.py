from repositories.servers import ServerRepository
from repositories.channels import ChannelRepository
from models import Membership, Server, Channel
class ServerService:
    def __init__(self):
        self.server_repo = ServerRepository()

    def get_user_servers(self, user_id: int):
        servers_with_roles = self.server_repo.get_user_servers_with_role(user_id)
        result = []
        for server, role in servers_with_roles:
            channels = self.server_repo.get_server_channels(server.id)
            result.append({
                "id": server.id,
                "name": server.name,
                "avatar_url": server.avatar_url,
                "role": role,
                "channels": [{"id": c.id, "name": c.name, "type": c.type} for c in channels]
            })
        return result

    def join_server_by_url(self, user_id: int, url: str):
        server = self.server_repo.get_by_url(url)
        if not server:
            return False, "Server not found"
        # при добавлении нового участника сразу задаём роль member
        self.server_repo.add_member(server.id, user_id, role="member")
        return True, server
    def create_channel(self, user_id: int, server_id: int, name: str, type_: str):
        membership = self.server_repo.get_membership(user_id, server_id)
        if not membership:
            return False, "User is not a member of this server"

        if membership.role not in ["owner", "admin"]:
            return False, "Permission denied"
        if type_ not in ["text", "voice"]:
            return False, "Invalid channel type"
        channel = self.server_repo.create_channel(server_id, name, type_)
        return True, channel
    def delete_channel(self, user_id: int, server_id: int, channel_id: int):
        membership = self.server_repo.get_membership(user_id, server_id)
        if not membership:
            return False, "User is not a member of this server"

        if membership.role not in ["owner", "admin"]:
            return False, "Permission denied"

        # проверить, что канал реально принадлежит этому серверу
        channels = self.server_repo.get_server_channels(server_id)
        if not any(c.id == channel_id for c in channels):
            return False, "Channel does not belong to this server"

        ok, err = self.server_repo.delete_channel(channel_id)
        if not ok:
            return False, err

        return True, None
