# Установка сервера kanemoot на Arch Linux

## 1. Установка PostgreSQL

```sh
sudo pacman -S postgresql
```

Инициализация базы данных:

```sh
sudo -iu postgres
initdb -D /var/lib/postgres/data
exit
```

Включение и запуск:

```sh
sudo systemctl enable postgresql
sudo systemctl start postgresql
```

Создание баз данных и пользователя:

```sh
sudo -iu postgres
psql
```

Внутри psql:

```sql
CREATE USER admin WITH PASSWORD 'password';
CREATE DATABASE auth_service OWNER admin;
CREATE DATABASE chat_service OWNER admin;
\q
```

Выход из-под `postgres`:

```sh
exit
```

---

## 2. Установка Python, pip и venv

На Arch Linux Python 3 уже предустановлен. Проверьте версию:

```sh
python --version
```

Установите pip и venv (если отсутствуют):

```sh
sudo pacman -S python-pip python-virtualenv
```

---

## 3. Копирование проекта на сервер

Скопируйте директорию `server/` на Arch-машину, например через scp:

```sh
scp -r server/ user@arch-host:/kanemoot/server
```

Или клонируйте репозиторий:

```sh
sudo mkdir -p /kanemoot
sudo chown $USER:$USER /kanemoot
cd /kanemoot
git clone https://github.com/fiseyy/kanemoot.git
```

> **Важно**: После клонирования весь репозиторий окажется в `/kanemoot/kanemoot/`. Переместите папку `server`:
> ```sh
> mv /kanemoot/kanemoot/server /kanemoot/server
> rm -rf /kanemoot/kanemoot
> ```

---

## 4. Создание виртуального окружения и установка зависимостей

Файл `requirements.txt` находится в корне `server/`:

```txt
aiohttp
sqlalchemy
psycopg2-binary
bcrypt
pyjwt
python-dotenv
```

Создайте виртуальное окружение:

```sh
cd /kanemoot/server
python -m venv venv
```

Активируйте его и установите зависимости:

```sh
source venv/bin/activate
pip install -r requirements.txt
deactivate
```

> **Примечание для Arch**: Если `psycopg2-binary` не собирается, установите системный пакет:
> ```sh
> sudo pacman -S python-psycopg2
> ```
> Затем установите остальные зависимости через pip (psycopg2-binary можно исключить из requirements.txt).

---

## 5. Настройка подключения к БД

Проект использует `.env`-файлы для конфигурации подключения к базе данных.

Создайте `.env` в `server/auth_service/`:

```sh
cat > /kanemoot/server/auth_service/.env <<EOF
DATABASE_URL=postgresql://admin:password@localhost/auth_service
EOF
```

Создайте `.env` в `server/main_service/`:

```sh
cat > /kanemoot/server/main_service/.env <<EOF
DATABASE_URL=postgresql://admin:password@localhost/chat_service
EOF
```

> **Важно**: Замените `password` на реальный пароль, который вы указали при создании пользователя admin в шаге 1. Файлы `.env` содержат пароль в открытом виде и **не должны** попадать в публичный репозиторий — они уже добавлены в `.gitignore`.

---

## 6. Запуск серверов (вручную, для теста)

**Терминал 1 — auth-service (порт 5001):**

```sh
cd /kanemoot/server/auth_service
/kanemoot/server/venv/bin/python main.py
```

**Терминал 2 — main-service (порт 5002):**

```sh
cd /kanemoot/server/main_service
/kanemoot/server/venv/bin/python main.py
```

Ожидаемый вывод:

```
[Сервер] Запуск сервера на порту 5001...
[chat.service] Запуск на порту 5002...
```

---

## 7. Настройка автозапуска через systemd

**Файл `/etc/systemd/system/kanemoot_auth.service`:**

```ini
[Unit]
Description=Kanemoot Auth Service
After=network.target postgresql.service

[Service]
ExecStart=/kanemoot/server/venv/bin/python /kanemoot/server/auth_service/main.py
Restart=on-failure
User=root
WorkingDirectory=/kanemoot/server/auth_service

[Install]
WantedBy=multi-user.target
```

**Файл `/etc/systemd/system/kanemoot_chat.service`:**

```ini
[Unit]
Description=Kanemoot Chat Service
After=network.target postgresql.service

[Service]
ExecStart=/kanemoot/server/venv/bin/python /kanemoot/server/main_service/main.py
Restart=on-failure
User=root
WorkingDirectory=/kanemoot/server/main_service

[Install]
WantedBy=multi-user.target
```

> **Важно**: `ExecStart` использует python из venv (`/kanemoot/server/venv/bin/python`), а `WorkingDirectory` указывает на директорию с `.env`, чтобы `load_dotenv()` нашёл файл.

