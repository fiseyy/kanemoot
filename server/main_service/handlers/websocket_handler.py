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

            elif action == "get_user_servers":
                servers = server_service.get_user_servers(user_id)
                await ws.send_str(json.dumps({
                    "action": "user_servers",
                    "servers": servers
                }))

            else:
                await ws.send_str(json.dumps({"error": "unknown_action"}))

        except Exception as e:
            print("WS handler exception:", e)
            await ws.send_str(json.dumps({"error": f"internal_error: {str(e)}"}))

    print("[chat.service] Соединение закрыто")
    return ws
