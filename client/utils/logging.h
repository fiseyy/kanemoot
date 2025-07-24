#ifndef LOGGING_H
#define LOGGING_H

#include <QDateTime>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker>

/**
 * @class Logging
 * @brief Класс для логирования сообщений в программе.
 *
 * Класс предоставляет методы для записи логов в файл с поддержкой разных уровней логирования.
 * Использует паттерн одиночка для обеспечения единственного экземпляра логгера.
 */
class Logging : public QObject {
  Q_OBJECT

 public:
  /**
   * @enum LogLevel
   * Уровни логирования, поддерживаемые в классе Logging.
   */
  enum LogLevel { Debug, Info, Warning, Critical };

  /**
   * @brief Возвращает единственный экземпляр логгера.
   * @return Ссылка на экземпляр Logging.
   */
  static Logging& instance();

  /**
   * @brief Логирует сообщение с указанным уровнем.
   * @param level Уровень логирования.
   * @param message Сообщение для логирования.
   */
  void log(LogLevel level, const QString& message);

  /**
   * @brief Устанавливает уровень логирования.
   * @param level Новый уровень логирования.
   */
  void setLogLevel(LogLevel level);

  /**
   * @brief Конструктор.
   * @param parent Родительский объект для QObject.
   */
  explicit Logging(QObject* parent = nullptr);

  /**
   * @brief Деструктор.
   * Закрывает файл логов.
   */
  ~Logging();

 private:
  QFile logFile;          ///< Файл для хранения логов.
  QTextStream outStream;  ///< Поток вывода для логов.
  LogLevel currentLevel = Info;  ///< Текущий уровень логирования.

  QMutex mutex;

  /**
   * @brief Преобразует уровень логирования в строку.
   * @param level Уровень логирования.
   * @return Строковое представление уровня логирования.
   */
  QString logLevelToString(LogLevel level);
};

/**
 * @brief Макрос для логирования с использованием уровня и сообщения.
 * @param level Уровень логирования.
 * @param msg Сообщение для логирования.
 */
#define LOG(level, msg) Logging::instance().log(Logging::level, msg)
#define SET_LOG_LEVEL(level) Logging::instance().setLogLevel(Logging::level)

#endif
