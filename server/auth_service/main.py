from aiohttp import web
import asyncio


async def websocket_handler(request):
    """
    Асинхронный обработчик WebSocket-соединений. Принимает входящие сообщения от клиентов,
    выводит их в консоль и отправляет ответ с префиксом "ECHO:".

    При подключении клиента выводит информацию о его адресе. В случае ошибки соединения
    логирует исключение. После закрытия соединения выводит соответствующее уведомление.

    :param request: Объект запроса (Request) из фреймворка aiohttp.
    :return: Объект WebSocketResponse для взаимодействия с клиентом.
    """
    ws = web.WebSocketResponse()
    await ws.prepare(request)  # Подготавливаем соединение

    peer_name = request.transport.get_extra_info("peername")
    print(f"Новое подключение от {peer_name}")

    async for msg in ws:
        if msg.type == web.WSMsgType.TEXT:
            print(f"Получено: {msg.data}")
            # Отправляем ответ с префиксом
            await ws.send_str(f"ECHO: {msg.data}")

        elif msg.type == web.WSMsgType.ERROR:
            print(f"Ошибка соединения: {ws.exception()}")

    print(f"Соединение с {peer_name} закрыто")
    return ws



def main():
    """
    Основная функция для запуска веб-сервера с поддержкой WebSocket.

    Создает экземпляр веб-приложения на основе фреймворка aiohttp, регистрирует маршрут `/ws` 
    для обработки WebSocket-соединений и запускает сервер на всех сетевых интерфейсах (`host=""`) 
    с портом `5001`.

    :return: None
    """
    app = web.Application()
    app.add_routes([web.get("/ws", websocket_handler)])
    web.run_app(app, host="", port=5001)



if __name__ == "__main__":
    main()
