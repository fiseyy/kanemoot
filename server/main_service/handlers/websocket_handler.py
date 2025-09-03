import json
from aiohttp import web
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
        if msg.type == web.WSMsgType.TEXT:
            try:
                data = json.loads(msg.data)
                action = data.get("action")
                token = data.get("jwt")

                if not token:
                    await ws.send_str(json.dumps({"error": "JWT required"}))
                    continue

                user_id, err = get_user_id_from_jwt(token) if token else (None, "JWT required")
                if not user_id:
                    await ws.send_str(json.dumps({"error": err}))
                    continue

                # --- Действия ---
                if action == "send_message":
                    success, result = chat_service.send_message(
                        data["guild_id"], data["channel_id"], user_id, data["content"]
                    )
                    if success:
                        m = result
                        await ws.send_str(json.dumps({
                            "action": "message_sent",
                            "success": True,
                            "message_id": m.id,
                            "content": m.content,
                            "timestamp": str(m.timestamp)
                        }))
                    else:
                        await ws.send_str(json.dumps({
                            "action": "message_sent",
                            "success": False,
                            "error": result
                        }))

                elif action == "get_messages":
                    messages = chat_service.get_messages(data["channel_id"])
                    await ws.send_str(json.dumps({
                        "action": "messages",
                        "success": True,
                        "messages": [
                            {"id": m.id, "user_id": m.user_id, "content": m.content, "timestamp": str(m.timestamp)}
                            for m in messages
                        ]
                    }))

                elif action == "get_user_servers":
                    servers = server_service.get_user_servers(user_id)
                    await ws.send_str(json.dumps({
                        "action": "user_servers",
                        "success": True,
                        "servers": servers
                    }))

                else:
                    await ws.send_str(json.dumps({"error": "unknown_action"}))

            except Exception as e:
                await ws.send_str(json.dumps({"error": str(e)}))

    print("[chat.service] Соединение закрыто")
    return ws
