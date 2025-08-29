#include "core/settingsmanager.h"

SettingsManager::SettingsManager(const QString &appName, const QString &orgName)
    : m_settings(orgName, appName)
{
}

void SettingsManager::setValue(const QString &key, const QString &value)
{
    m_settings.setValue(key, value);
    m_settings.sync();
}

QString SettingsManager::getValue(const QString &key, const QString &defaultValue)
{
    return m_settings.value(key, defaultValue).toString();
}

void SettingsManager::removeValue(const QString &key)
{
    m_settings.remove(key);
    m_settings.sync();
}
