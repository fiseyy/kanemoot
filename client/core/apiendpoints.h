#ifndef APIENDPOINTS_H
#define APIENDPOINTS_H

#include <QObject>
#include <QMap>
#include <QUrl>

/**
 * @class ApiEndpoints
 * @brief Класс для управления адресами серверов.
 *
 * Использует паттерн одиночка для обеспечения единственного экземпляра.
 */
class ApiEndpoints : public QObject {
    Q_OBJECT

public:
    // Удаление конструктора копирования и оператора присваивания
    ApiEndpoints(const ApiEndpoints &) = delete;
    ApiEndpoints &operator=(const ApiEndpoints &) = delete;

    /**
     * @brief Возвращает единственный экземпляр ApiEndpoints.
     * @return Ссылка на экземпляр ApiEndpoints.
     */
    static ApiEndpoints &instance();

    /**
     * @brief Возвращает URL для указанного сервиса.
     * @param service Название сервиса (например, "auth", "chat").
     * @return URL-адрес сервиса.
     */
    QUrl getEndpoint(const QString &service) const;

    /**
     * @brief Возвращает название сервиса по URL.
     * @param url URL-адрес сервиса
     * @return Название сервиса (например, "auth", "chat").
     */
    QString getServiceName(const QUrl &url) const;

private:
    explicit ApiEndpoints(QObject *parent = nullptr); // Приватный конструктор

    QMap<QString, QUrl> m_endpoints; // Хранилище адресов серверов

    void initializeDefaultEndpoints(); // Инициализация адресов по умолчанию
};

#endif // APIENDPOINTS_H
