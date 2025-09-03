from aiohttp import web
from handlers.websocket_handler import websocket_handler

app = web.Application()
app.add_routes([web.get("/ws", websocket_handler)])

if __name__ == "__main__":
    print("[chat.service] Запуск на порту 5002...")
    web.run_app(app, host="0.0.0.0", port=5002)
