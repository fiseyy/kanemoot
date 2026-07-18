# WebSocket контракт для iOS-клиента

## Обновлено: 02.07.2026

Это обновленный контракт между Python WebSocket-серверами и будущим iOS-клиентом. Контракт оптимизирован для удобной работы с Swift/SwiftUI и готов к миграции на распределенную архитектуру (Bootstrap → Node → Event Bus).

---

## Общая структура

### Два WebSocket endpoint'а

1. **Auth Service**: `ws://host:5001/ws` — авторизация, регистрация, управление сессиями
2. **Main/Chat Service**: `ws://host:5002/ws` — сообщения, серверы, каналы, realtime события

### Обязательные поля для всех запросов

```json
{
  "action": "action_name",
  "request_id": "client-generated-uuid"
}
```

- `action` — тип операции
- `request_id` (опционально, но рекомендуется) — UUID от клиента для сопоставления запроса/ответа

### Обязательные поля для всех ответов

```json
{
  "action": "response_action",
  "request_id": "same-as-request"
}
```

### Унифицированный формат ошибок

Все ошибки теперь возвращаются в едином формате:

```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "ERROR_CODE",
  "message": "Human readable message"
}
```

Коды ошибок:
- `AUTH_REQUIRED` — JWT не передан
- `JWT_EXPIRED` — JWT истек
- `INVALID_JWT` — JWT невалидный
- `MISSING_PARAMS` — отсутствуют обязательные параметры
- `ACCESS_DENIED` — нет доступа к ресурсу
- `SEND_FAILED` — ошибка отправки сообщения
- `CREATE_FAILED` — ошибка создания ресурса
- `DELETE_FAILED` — ошибка удаления
- `JOIN_FAILED` — ошибка присоединения к серверу
- `UNKNOWN_ACTION` — неизвестное действие
- `INTERNAL_ERROR` — внутренняя ошибка сервера

### Формат timestamp

Все временные метки теперь в ISO 8601 UTC:

```
"2026-07-02T10:50:23.123456Z"
```

---

## Auth Service (`:5001/ws`)

### Ping/Pong (heartbeat)

**Запрос:**
```json
{
  "action": "ping",
  "request_id": "uuid"
}
```

**Ответ:**
```json
{
  "action": "pong",
  "request_id": "uuid",
  "timestamp": "2026-07-02T10:50:23.123456Z"
}
```

### Login

**Запрос:**
```json
{
  "action": "login",
  "user": "username",
  "password": "password"
}
```

**Успех:**
```json
{
  "success": true,
  "error": "",
  "jwt": "short_lived_jwt_30min",
  "access_token": "long_lived_token_1year",
  "user_id": 123
}
```

**Ошибка:**
```json
{
  "success": false,
  "error": "Неправильный логин и/или пароль",
  "jwt": ""
}
```

### Registration

**Запрос:**
```json
{
  "action": "reg",
  "user": "username",
  "password": "password",
  "email": "email@example.com"
}
```

**Успех:**
```json
{
  "success": true,
  "error": "",
  "jwt": "short_lived_jwt",
  "access_token": "long_lived_token",
  "user_id": 456
}
```

**Ошибка:**
```json
{
  "success": false,
  "error": "Пользователь с таким логином уже существует",
  "jwt": ""
}
```

### Auto JWT login

Проверка/обновление существующего JWT.

**Запрос:**
```json
{
  "action": "auto_jwt_login",
  "token": "existing_jwt"
}
```

**Успех:**
```json
{
  "success": true,
  "error": "",
  "jwt": "same_jwt",
  "user_id": 123
}
```

**Ошибка:**
```json
{
  "success": false,
  "error": "JWT expired",
  "jwt": ""
}
```

### Auto access token login

Получение нового JWT по долгоживущему access token (рекомендуется для iOS).

**Запрос:**
```json
{
  "action": "auto_token_login",
  "token": "long_lived_access_token"
}
```

**Успех:**
```json
{
  "success": true,
  "error": "",
  "jwt": "new_fresh_jwt",
  "access_token": "same_access_token",
  "user_id": 123
}
```

**Ошибки:**
- `"Invalid token"` — токен не найден
- `"Token blocked"` — токен заблокирован
- `"Token expired"` — токен истек

---

## Main/Chat Service (`:5002/ws`)

### Авторизация

Все запросы к Main Service требуют:

```json
{
  "jwt": "short_lived_jwt"
}
```

Если JWT невалидный/истек, будет возвращена ошибка:

