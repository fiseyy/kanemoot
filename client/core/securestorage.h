#ifndef SECURESTORAGE_H
#define SECURESTORAGE_H

#include <QString>
#include <optional>

class SecureStorage
{
public:
    // Получение единственного экземпляра
    static SecureStorage& instance() {
        static SecureStorage inst("KANEMOOT");
        return inst;
    }

    // Удаляем копирование и присваивание
    SecureStorage(const SecureStorage&) = delete;
    SecureStorage& operator=(const SecureStorage&) = delete;

    ~SecureStorage();

    bool setValue(const QString &key, const QString &value);
    std::optional<QString> getValue(const QString &key);
    bool removeValue(const QString &key);


private:
    SecureStorage(const QString &serviceName);
    QString m_serviceName;

#ifdef _WIN32
    bool writeWindowsCredential(const QString &key, const QString &value);
    std::optional<QString> readWindowsCredential(const QString &key);
    bool deleteWindowsCredential(const QString &key);
#elif defined(Q_OS_MACOS)
    bool writeMacKeychain(const QString &key, const QString &value);
    std::optional<QString> readMacKeychain(const QString &key);
    bool deleteMacKeychain(const QString &key);
#else
    QString storageDir() const;
    bool writeLinuxFile(const QString &key, const QString &value);
    std::optional<QString> readLinuxFile(const QString &key);
    bool deleteLinuxFile(const QString &key);
#endif
};

#endif // SECURESTORAGE_H
