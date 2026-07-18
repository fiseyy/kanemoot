import os
from dotenv import load_dotenv
from models import SessionLocal, User, AccessToken
import bcrypt
import psycopg2
import uuid
from datetime import datetime, timedelta

load_dotenv()

def create_user(username, password):
    password_hash = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt())

    db = SessionLocal()
    user = User(username=username, password_hash=password_hash.decode("utf-8"))
    db.add(user)
    db.commit()
    db.refresh(user)
    db.close()
    return user


def authenticate_user(username, password):
    db = SessionLocal()
    user = db.query(User).filter(User.username == username).first()
    db.close()

    if user and bcrypt.checkpw(password.encode("utf-8"), user.password_hash.encode("utf-8")):
        return True
    return False

def check_connection():
    try:
        from models import DATABASE_URL
        connection = psycopg2.connect(DATABASE_URL)
        print("Подключение успешно!")
        connection.close()
        return True
    except Exception as e:
        print("Ошибка подключения:", e)
        return False
    

def create_access_token(user_id: int, expire_minutes: int = 0):
    db = SessionLocal()
    token_str = str(uuid.uuid4())
    expires_at = datetime.utcnow() + timedelta(minutes=expire_minutes) if expire_minutes > 0 else None
    token = AccessToken(user_id=user_id, token=token_str, expires_at=expires_at)
    db.add(token)
    db.commit()
    db.refresh(token)
    db.close()
    return token