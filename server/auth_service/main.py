from aiohttp import web
import json
from models import *
from database import *


def auth(action, login, password, email=""):
    """
    Обработка аутентификации и регистрации.
    Возвращает словарь с результатом операции.
    """
    data = {
        "success": "false",
        "error": "",
    }

    if action == "login":
        # Проверка аутентификации пользователя
        if authenticate_user(login, password):
            data["success"] = "true"
        else:
            data["error"] = "Неправильный логин и/или пароль"

    elif action == "reg":
        if not login or not password or not email:
            data["error"] = "Необходимо указать логин, пароль и email"
            return data

        db = SessionLocal()
        existing_user = db.query(User).filter(User.username == login).first()
        db.close()

        if existing_user:
            data["error"] = "Пользователь с таким логином уже существует"
            return data

        try:
            create_user(login, password)
            data["success"] = "true"
        except Exception as e:
            data["error"] = f"Ошибка при создании пользователя: {str(e)}"

    return data


async def websocket_handler(request):
    """
    Асинхронный обработчик WebSocket-соединений.
    Принимает словари в формате JSON, выводит их в консоль и отправляет ответ.
    """
    ws = web.WebSocketResponse()
    await ws.prepare(request)

    peer_name = request.transport.get_extra_info("peername")
    real_ip = request.headers.get("X-Real-IP") or request.headers.get("X-Forwarded-For", "").split(",")[0].strip()
    client_info = real_ip or f"{peer_name[0]}:{peer_name[1]}"
    print(f"Новое подключение от {client_info}")

    async for msg in ws:
        if msg.type == web.WSMsgType.TEXT:
            try:
                data = json.loads(msg.data)
                print(f"\n[Сервер] Получен словарь: {data}")

                response_data = auth(
                    data.get("action"),
                    data.get("user"),
                    data.get("password"),
                    data.get("email", "")
                )

                await ws.send_str(json.dumps(response_data))
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

    print(f"[Сервер] Соединение с {client_info} закрыто")
    return ws


def main():
    """Запуск WebSocket сервера"""
    app = web.Application()
    app.add_routes([web.get("/ws", websocket_handler)])
    print("[Сервер] Запуск сервера на порту 5001...")
    web.run_app(app, host="0.0.0.0", port=5001)


if __name__ == "__main__":
    main()