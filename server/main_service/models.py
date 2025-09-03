from sqlalchemy import create_engine, Column, Integer, String, ForeignKey, Text, DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, relationship
from datetime import datetime

DATABASE_URL = "postgresql://postgres:FiseyyDB!@localhost/chat_service"
engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

class Server(Base):
    __tablename__ = "servers"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, nullable=False)
    avatar_url = Column(String, nullable=True)
    url = Column(String, unique=True, nullable=False)

class Channel(Base):
    __tablename__ = "channels"
    id = Column(Integer, primary_key=True, index=True)
    server_id = Column(Integer, ForeignKey("servers.id"), nullable=False)
    name = Column(String, nullable=False)
    type = Column(String, default="text")  # text / voice
    server = relationship("Server", backref="channels")

class Message(Base):
    __tablename__ = "messages"
    id = Column(Integer, primary_key=True, index=True)
    channel_id = Column(Integer, ForeignKey("channels.id"), nullable=False)
    user_id = Column(Integer, nullable=False)  # пока просто int, потом подтянем auth
    content = Column(Text, nullable=False)
    timestamp = Column(DateTime, default=datetime.utcnow)

class Membership(Base):
    __tablename__ = "memberships"
    guild_id = Column(Integer, ForeignKey("servers.id"), primary_key=True)
    user_id = Column(Integer, primary_key=True)
    role = Column(String, default="member")  # owner/admin/member
    joined_at = Column(DateTime, default=datetime.utcnow)

Base.metadata.create_all(bind=engine)
