import jwt
from datetime import datetime, timedelta
import aiohttp
from aiohttp import web
import json
from models import *
from database import *

JWT_TOKEN_EXPIRE_MINUTES = 30
SECRET = "Fiseyy_JWT_Secret!"  # используем одинаковый секрет, что и в chat-service
ALGORITHM = "HS256"

async def get_location_by_ip(ip: str) -> str:
    if not ip:
        return "Unknown"
    try:
        async with aiohttp.ClientSession() as session:
            async with session.get(f"http://ip-api.com/json/{ip}?fields=country,regionName,city", timeout=2) as resp:
                data = await resp.json()
                if data.get("status") == "success":
                    return ", ".join(filter(None, [data.get("city"), data.get("regionName"), data.get("country")]))
        return "Unknown"
    except:
        return "Unknown"
    
def create_jwt(user_id: int):
    expire = datetime.utcnow() + timedelta(minutes=JWT_TOKEN_EXPIRE_MINUTES)
    payload = {
        "user_id": user_id,
        "exp": expire
    }
    token = jwt.encode(payload, SECRET, algorithm=ALGORITHM)
    return token

async def auth(request, action, login="", password="", email="", token=""):
    data = {"success": False, "error": "", "jwt": ""}

    if action == "login":
        db = SessionLocal()
        user = db.query(User).filter(User.username == login).first()
        if user and bcrypt.checkpw(password.encode("utf-8"), user.password_hash.encode("utf-8")):
            new_jwt = create_jwt(user.id)
            access_token_obj = create_access_token(user.id, expire_minutes=525600)  # 365*24*60
            data["success"] = True
            data["jwt"] = new_jwt
            data["access_token"] = access_token_obj.token
        else:
            data["error"] = "Неправильный логин и/или пароль"
        db.close()

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
            new_jwt = create_jwt(user.id)
            access_token_obj = create_access_token(user.id, expire_minutes=525600)
            data["success"] = True
            data["jwt"] = new_jwt
            data["access_token"] = access_token_obj.token
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
    elif action == "auto_token_login":
        if not token:
            data["error"] = "Token required"
            return data

        db = SessionLocal()
        access_token = db.query(AccessToken).filter(AccessToken.token == token).first()
        if not access_token:
            data["error"] = "Invalid token"
        elif access_token.blocked:
            data["error"] = "Token blocked"
        elif access_token.expires_at and access_token.expires_at < datetime.utcnow():
            data["error"] = "Token expired"
        else:
            # создаем новый JWT при каждом использовании access-token
            new_jwt = create_jwt(access_token.user_id)
            data["success"] = True
            data["jwt"] = new_jwt
            data["access_token"] = access_token.token
            data["user_id"] = access_token.user_id

            # обновляем лог
            access_token.last_used = datetime.utcnow()
            real_ip = request.headers.get("X-Real-IP") or request.headers.get("X-Forwarded-For", "").split(",")[0].strip()
            access_token.latest_ip = real_ip
            access_token.latest_location = await get_location_by_ip(real_ip)
            db.commit()
        db.close()

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

                response_data = await auth(
                    request=request,
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