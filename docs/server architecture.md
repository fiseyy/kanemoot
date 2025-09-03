# KANEMOOT — Архитектура серверов

## Общая идея
Система устроена как гибрид:
- **Сообщения** хранятся в глобальной таблице (не размазываются по отдельным БД).
- **Метаданные серверов** (guilds), каналы, участники, баны и права доступа привязаны к `guild_id`.

Это позволяет масштабировать систему, хранить историю и централизованно управлять логикой.

---

# Основные сущности

### Таблица `guilds`
```sql
CREATE TABLE guilds (
    guild_id    UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name        TEXT NOT NULL,
    avatar_url  TEXT,
    owner_id    UUID NOT NULL,
    created_at  TIMESTAMP DEFAULT NOW()
);
```
### Таблица `channels`
```sql
CREATE TABLE channels (
    channel_id  UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    guild_id    UUID NOT NULL REFERENCES guilds(guild_id) ON DELETE CASCADE,
    name        TEXT NOT NULL,
    type        TEXT CHECK (type IN ('text', 'voice')) NOT NULL,
    created_at  TIMESTAMP DEFAULT NOW()
);
```
### Таблица `memberships`
```sql
CREATE TABLE memberships (
    guild_id    UUID NOT NULL REFERENCES guilds(guild_id) ON DELETE CASCADE,
    user_id     UUID NOT NULL,
    role        TEXT CHECK (role IN ('owner','admin','member')) DEFAULT 'member',
    joined_at   TIMESTAMP DEFAULT NOW(),
    PRIMARY KEY (guild_id, user_id)
);
```
### Таблица `bans`
```sql
CREATE TABLE bans (
    guild_id    UUID NOT NULL REFERENCES guilds(guild_id) ON DELETE CASCADE,
    user_id     UUID NOT NULL,
    reason      TEXT,
    banned_at   TIMESTAMP DEFAULT NOW(),
    PRIMARY KEY (guild_id, user_id)
);
```
### Таблица `messages`
```sql
CREATE TABLE messages (
    message_id  UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    guild_id    UUID NOT NULL REFERENCES guilds(guild_id) ON DELETE CASCADE,
    channel_id  UUID NOT NULL REFERENCES channels(channel_id) ON DELETE CASCADE,
    user_id     UUID NOT NULL,
    content     TEXT NOT NULL,
    created_at  TIMESTAMP DEFAULT NOW()
);
```
### API:
```
{ "action": "get_server_info", "guild_id": "..." }
```
#### Ответы:

- 404 → "Такого сервера не существует"
- 403 → "Пользователь забанен"
- 200 → { "name": "...", "avatar_url": "...", "channels": [ ... ] }

### Запросы API:
- `JOIN_SERVER` - Добавляет пользователя в memberships.

- `SEND_MESSAGE` - Кидает сообщение в messages.

- `BAN_USER` - Добавляет запись в bans.

- `LEAVE_SERVER` - Удаляет участника из memberships.

## НА БУДУЩЕЕ
- Расширить permissions для гибкой настройки ролей
- В голосовых каналах планируется отдельная подсистема (медиа-сервис)


# Использование Redis в архитектуре KANEMOOT

## 1. Общая идея
Redis используется как быстрый in-memory уровень для:
- кэширования данных (чтобы не дергать PostgreSQL при каждом чихе);
- обмена сообщениями между микросервисами;
- хранения временных данных (сессии, JWT, OTP);
- реализации rate limiting.

PostgreSQL остаётся источником правды (persistent storage), а Redis — ускоритель и связующее звено.

---

## 2. Основные сценарии применения

### 2.1. Кэш
- Кэширование профилей пользователей:
  - ключ: `user:<id>:profile`
  - TTL: 5–10 минут.
- Кэширование информации о серверах/каналах:
  - ключ: `server:<id>:info`
  - ключ: `channel:<id>:info`

### 2.2. Авторизация
- Сессии и refresh-токены:
  - ключ: `session:<jwt_id>`
  - значение: user_id, срок жизни (TTL).
- OTP/Recovery коды:
  - ключ: `otp:<user_id>`
  - TTL: 5 минут.

### 2.3. Pub/Sub (взаимодействие сервисов)
- Канал `auth-events`:
  - уведомления о регистрации, входе/выходе.
- Канал `chat-events`:
  - новые сообщения, редактирование, удаление.
- Канал `server-events`:
  - создание сервера, бан/анбан, приглашения.

### 2.4. Rate limiting
- Ограничения на запросы:
  - ключ: `ratelimit:<user_id>:<action>`
  - TTL: 1–5 секунд.
- Пример: не более 5 запросов на создание сообщений за 2 секунды.

### 2.5. Транзитные данные
- Хранение "неподтвержденных" сообщений (до записи в PostgreSQL).
- Буферизация push-уведомлений.

---

## 3. Поток данных (пример: сообщение в чате)

1. Клиент → `chat-service` → запрос "отправить сообщение".
2. `chat-service`:
   - проверяет rate limit в Redis,
   - публикует событие в `chat-events`,
   - пишет сообщение в PostgreSQL.
3. `chat-gateway-service` слушает `chat-events` через Redis Pub/Sub и пушит сообщение другим клиентам.
4. Redis держит копию последних сообщений (например, 100 штук) для быстрого ответа на `get_messages`.

---

## 4. Примерные ключи

```txt
user:<id>:profile
server:<id>:info
channel:<id>:info
session:<jwt_id>
otp:<user_id>
ratelimit:<user_id>:<action>
chat:last:<channel_id>
```
## 5. Преимущества

- Ускорение ответов (кэш).

- Меньше нагрузка на PostgreSQL.

- Простое масштабирование (несколько сервисов слушают одни и те же события).

- Возможность легко вводить ограничения и TTL.