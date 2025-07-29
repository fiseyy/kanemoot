from aiohttp import web
import json


def auth(action, login, password):
    if action == "login":
        if login == "admin" and password == "admin":
            return "true"
        else:
            return "false"


async def websocket_handler(request):
    """
    Асинхронный обработчик WebSocket-соединений.
    Принимает словари в формате JSON, выводит их в консоль и отправляет ответ.
    """
    ws = web.WebSocketResponse()
    await ws.prepare(request)

    peer_name = request.transport.get_extra_info("peername")
    print(f"Новое подключение от {peer_name}")

    async for msg in ws:
        if msg.type == web.WSMsgType.TEXT:
            try:
                data = json.loads(msg.data)
                print(f"\n[Сервер] Получен словарь: {data}")

                response = {
                    "success": auth(data.get("action"),data.get("user"), data.get("password")),
                    "error": "",
                }

                await ws.send_str(json.dumps(response))
                print(f"[Сервер] Отправлен ответ клиенту")

            except json.JSONDecodeError:
                error_response = {
                    "error": "invalid_json",
                    "message": "Некорректный формат данных. Ожидается JSON",
                }
                await ws.send_str(json.dumps(error_response))
                print(f"[Сервер] Ошибка декодирования JSON: {msg.data}")

        elif msg.type == web.WSMsgType.ERROR:
            print(f"[Сервер] Ошибка соединения: {ws.exception()}")

    print(f"[Сервер] Соединение с {peer_name} закрыто")
    return ws


def main():
    """Запуск WebSocket сервера"""
    app = web.Application()
    app.add_routes([web.get("/ws", websocket_handler)])
    print("[Сервер] Запуск сервера на порту 5001...")
    web.run_app(app, host="0.0.0.0", port=5001)


if __name__ == "__main__":
    main()
