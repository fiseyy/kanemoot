from aiohttp import web
import json
import jwt
from services.chat_service import ChatService
from services.server_service import ServerService

SECRET = "Fiseyy_JWT_Secret!"

chat_service = ChatService()
server_service = ServerService()

def get_user_id_from_jwt(token: str):
    try:
        payload = jwt.decode(token, SECRET, algorithms=["HS256"])
        return payload["user_id"]
    except jwt.ExpiredSignatureError:
        return None, "JWT expired"
    except jwt.InvalidTokenError:
        return None, "Invalid JWT"

async def websocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)

    print("[chat.service] Новое подключение")

    async for msg in ws:
        try:
            if msg.type != web.WSMsgType.TEXT:
                continue

            print("Incoming WS message:", msg.data)
            data = json.loads(msg.data)
            action = data.get("action")
            token = data.get("jwt")

            if not token:
                await ws.send_str(json.dumps({"error": "JWT required"}))
                continue

            user_id, err = None, None
            try:
                user_id = jwt.decode(token, SECRET, algorithms=["HS256"])["user_id"]
            except jwt.ExpiredSignatureError:
                err = "JWT expired"
            except jwt.InvalidTokenError:
                err = "Invalid JWT"

            if err:
                await ws.send_str(json.dumps({"error": err}))
                continue

            # --- обработка действий ---
            if action == "send_message":
                success, msg_obj = chat_service.send_message(
                    data["guild_id"],
                    data["channel_id"],
                    user_id,
                    data["content"]
                )
                if not success:
                    await ws.send_str(json.dumps({"error": msg_obj}))
                    continue

                await ws.send_str(json.dumps({
                    "action": "message_sent",
                    "message_id": msg_obj.id,
                    "content": msg_obj.content,
                    "timestamp": str(msg_obj.timestamp)
                }))

            elif action == "get_messages":
                messages = chat_service.get_messages(data["channel_id"])
                await ws.send_str(json.dumps({
                    "action": "messages",
                    "messages": [
                        {"id": m.id, "user_id": m.user_id, "content": m.content, "timestamp": str(m.timestamp)}
                        for m in messages
                    ]
                }))
            elif action == "create_channel":
                server_id = data.get("server_id")
                name = data.get("channel_name")
                type_ = data.get("channel_type", "text")

                if not server_id or not name:
                    await ws.send_str(json.dumps({
                        "action": "create_channel",
                        "success": False,
                        "error": "server_id and channel_name required"
                    }))
                    continue

                success, result = server_service.create_channel(user_id, server_id, name, type_)
                if not success:
                    await ws.send_str(json.dumps({
                        "action": "create_channel",
                        "success": False,
                        "error": result
                    }))
                    continue

                await ws.send_str(json.dumps({
                    "action": "create_channel",
                    "success": True,
                    "channel": {
                        "id": result.id,
                        "name": result.name,
                        "type": result.type
                    }
                }))
            elif action == "delete_channel":
                server_id = data.get("server_id")
                channel_id = data.get("channel_id")

                if not server_id or not channel_id:
                    await ws.send_str(json.dumps({
                        "action": "delete_channel",
                        "success": False,
                        "error": "server_id and channel_id required"
                    }))
                    continue

                success, err = server_service.delete_channel(user_id, server_id, channel_id)
                if not success:
                    await ws.send_str(json.dumps({
                        "action": "delete_channel",
                        "success": False,
                        "error": err
                    }))
                    continue

                await ws.send_str(json.dumps({
                    "action": "delete_channel",
                    "success": True,
                    "channel_id": channel_id
                }))

            elif action == "create_server":
                name = data.get("name")
                if not name:
                    await ws.send_str(json.dumps({"action": "create_server", "success": False, "error": "Server name required"}))
                    continue

                # создаём URL для сервера (например, безопасный хеш)
                import uuid
                url = f"https://server.kanemoot.ru/{uuid.uuid4().hex[:8]}"

                from models import SessionLocal, Server, Membership

                db = SessionLocal()
                try:
                    new_server = Server(
                        name=name,
                        avatar_url="",  # дефолт пустой
                        url=url
                    )
                    db.add(new_server)
                    db.commit()
                    db.refresh(new_server)

                    # добавляем пользователя как owner
                    membership = Membership(
                        guild_id=new_server.id,
                        user_id=user_id,
                        role="owner"
                    )
                    db.add(membership)
                    db.commit()

                    await ws.send_str(json.dumps({
                        "action": "create_server",
                        "success": True,
                        "error": "",
                        "server": {
                            "id": new_server.id,
                            "name": new_server.name,
                            "avatar_url": new_server.avatar_url,
                            "url": new_server.url
                        }
                    }))
                except Exception as e:
                    db.rollback()
                    await ws.send_str(json.dumps({
                        "action": "create_server",
                        "success": False,
                        "error": f"Failed to create server: {str(e)}"
                    }))
                finally:
                    db.close()

            elif action == "get_user_servers":
                servers = server_service.get_user_servers(user_id)
                await ws.send_str(json.dumps({
                    "action": "user_servers",
                    "servers": servers
                }))
            elif action == "join_server":
                url = data.get("url")
                if not url:
                    await ws.send_str(json.dumps({"error": "URL required"}))
                    continue

                success, result = server_service.join_server_by_url(user_id, url)
                if not success:
                    await ws.send_str(json.dumps({"error": result}))
                    continue

                await ws.send_str(json.dumps({
                    "action": "joined_server",
                    "server": {
                        "id": result.id,
                        "name": result.name,
                        "avatar_url": result.avatar_url,
                        "url": result.url
                    }
                }))

            else:
                await ws.send_str(json.dumps({"error": "unknown_action"}))

        except Exception as e:
            print("WS handler exception:", e)
            await ws.send_str(json.dumps({"error": f"internal_error: {str(e)}"}))

    print("[chat.service] Соединение закрыто")
    return ws
