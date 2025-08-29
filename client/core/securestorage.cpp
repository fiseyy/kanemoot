#include "core/securestorage.h"

#include <QStandardPaths>
#include <QFile>
#include <QDir>

#ifdef _WIN32
#include <windows.h>
#include <wincred.h>
#else
#include <QCryptographicHash>
#endif

SecureStorage::SecureStorage(const QString &serviceName)
    : m_serviceName(serviceName)
{
}

SecureStorage::~SecureStorage()
{
}

bool SecureStorage::setValue(const QString &key, const QString &value)
{
#ifdef _WIN32
    return writeWindowsCredential(key, value);
#elif defined(Q_OS_MACOS)
    return writeMacKeychain(key, value);
#else
    return writeLinuxFile(key, value);
#endif
}

std::optional<QString> SecureStorage::getValue(const QString &key)
{
#ifdef _WIN32
    return readWindowsCredential(key);
#elif defined(Q_OS_MACOS)
    return readMacKeychain(key);
#else
    return readLinuxFile(key);
#endif
}

bool SecureStorage::removeValue(const QString &key)
{
#ifdef _WIN32
    return deleteWindowsCredential(key);
#elif defined(Q_OS_MACOS)
    return deleteMacKeychain(key);
#else
    return deleteLinuxFile(key);
#endif
}


#ifdef _WIN32
bool SecureStorage::writeWindowsCredential(const QString &key, const QString &value)
{
    CREDENTIAL cred{};
    QByteArray valBytes = value.toUtf8();
    QByteArray targetName = (m_serviceName + ":" + key).toUtf8();

    cred.Type = CRED_TYPE_GENERIC;
    cred.TargetName = (LPWSTR)targetName.constData();
    cred.CredentialBlob = (LPBYTE)valBytes.constData();
    cred.CredentialBlobSize = valBytes.size();
    cred.Persist = CRED_PERSIST_LOCAL_MACHINE;
    cred.UserName = nullptr;

    return CredWriteW(&cred, 0);
}

std::optional<QString> SecureStorage::readWindowsCredential(const QString &key)
{
    QByteArray targetName = (m_serviceName + ":" + key).toUtf8();
    CREDENTIAL *pcred = nullptr;
    if (CredReadW((LPCWSTR)targetName.constData(), CRED_TYPE_GENERIC, 0, &pcred))
    {
        QString value = QString::fromUtf8((char*)pcred->CredentialBlob, pcred->CredentialBlobSize);
        CredFree(pcred);
        return value;
    }
    return std::nullopt;
}

bool SecureStorage::deleteWindowsCredential(const QString &key)
{
    QByteArray targetName = (m_serviceName + ":" + key).toUtf8();
    return CredDeleteW((LPCWSTR)targetName.constData(), CRED_TYPE_GENERIC, 0);
}

#elif defined(Q_OS_MACOS)
#include <QProcess>

bool SecureStorage::writeMacKeychain(const QString &key, const QString &value)
{
    QString serviceKey = m_serviceName + ":" + key;
    QProcess proc;
    QStringList args = { "add-generic-password", "-a", serviceKey, "-s", m_serviceName, "-w", value, "-U" };
    proc.start("security", args);
    proc.waitForFinished();
    return proc.exitCode() == 0;
}

std::optional<QString> SecureStorage::readMacKeychain(const QString &key)
{
    QString serviceKey = m_serviceName + ":" + key;
    QProcess proc;
    QStringList args = { "find-generic-password", "-a", serviceKey, "-s", m_serviceName, "-w" };
    proc.start("security", args);
    proc.waitForFinished();
    if (proc.exitCode() != 0)
        return std::nullopt;
    QString result = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
    return result;
}

bool SecureStorage::deleteMacKeychain(const QString &key)
{
    QString serviceKey = m_serviceName + ":" + key;
    QProcess proc;
    QStringList args = { "delete-generic-password", "-a", serviceKey, "-s", m_serviceName };
    proc.start("security", args);
    proc.waitForFinished();
    return proc.exitCode() == 0;
}


#else // Linux

QString SecureStorage::storageDir() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir;
}

bool SecureStorage::writeLinuxFile(const QString &key, const QString &value)
{
    QString filename = storageDir() + "/" + QString(QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256).toHex());
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(value.toUtf8());
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    file.close();
    return true;
}

std::optional<QString> SecureStorage::readLinuxFile(const QString &key)
{
    QString filename = storageDir() + "/" + QString(QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256).toHex());
    QFile file(filename);
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
        return std::nullopt;
    QByteArray data = file.readAll();
    file.close();
    return QString::fromUtf8(data);
}

bool SecureStorage::deleteLinuxFile(const QString &key)
{
    QString filename = storageDir() + "/" + QString(QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256).toHex());
    return QFile::remove(filename);
}


#endif
