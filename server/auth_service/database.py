from models import SessionLocal, User
import bcrypt
import psycopg2

def create_user(username, password):
    password_hash = bcrypt.hashpw(password.encode("utf-8"), bcrypt.gensalt())

    db = SessionLocal()
    user = User(username=username, password_hash=password_hash.decode("utf-8"))
    db.add(user)
    db.commit()
    db.refresh(user)
    db.close()

def authenticate_user(username, password):
    db = SessionLocal()
    user = db.query(User).filter(User.username == username).first()
    db.close()

    if user and bcrypt.checkpw(password.encode("utf-8"), user.password_hash.encode("utf-8")):
        return True
    return False

def check_connection():
    try:
        connection = psycopg2.connect(
            dbname="auth_service",
            user="admin",
            password="password",
            host="localhost"
        )
        print("Подключение успешно!")
        connection.close()
        return True
    except Exception as e:
        print("Ошибка подключения:", e)
        return False