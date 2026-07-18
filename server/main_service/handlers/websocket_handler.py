from aiohttp import web
import json
import jwt
from datetime import datetime
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

def send_error(request_id: str, code: str, message: str) -> str:
    """Унифицированный формат ошибки"""
    return json.dumps({
        "action": "error",
        "request_id": request_id,
        "code": code,
        "message": message
    })

def format_timestamp(dt) -> str:
    """Конвертация datetime в ISO 8601 UTC"""
    if dt:
        return dt.isoformat() + "Z"
    return datetime.utcnow().isoformat() + "Z"

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
            request_id = data.get("request_id", "")

            # Ping/pong для heartbeat
            if action == "ping":
                await ws.send_str(json.dumps({
                    "action": "pong",
                    "request_id": request_id,
                    "timestamp": format_timestamp(None)
                }))
                continue

            if not token:
                await ws.send_str(send_error(request_id, "AUTH_REQUIRED", "JWT required"))
                continue

            user_id, err = None, None
            try:
                user_id = jwt.decode(token, SECRET, algorithms=["HS256"])["user_id"]
            except jwt.ExpiredSignatureError:
                err = "JWT_EXPIRED"
            except jwt.InvalidTokenError:
                err = "INVALID_JWT"

            if err:
                await ws.send_str(send_error(request_id, err, err.replace("_", " ").title()))
                continue

            # --- обработка действий ---
            if action == "send_message":
                guild_id = data.get("guild_id")
                channel_id = data.get("channel_id")
                content = data.get("content")
                client_message_id = data.get("client_message_id", "")

                if not guild_id or not channel_id or not content:
                    await ws.send_str(send_error(request_id, "MISSING_PARAMS", "guild_id, channel_id and content required"))
                    continue

                success, msg_obj = chat_service.send_message(
                    guild_id,
                    channel_id,
                    user_id,
                    content
                )
                if not success:
                    await ws.send_str(send_error(request_id, "SEND_FAILED", msg_obj))
                    continue

                # ACK для отправителя
                await ws.send_str(json.dumps({
                    "action": "message_sent_ack",
                    "request_id": request_id,
                    "message_id": msg_obj.id,
                    "guild_id": guild_id,
                    "channel_id": channel_id,
                    "user_id": user_id,
                    "client_message_id": client_message_id,
                    "content": msg_obj.content,
                    "timestamp": format_timestamp(msg_obj.timestamp)
                }))

            elif action == "get_messages":
                channel_id = data.get("channel_id")
                guild_id = data.get("guild_id")

                if not channel_id or not guild_id:
                    await ws.send_str(send_error(request_id, "MISSING_PARAMS", "channel_id and guild_id required"))
                    continue

                # Проверка membership
                if not chat_service.check_user_access(user_id, guild_id):
                    await ws.send_str(send_error(request_id, "ACCESS_DENIED", "User is not a member of this guild"))
                    continue

                messages = chat_service.get_messages(channel_id)
                await ws.send_str(json.dumps({
                    "action": "messages",
                    "request_id": request_id,
                    "channel_id": channel_id,
                    "guild_id": guild_id,
                    "messages": [
                        {
                            "id": m.id,
                            "user_id": m.user_id,
                            "content": m.content,
                            "timestamp": format_timestamp(m.timestamp)
                        }
                        for m in messages
                    ]
                }))
            elif action == "create_channel":
                server_id = data.get("server_id")
                name = data.get("channel_name")
                type_ = data.get("channel_type", "text")

                if not server_id or not name:
                    await ws.send_str(send_error(request_id, "MISSING_PARAMS", "server_id and channel_name required"))
                    continue

                success, result = server_service.create_channel(user_id, server_id, name, type_)
                if not success:
                    await ws.send_str(send_error(request_id, "CREATE_FAILED", result))
                    continue

                await ws.send_str(json.dumps({
                    "action": "create_channel",
                    "request_id": request_id,
                    "success": True,
                    "channel": {
                        "id": result.id,
                        "name": result.name,
                        "type": result.type,
                        "server_id": server_id
                    }
                }))
            elif action == "delete_channel":
                server_id = data.get("server_id")
                channel_id = data.get("channel_id")

                if not server_id or not channel_id:
                    await ws.send_str(send_error(request_id, "MISSING_PARAMS", "server_id and channel_id required"))
                    continue

                success, err = server_service.delete_channel(user_id, server_id, channel_id)
                if not success:
                    await ws.send_str(send_error(request_id, "DELETE_FAILED", err))
                    continue

                await ws.send_str(json.dumps({
                    "action": "delete_channel",
                    "request_id": request_id,
                    "success": True,
                    "channel_id": channel_id,
                    "server_id": server_id
                }))

            elif action == "create_server":
                name = data.get("name")
                if not name:
                    await ws.send_str(send_error(request_id, "MISSING_PARAMS", "Server name required"))
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
                        "request_id": request_id,
                        "success": True,
                        "server": {
                            "id": new_server.id,
                            "name": new_server.name,
                            "avatar_url": new_server.avatar_url,
                            "url": new_server.url
                        }
                    }))
                except Exception as e:
                    db.rollback()
                    await ws.send_str(send_error(request_id, "CREATE_FAILED", f"Failed to create server: {str(e)}"))
                finally:
                    db.close()

            elif action == "get_user_servers":
                servers = server_service.get_user_servers(user_id)
                await ws.send_str(json.dumps({
                    "action": "user_servers",
                    "request_id": request_id,
                    "servers": servers
                }))
            elif action == "join_server":
                url = data.get("url")
                if not url:
                    await ws.send_str(send_error(request_id, "MISSING_PARAMS", "URL required"))
                    continue

                success, result = server_service.join_server_by_url(user_id, url)
                if not success:
                    await ws.send_str(send_error(request_id, "JOIN_FAILED", result))
                    continue

                await ws.send_str(json.dumps({
                    "action": "joined_server",
                    "request_id": request_id,
                    "server": {
                        "id": result.id,
                        "name": result.name,
                        "avatar_url": result.avatar_url,
                        "url": result.url
                    }
                }))

            else:
                await ws.send_str(send_error(request_id, "UNKNOWN_ACTION", f"Unknown action: {action}"))

        except Exception as e:
            print("WS handler exception:", e)
            request_id = ""
            try:
                data = json.loads(msg.data)
                request_id = data.get("request_id", "")
            except:
                pass
            await ws.send_str(send_error(request_id, "INTERNAL_ERROR", f"Internal error: {str(e)}"))

    print("[chat.service] Соединение закрыто")
    return ws
