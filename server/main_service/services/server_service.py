from repositories.servers import ServerRepository
from repositories.channels import ChannelRepository

class ServerService:
    def __init__(self):
        self.server_repo = ServerRepository()
        self.channel_repo = ChannelRepository()

    def get_user_servers(self, user_id: int):
        servers = self.server_repo.get_user_servers(user_id)
        result = []
        for s in servers:
            channels = self.channel_repo.get_by_server(s.id)
            result.append({
                "id": s.id,
                "name": s.name,
                "avatar_url": s.avatar_url,
                "channels": [{"id": c.id, "name": c.name, "type": c.type} for c in channels]
            })
        return result
