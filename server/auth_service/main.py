import jwt
from datetime import datetime, timedelta
from aiohttp import web
import json
from models import *
from database import *


SECRET = "Fiseyy_JWT_Secret!"  # используем одинаковый секрет, что и в chat-service
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 1440  # 1 день жизни токена
def create_jwt(user_id: int):
    expire = datetime.utcnow() + timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    payload = {
        "user_id": user_id,
        "exp": expire
    }
    token = jwt.encode(payload, SECRET, algorithm=ALGORITHM)
    return token

def auth(action, login="", password="", email="", token=""):
    data = {"success": False, "error": "", "jwt": ""}

    if action == "login":
        db = SessionLocal()
        user = db.query(User).filter(User.username == login).first()
        db.close()

        if user and bcrypt.checkpw(password.encode("utf-8"), user.password_hash.encode("utf-8")):
            data["success"] = True
            data["jwt"] = create_jwt(user.id)
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
            user = create_user(login, password)
            data["success"] = True
            data["jwt"] = create_jwt(user.id)
        except Exception as e:
            data["error"] = f"Ошибка при создании пользователя: {str(e)}"

    elif action == "auto_jwt_login":
        if not token:
            data["error"] = "JWT required"
            return data

        try:
            payload = jwt.decode(token, SECRET, algorithms=[ALGORITHM])
            user_id = payload["user_id"]
            data["success"] = True
            data["jwt"] = token
            data["user_id"] = user_id
        except jwt.ExpiredSignatureError:
            data["error"] = "JWT expired"
        except jwt.InvalidTokenError:
            data["error"] = "Invalid JWT"

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
                    action=data.get("action"),
                    login=data.get("user", ""),
                    password=data.get("password", ""),
                    email=data.get("email", ""),
                    token=data.get("jwt", "")
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