Включите и запустите:

```sh
sudo systemctl daemon-reload
sudo systemctl enable kanemoot_auth kanemoot_chat
sudo systemctl start kanemoot_auth kanemoot_chat
```

Проверка статуса:

```sh
sudo systemctl status kanemoot_auth
sudo systemctl status kanemoot_chat
```

---

## 8. Настройка файрвола (nftables)

Установите nftables (если не установлен):

```sh
sudo pacman -S nftables
```

Откройте порты 5001 и 5002 (если нужен внешний доступ).

Создайте файл `/etc/nftables.conf`:

```nft
#!/usr/sbin/nft -f

flush ruleset

table inet filter {
    chain input {
        type filter hook input priority 0; policy drop;

        # Разрешить localhost
        iif lo accept

        # Разрешить установленные соединения
        ct state established,related accept

        # Разрешить SSH (если нужно)
        tcp dport 22 accept

        # Разрешить порты kanemoot
        tcp dport { 5001, 5002 } accept

        # Разрешить ICMP (ping)
        ip protocol icmp accept
        ip6 protocol icmpv6 accept
    }

    chain forward {
        type filter hook forward priority 0; policy drop;
    }

    chain output {
        type filter hook output priority 0; policy accept;
    }
}
```

Примените:

```sh
sudo systemctl enable nftables
sudo systemctl start nftables
```

> **Альтернатива — iptables**: Если вы предпочитаете iptables, установите `iptables-nft` и настройте по аналогии.

---

## 9. Проверка работоспособности

Установите websocat:

```sh
sudo pacman -S websocat
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

## 10. (Опционально) nginx для единого порта с SSL

Установите nginx:

```sh
sudo pacman -S nginx
```

Получите SSL-сертификаты Let's Encrypt:

```sh
sudo pacman -S certbot certbot-nginx
sudo certbot certonly --nginx -d kanemoot.space
```

> **Примечание**: Замените `kanemoot.space` на ваш домен. Сертификаты сохранятся в `/etc/letsencrypt/live/kanemoot.space/`.

Пример конфигурации `/etc/nginx/nginx.conf`:

```nginx
server {
    listen 80;
    server_name kanemoot.space;

    return 301 https://$host$request_uri;
}

server {
    listen 443 ssl;
    server_name kanemoot.space;

    ssl_certificate /etc/letsencrypt/live/kanemoot.space/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/kanemoot.space/privkey.pem;

    include /etc/letsencrypt/options-ssl-nginx.conf;
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem;

    # Auth WebSocket
    location /auth/ws {
        proxy_pass http://127.0.0.1:5001/ws;

        proxy_http_version 1.1;

        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";

        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        proxy_read_timeout 3600s;
        proxy_send_timeout 3600s;
    }

    # Chat WebSocket
    location /chat/ws {
        proxy_pass http://127.0.0.1:5002/ws;

        proxy_http_version 1.1;

        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";

        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        proxy_read_timeout 3600s;
        proxy_send_timeout 3600s;
    }
}
```

> **Как это работает**: nginx принимает WebSocket-соединения на `wss://kanemoot.space/auth/ws` и `wss://kanemoot.space/chat/ws`, заменяет путь на `/ws` и проксирует на соответствующий aiohttp-сервер. Благодаря `proxy_pass http://127.0.0.1:5001/ws` (с `/ws` на конце) путь `/auth/ws` заменяется на `/ws`, и aiohttp, который слушает только `/ws`, получает правильный путь.

Проверьте конфигурацию и перезапустите nginx:

```sh
sudo nginx -t
sudo systemctl restart nginx
```

Настройте автообновление сертификатов:

```sh
sudo systemctl enable certbot-renew.timer
sudo systemctl start certbot-renew.timer
```

---

## Итоговая структура

```
/kanemoot/server/
├── requirements.txt
├── venv/                          # виртуальное окружение
│   └── bin/python                 # используется в systemd
├── auth_service/
│   ├── .env
│   ├── main.py
│   ├── models.py
│   └── database.py
└── main_service/
    ├── .env
    ├── main.py
    ├── models.py
    ├── handlers/
    │   └── websocket_handler.py
    ├── repositories/
    │   ├── channels.py
    │   ├── memberships.py
    │   ├── messages.py
    │   └── servers.py
    └── services/
        ├── chat_service.py
        └── server_service.py
```

| Компонент     | Порт | Путь                                           |
|---------------|------|------------------------------------------------|
| auth-service  | 5001 | `/kanemoot/server/auth_service/main.py`        |
| main-service  | 5002 | `/kanemoot/server/main_service/main.py`        |
| venv python   | —    | `/kanemoot/server/venv/bin/python`             |
| PostgreSQL    | 5432 | —                                              |