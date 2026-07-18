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

## 2. Установка Python и pip

На Arch Linux Python 3 уже предустановлен. Проверьте версию:

```sh
python --version
```

Установите pip (если отсутствует):

```sh
sudo pacman -S python-pip
```

---

## 3. Копирование проекта на сервер

Скопируйте директорию `server/` на Arch-машину, например через scp:

```sh
scp -r server/ user@arch-host:/home/user/kanemoot-server/
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
sudo pip install -r requirements.txt
```

> **Примечание для Arch**: Если `psycopg2-binary` не собирается, установите системный пакет:
> ```sh
> sudo pacman -S python-psycopg2
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
python main.py
```

**Терминал 2 — main-service (порт 5002):**

```sh
cd /home/user/kanemoot/server/main_service
python main.py
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
ExecStart=/usr/bin/python /home/user/kanemoot/server/auth_service/main.py
Restart=on-failure
User=root
WorkingDirectory=/home/user/kanemoot/server/auth_service

[Install]
WantedBy=multi-user.target
```

**Файл `/etc/systemd/system/kanemoot_chat.service`:**

```ini
[Unit]
Description=Kanemoot Chat Service
After=network.target postgresql.service

[Service]
ExecStart=/usr/bin/python /home/user/kanemoot/server/main_service/main.py
Restart=on-failure
User=root
WorkingDirectory=/home/user/kanemoot/server/main_service

[Install]
WantedBy=multi-user.target
```

> **Важно**: `WorkingDirectory` указывает на директорию с `.env`, чтобы `load_dotenv()` нашёл файл.

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

## 10. (Опционально) nginx для единого порта

Установите nginx:

```sh
sudo pacman -S nginx
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

Включите и запустите nginx:

```sh
sudo systemctl enable nginx
sudo systemctl start nginx
```

---

## Итоговая структура

| Компонент     | Порт | Путь                                           |
|---------------|------|------------------------------------------------|
| auth-service  | 5001 | `/home/user/kanemoot/server/auth_service/main.py` |
| main-service  | 5002 | `/home/user/kanemoot/server/main_service/main.py` |
| PostgreSQL    | 5432 | —                                              |