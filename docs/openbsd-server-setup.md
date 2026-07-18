# Установка сервера kanemoot на OpenBSD

## 1. Установка PostgreSQL

```sh
doas pkg_add postgresql-server
```

Инициализация базы данных:

```sh
doas su - _postgresql
mkdir /var/postgresql/data
initdb -D /var/postgresql/data -U postgres
exit
```

Включение и запуск:

```sh
doas rcctl enable postgresql
doas rcctl start postgresql
```

Создание баз данных и пользователя:

```sh
doas su - _postgresql
psql -U postgres
```

Внутри psql:

```sql
CREATE USER admin WITH PASSWORD 'password';
CREATE DATABASE auth_service OWNER admin;
CREATE DATABASE chat_service OWNER admin;
\q
```

Выход из-под `_postgresql`:

```sh
exit
```

---

## 2. Установка Python и pip

На OpenBSD Python 3 уже предустановлен. Проверьте версию:

```sh
python3 --version
```

Установите pip (если отсутствует):

```sh
doas pkg_add py3-pip
```

---

## 3. Копирование проекта на сервер

Скопируйте директорию `server/` на OpenBSD-машину, например через scp:

```sh
scp -r server/ user@openbsd-host:/home/user/kanemoot-server/
```

Или клонируйте репозиторий:

```sh
cd /home/user
git clone https://github.com/fiseyy/kanemoot.git
```

---

## 4. Установка Python-зависимостей

Файл `requirements.txt` находится в корне `server/`:

```txt
aiohttp
sqlalchemy
psycopg2-binary
bcrypt
pyjwt
python-dotenv
```

Установите зависимости:

```sh
cd /home/user/kanemoot/server
doas pip3 install -r requirements.txt
```

> **Примечание для OpenBSD**: Если `psycopg2-binary` не собирается, установите системный пакет:
> ```sh
> doas pkg_add py3-psycopg2
> ```

---

## 5. Настройка подключения к БД

Проект использует `.env`-файлы для конфигурации подключения к базе данных. Они уже созданы в репозитории:

- `server/auth_service/.env`
- `server/main_service/.env`

Содержимое `server/auth_service/.env`:

```env
DATABASE_URL=postgresql://admin:password@localhost/auth_service
```

Содержимое `server/main_service/.env`:

```env
DATABASE_URL=postgresql://admin:password@localhost/chat_service
```

Если вы клонировали репозиторий — эти файлы уже на месте. Если копировали вручную — создайте их в соответствующих директориях с указанным содержимым.

> **Важно**: Файлы `.env` содержат пароль в открытом виде и **не должны** попадать в публичный репозиторий. В репозитории они уже добавлены в `.gitignore`.

---

## 6. Запуск серверов (вручную, для теста)

**Терминал 1 — auth-service (порт 5001):**

```sh
cd /home/user/kanemoot/server/auth_service
python3 main.py
```

**Терминал 2 — main-service (порт 5002):**

```sh
cd /home/user/kanemoot/server/main_service
python3 main.py
```

Ожидаемый вывод:

```
[Сервер] Запуск сервера на порту 5001...
[chat.service] Запуск на порту 5002...
```

---

## 7. Настройка автозапуска через rcctl (daemon)

**Файл `/etc/rc.d/kanemoot_auth`:**

```sh
#!/bin/sh
daemon="/usr/local/bin/python3"
daemon_flags="/home/user/kanemoot/server/auth_service/main.py"
daemon_user="root"

. /etc/rc.d/rc.subr

rc_reload=NO

rc_cmd $1
```

**Файл `/etc/rc.d/kanemoot_chat`:**

```sh
#!/bin/sh
daemon="/usr/local/bin/python3"
daemon_flags="/home/user/kanemoot/server/main_service/main.py"
daemon_user="root"

. /etc/rc.d/rc.subr

rc_reload=NO

rc_cmd $1
```

> **Важно**: rc-скрипты на OpenBSD запускаются из корня. Чтобы `load_dotenv()` нашёл `.env`, нужно либо указать полный путь в `load_dotenv("/home/user/kanemoot/server/auth_service/.env")`, либо добавить `cd` в `daemon_flags`. Рекомендуется явно указать путь в коде (см. примечание ниже).

Сделайте скрипты исполняемыми:

```sh
doas chmod +x /etc/rc.d/kanemoot_auth
doas chmod +x /etc/rc.d/kanemoot_chat
```

Включите и запустите:

```sh
doas rcctl enable kanemoot_auth
doas rcctl enable kanemoot_chat
doas rcctl start kanemoot_auth
doas rcctl start kanemoot_chat
```

> **Примечание для OpenBSD**: rc.d-скрипты не поддерживают `WorkingDirectory` как systemd. Если `load_dotenv()` не находит `.env`, замените в `auth_service/models.py` и `main_service/models.py`:
> ```python
> load_dotenv("/home/user/kanemoot/server/auth_service/.env")  # для auth
> load_dotenv("/home/user/kanemoot/server/main_service/.env")  # для main
> ```

---

## 8. Настройка файрвола (pf)

Откройте порты 5001 и 5002 (если нужен внешний доступ).

Добавьте в `/etc/pf.conf`:

```
pass in on egress proto tcp to port { 5001 5002 }
```

Примените:

```sh
doas pfctl -f /etc/pf.conf
```

---

## 9. Проверка работоспособности

Установите websocat:

```sh
doas pkg_add websocat
```

Проверка auth-service:

```sh
echo '{"action": "login", "user": "test", "password": "test"}' | websocat ws://127.0.0.1:5001/ws
```

Ожидаемый ответ:

```json
{"success": false, "error": "Неправильный логин и/или пароль", "jwt": ""}
```

Проверка main-service:

```sh
echo '{"action": "get_user_servers", "jwt": "test"}' | websocat ws://127.0.0.1:5002/ws
```

---

## 10. (Опционально) relayd / nginx для единого порта

Установите nginx:

```sh
doas pkg_add nginx
```

Пример конфигурации `/etc/nginx/nginx.conf`:

```nginx
upstream auth {
    server 127.0.0.1:5001;
}
upstream chat {
    server 127.0.0.1:5002;
}
server {
    listen 80;
    location /auth/ws {
        proxy_pass http://auth/ws;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
    }
    location /chat/ws {
        proxy_pass http://chat/ws;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
    }
}
```

---

## Итоговая структура

| Компонент     | Порт | Путь                                           |
|---------------|------|------------------------------------------------|
| auth-service  | 5001 | `/home/user/kanemoot/server/auth_service/main.py` |
| main-service  | 5002 | `/home/user/kanemoot/server/main_service/main.py` |
| PostgreSQL    | 5432 | —                                              |