```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "JWT_EXPIRED",
  "message": "Jwt Expired"
}
```

iOS должен автоматически:
1. Сходить в Auth Service с `auto_token_login`
2. Получить свежий JWT
3. Повторить исходный запрос

### Ping/Pong

**Запрос:**
```json
{
  "action": "ping",
  "request_id": "uuid"
}
```

**Ответ:**
```json
{
  "action": "pong",
  "request_id": "uuid",
  "timestamp": "2026-07-02T10:50:23.123456Z"
}
```

### Send message

**Запрос:**
```json
{
  "action": "send_message",
  "request_id": "uuid",
  "jwt": "jwt",
  "guild_id": 1,
  "channel_id": 10,
  "content": "Привет!",
  "client_message_id": "client-uuid-for-optimistic-ui"
}
```

**Успех (ACK для отправителя):**
```json
{
  "action": "message_sent_ack",
  "request_id": "uuid",
  "message_id": 555,
  "guild_id": 1,
  "channel_id": 10,
  "user_id": 123,
  "client_message_id": "client-uuid-for-optimistic-ui",
  "content": "Привет!",
  "timestamp": "2026-07-02T10:50:23.123456Z"
}
```

**Ошибка:**
```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "SEND_FAILED",
  "message": "User is not a member of the guild"
}
```

**Важно для iOS:**
- `client_message_id` позволяет реализовать optimistic UI: показать сообщение до получения ACK
- При получении ACK можно заменить временное сообщение на серверное с `message_id`
- В будущем будет добавлено broadcast-событие `message_created` для других клиентов

### Get messages

**Запрос:**
```json
{
  "action": "get_messages",
  "request_id": "uuid",
  "jwt": "jwt",
  "guild_id": 1,
  "channel_id": 10
}
```

**Успех:**
```json
{
  "action": "messages",
  "request_id": "uuid",
  "channel_id": 10,
  "guild_id": 1,
  "messages": [
    {
      "id": 555,
      "user_id": 123,
      "content": "Привет!",
      "timestamp": "2026-07-02T10:50:23.123456Z"
    },
    {
      "id": 556,
      "user_id": 124,
      "content": "Как дела?",
      "timestamp": "2026-07-02T10:51:00.000000Z"
    }
  ]
}
```

**Ошибка:**
```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "ACCESS_DENIED",
  "message": "User is not a member of this guild"
}
```

**Улучшение:** Теперь сервер проверяет membership перед выдачей сообщений.

### Get user servers

Получить список серверов пользователя со всеми каналами.

**Запрос:**
```json
{
  "action": "get_user_servers",
  "request_id": "uuid",
  "jwt": "jwt"
}
```

**Успех:**
```json
{
  "action": "user_servers",
  "request_id": "uuid",
  "servers": [
    {
      "id": 1,
      "name": "My Server",
      "avatar_url": "",
      "role": "owner",
      "channels": [
        {"id": 10, "name": "general", "type": "text"},
        {"id": 11, "name": "random", "type": "text"},
        {"id": 12, "name": "Voice", "type": "voice"}
      ]
    },
    {
      "id": 2,
      "name": "Another Server",
      "avatar_url": "",
      "role": "member",
      "channels": [
        {"id": 20, "name": "chat", "type": "text"}
      ]
    }
  ]
}
```

### Create server

**Запрос:**
```json
{
  "action": "create_server",
  "request_id": "uuid",
  "jwt": "jwt",
  "name": "My New Server"
}
```

**Успех:**
```json
{
  "action": "create_server",
  "request_id": "uuid",
  "success": true,
  "server": {
    "id": 3,
    "name": "My New Server",
    "avatar_url": "",
    "url": "https://server.kanemoot.ru/abcd1234"
  }
}
```

**Ошибка:**
```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "CREATE_FAILED",
  "message": "Failed to create server: database error"
}
```

### Join server

**Запрос:**
```json
{
  "action": "join_server",
  "request_id": "uuid",
  "jwt": "jwt",
  "url": "https://server.kanemoot.ru/abcd1234"
}
```

**Успех:**
```json
{
  "action": "joined_server",
  "request_id": "uuid",
  "server": {
    "id": 1,
    "name": "Some Server",
    "avatar_url": "",
    "url": "https://server.kanemoot.ru/abcd1234"
  }
}
```

**Ошибка:**
```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "JOIN_FAILED",
  "message": "Server not found"
}
```

### Create channel

