#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include "utils/logging.h"
#include "core/errorhandler.h"
#include "core/systemfatalerror.h"
#include "core/errorcode.h"

Logging::Logging(QObject* parent) : QObject(parent) {
  QString logDirPath;
#ifdef Q_OS_WIN
    logDirPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/kanemoot/logs";
#elif defined(Q_OS_MACOS)
    logDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/kanemoot/logs";
#else // Linux и прочее
    logDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/kanemoot/logs";
#endif

  QDir dir;
  if (!dir.exists(logDirPath)) {
    dir.mkpath(logDirPath);
  }

  QString logFilePath = logDirPath + "/latest.log";

  QFileInfo fileInfo(logFilePath);
  if (fileInfo.exists()) {
    QDateTime now = QDateTime::currentDateTime();
    QString oldFilePath =
        logDirPath + "/" + now.toString("yyyy-MM-dd_HH-mm-ss.zzz") + ".log";
    if (!QFile::rename(logFilePath, oldFilePath)) {
      qWarning() << "Не удалось переименовать файл:" << logFilePath;
    }
    logFile.close();
  }

  logFile.setFileName(logFilePath);
  if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "Не удалось открыть файл лога для записи:" << logFilePath;
    return;
  }

  outStream.setDevice(&logFile);
}

Logging::~Logging() {
  if (logFile.isOpen()) {
    logFile.close();
  }
}

Logging& Logging::instance() {
  static Logging instance;
  return instance;
}

void Logging::setLogLevel(LogLevel level) {
  currentLevel = level;
}

void Logging::log(LogLevel level, const QString& message) {
    QMutexLocker locker(&mutex);
  if (level < currentLevel || !logFile.isOpen()) {
    return;
  }

  QString timestamp =
      QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
  QString levelStr = logLevelToString(level);
  QString logMessage =
      QString("[%1] [%2] %3").arg(timestamp, levelStr, message);

  outStream << logMessage << "\n";
  outStream.flush();
  qDebug() << logMessage.toUtf8().constData();
  if(level >= Logging::Warning && level != Logging::Fatal) {
      ErrorHandler::instance().showError(levelStr, message);
  }  else if(level == Logging::Fatal) {
      SystemFatalError::show(message);
  }
}

void Logging::log(LogLevel level, uint32_t code, const QString &details) {
    QString text = ErrorCode::decode(code);
    if (!details.isEmpty()) text += ": " + details;
    log(level, text);
}



QString Logging::logLevelToString(LogLevel level) {
  switch (level) {
    case Debug:
      return "DEBUG";
    case Info:
      return "INFO";
    case Warning:
      return "WARNING";
    case Critical:
      return "CRITICAL";
    case Fatal:
      return "FATAL";
    default:
      return "UNKNOWN";
  }
}
