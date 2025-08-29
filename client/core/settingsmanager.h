#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QSettings>

class SettingsManager
{
public:
    static SettingsManager& instance() {
        static SettingsManager inst("KANEMOOT", "KANEMOOT Corp");
        return inst;
    }

    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    ~SettingsManager() = default;

    void setValue(const QString &key, const QString &value);
    QString getValue(const QString &key, const QString &defaultValue = QString());
    void removeValue(const QString &key);

private:
    SettingsManager(const QString &appName, const QString &orgName);

    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H