**Запрос:**
```json
{
  "action": "create_channel",
  "request_id": "uuid",
  "jwt": "jwt",
  "server_id": 1,
  "channel_name": "new-channel",
  "channel_type": "text"
}
```

**Успех:**
```json
{
  "action": "create_channel",
  "request_id": "uuid",
  "success": true,
  "channel": {
    "id": 15,
    "name": "new-channel",
    "type": "text",
    "server_id": 1
  }
}
```

**Ошибка:**
```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "CREATE_FAILED",
  "message": "Permission denied"
}
```

### Delete channel

**Запрос:**
```json
{
  "action": "delete_channel",
  "request_id": "uuid",
  "jwt": "jwt",
  "server_id": 1,
  "channel_id": 15
}
```

**Успех:**
```json
{
  "action": "delete_channel",
  "request_id": "uuid",
  "success": true,
  "channel_id": 15,
  "server_id": 1
}
```

**Ошибка:**
```json
{
  "action": "error",
  "request_id": "uuid",
  "code": "DELETE_FAILED",
  "message": "Channel does not belong to this server"
}
```

---

## Рекомендации для iOS-разработки

### 1. Архитектура сетевого слоя

```
SwiftUI Views
  ↓
ViewModels (@Observable)
  ↓
Domain Repositories (AuthRepository, ChatRepository, ServerRepository)
  ↓
MessagingClient protocol
  ↓
Connection Orchestrator
  ↓
AuthGateway + RealtimeGateway
  ↓
WebSocket Transport
```

### 2. Token lifecycle

```swift
// Keychain
let accessToken: String // долгоживущий, 1 год

// Memory
var jwt: String? // короткоживущий, 30 минут

func refreshJWT() async throws {
    let response = await authGateway.send(
        action: "auto_token_login",
        params: ["token": accessToken]
    )
    self.jwt = response.jwt
}
```

### 3. Request/Response matching

```swift
struct Request {
    let id: UUID
    let action: String
    let payload: [String: Any]
    let continuation: CheckedContinuation<Response, Error>
}

// При отправке
let requestId = UUID()
let request = Request(id: requestId, action: "send_message", ...)
pendingRequests[requestId] = request

// При получении
if let request = pendingRequests[response.requestId] {
    request.continuation.resume(returning: response)
}
```

### 4. Обработка ошибок

```swift
func handleError(_ error: ServerError) {
    switch error.code {
    case "JWT_EXPIRED":
        Task { 
            try await refreshJWT()
            try await retryLastRequest()
        }
    case "ACCESS_DENIED":
        showAlert("Нет доступа")
    case "INTERNAL_ERROR":
        showRetryDialog()
    default:
        showGenericError(error.message)
    }
}
```

### 5. Reconnect strategy

```swift
var reconnectDelay = 1.0
let maxDelay = 30.0

func reconnect() {
    Task {
        try await Task.sleep(for: .seconds(reconnectDelay))
        do {
            try await connect()
            reconnectDelay = 1.0 // reset
        } catch {
            reconnectDelay = min(reconnectDelay * 2, maxDelay)
            reconnect()
        }
    }
}
```

### 6. Optimistic UI для сообщений

```swift
struct Message {
    let id: String // может быть client_message_id или server message_id
    let content: String
    let status: Status // sending, sent, failed
}

func sendMessage(content: String) {
    let clientId = UUID().uuidString
    let optimisticMessage = Message(
        id: clientId, 
        content: content, 
        status: .sending
    )
    
    messages.append(optimisticMessage)
    
    Task {
        do {
            let ack = try await gateway.sendMessage(
                clientMessageId: clientId,
                content: content
            )
            // Заменить optimistic на серверное
            if let index = messages.firstIndex(where: { $0.id == clientId }) {
                messages[index] = Message(
                    id: "\(ack.messageId)", 
                    content: content, 
                    status: .sent
                )
            }
        } catch {
            // Пометить как failed
            if let index = messages.firstIndex(where: { $0.id == clientId }) {
                messages[index].status = .failed
            }
        }
    }
}
```

### 7. Background/Foreground

```swift
// AppDelegate/SceneDelegate
func sceneDidEnterBackground() {
    // iOS может закрыть WebSocket в течение 30 секунд
    gateway.disconnect()
}

func sceneWillEnterForeground() {
    Task {
        try await gateway.connect()
        try await refreshJWT()
        try await syncMissedMessages()
    }
}
```

### 8. Network monitoring

