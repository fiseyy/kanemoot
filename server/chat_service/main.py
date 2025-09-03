from aiohttp import web
import json
from chat import save_message, get_last_messages

async def websocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)

    print("[chat.service] Новое подключение")

    async for msg in ws:
        if msg.type == web.WSMsgType.TEXT:
            try:
                data = json.loads(msg.data)
                action = data.get("action")

                if action == "send_message":
                    channel_id = data["channel_id"]
                    user_id = data["user_id"]
                    content = data["content"]
                    msg_obj = save_message(channel_id, user_id, content)

                    response = {
                        "action": "message_sent",
                        "message_id": msg_obj.id,
                        "timestamp": str(msg_obj.timestamp),
                        "content": msg_obj.content,
                    }
                    await ws.send_str(json.dumps(response))

                elif action == "get_messages":
                    channel_id = data["channel_id"]
                    messages = get_last_messages(channel_id)
                    response = {
                        "action": "messages",
                        "messages": [
                            {
                                "id": m.id,
                                "user_id": m.user_id,
                                "content": m.content,
                                "timestamp": str(m.timestamp),
                            }
                            for m in messages
                        ],
                    }
                    await ws.send_str(json.dumps(response))

                else:
                    await ws.send_str(json.dumps({"error": "unknown_action"}))

            except Exception as e:
                await ws.send_str(json.dumps({"error": str(e)}))

    print("[chat.service] Соединение закрыто")
    return ws

def main():
    app = web.Application()
    app.add_routes([web.get("/ws", websocket_handler)])
    print("[chat.service] Запуск на порту 5002...")
    web.run_app(app, host="0.0.0.0", port=5002)

if __name__ == "__main__":
    main()
