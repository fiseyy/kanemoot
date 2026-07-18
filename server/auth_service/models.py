import os
from dotenv import load_dotenv
from sqlalchemy import create_engine, Column, Integer, String, Boolean, DateTime, ForeignKey
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from datetime import datetime

load_dotenv()

DATABASE_URL = os.getenv("DATABASE_URL", "postgresql://admin:password@localhost/auth_service")
engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

class User(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True, index=True)
    username = Column(String, unique=True, index=True)
    password_hash = Column(String)


class AccessToken(Base):
    __tablename__ = "access_tokens"

    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey("users.id"))
    token = Column(String, unique=True, index=True)  # UUID
    blocked = Column(Boolean, default=False)
    last_used = Column(DateTime, default=datetime.utcnow)
    expires_at = Column(DateTime, nullable=True)
    latest_ip = Column(String, nullable=True)
    latest_location = Column(String, nullable=True)


Base.metadata.create_all(bind=engine)