```swift
import Network

let monitor = NWPathMonitor()
monitor.pathUpdateHandler = { path in
    if path.status == .satisfied {
        // Сеть доступна
        if !gateway.isConnected {
            Task { try await gateway.reconnect() }
        }
    } else {
        // Сеть недоступна
        gateway.disconnect()
        showOfflineBanner()
    }
}
```

### 9. Потокобезопасность

```swift
@MainActor
class ChatViewModel: ObservableObject {
    @Published var messages: [Message] = []
    
    func handleIncomingMessage(_ message: Message) {
        // Уже на MainActor, безопасно обновлять @Published
        messages.append(message)
    }
}

// В сетевом слое
actor WebSocketClient {
    func send(_ data: Data) async throws {
        // actor изолирует состояние
    }
}
```

---

## Будущие изменения (подготовка к Node/Event Bus)

### Broadcast события (в разработке)

Когда появится Redis Pub/Sub, будут добавлены события:

```json
{
  "action": "message_created",
  "guild_id": 1,
  "channel_id": 10,
  "message": {
    "id": 557,
    "user_id": 124,
    "content": "Новое сообщение",
    "timestamp": "2026-07-02T11:00:00.000000Z"
  }
}
```

```json
{
  "action": "channel_created",
  "guild_id": 1,
  "channel": { "id": 16, "name": "new", "type": "text" }
}
```

```json
{
  "action": "member_joined",
  "guild_id": 1,
  "user_id": 125
}
```

### Подписки на события (в разработке)

```json
{
  "action": "subscribe",
  "jwt": "jwt",
  "guild_id": 1,
  "channel_ids": [10, 11, 12]
}
```

### Bootstrap resolver (в разработке)

iOS будет запрашивать у Bootstrap-сервера, к какому Node подключаться:

```json
{
  "action": "resolve_node",
  "user_id": 123,
  "guild_id": 1
}
```

Ответ:

```json
{
  "node_id": "node-a",
  "endpoint": "wss://node-a.kanemoot.ru/ws",
  "fallback": "wss://cdn.kanemoot.ru/ws"
}
```

---

## Миграционная стратегия

1. **Фаза 1 (сейчас)**: iOS работает с одним Python-сервером `:5002`
2. **Фаза 2**: Добавить Bootstrap-сервер, но он пока возвращает тот же endpoint
3. **Фаза 3**: Bootstrap начинает возвращать разные Node для разных guild
4. **Фаза 4**: iOS держит несколько WebSocket-соединений (по одному на Node)
5. **Фаза 5**: Полноценный Event Bus с broadcast-событиями

**Ключ к успеху**: domain repositories и UI не знают про эти детали. Для них есть только `sendMessage()`, `getMessages()` и т.д.

---

## Контрольный чек-лист для iOS

- [ ] Реализовать AuthGateway и RealtimeGateway раздельно
- [ ] Хранить access_token в Keychain
- [ ] Автоматический refresh JWT при `JWT_EXPIRED`
- [ ] Request/response matching через request_id
- [ ] Exponential backoff при reconnect
- [ ] Network monitoring через NWPathMonitor
- [ ] Heartbeat ping каждые 30 секунд
- [ ] Обработка background/foreground transitions
- [ ] Optimistic UI для сообщений через client_message_id
- [ ] Парсинг ISO 8601 timestamp
- [ ] Потокобезопасность через @MainActor для UI state
- [ ] Обработка всех error codes
- [ ] Graceful degradation при network issues
- [ ] Resync данных после reconnect
- [ ] Push notifications для фоновых сообщений (будущее)

---

## Дополнительные улучшения серверного контракта (TODO)

Эти изменения пока не реализованы, но запланированы:

1. ✅ ~~Добавить `request_id`~~ — готово
2. ✅ ~~Унифицировать ошибки~~ — готово
3. ✅ ~~ISO 8601 timestamp~~ — готово
4. ✅ ~~`client_message_id` в send_message~~ — готово
5. ✅ ~~Проверка membership в get_messages~~ — готово
6. ✅ ~~Ping/pong~~ — готово
7. ⏳ WebSocket auth через query param/header (сейчас JWT в каждом сообщении)
8. ⏳ Rate limiting
9. ⏳ Broadcast события для других клиентов
10. ⏳ Подписки на guild/channel
11. ⏳ Typing indicators
12. ⏳ Read receipts
13. ⏳ Message reactions
14. ⏳ File uploads
15. ⏳ Voice/video metadata

---

**Контакт**: Обновления контракта вносятся в этот файл по мере развития серверной архитектуры.
