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
