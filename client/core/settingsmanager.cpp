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

QString SettingsManager::getValue(const QString &key, const QString &defaultValue) const
{
    return m_settings.value(key, defaultValue).toString();
}

void SettingsManager::removeValue(const QString &key)
{
    m_settings.remove(key);
    m_settings.sync();
}

SettingsManager &SettingsManager::instance() {
    static SettingsManager inst("KANEMOOT", "KANEMOOT Corp");
    return inst;
}

bool SettingsManager::autoStart() const {
    return getValue("autoStart", "0") == "1";
}

void SettingsManager::setAutoStart(bool val) {
    if (autoStart() == val) return;
    setValue("autoStart", val ? "1" : "0");
    emit autoStartChanged();
}

bool SettingsManager::enableUpdates() const {
    return getValue("enableUpdates", "0") == "1";
}

void SettingsManager::setEnableUpdates(bool val) {
    if (enableUpdates() == val) return;
    setValue("enableUpdates", val ? "1" : "0");
    emit enableUpdatesChanged();
}
QString SettingsManager::theme() const { return getValue("theme", "dark"); }

void SettingsManager::setTheme(const QString &val) {
    if (theme() == val) return;
    setValue("theme", val);
    emit themeChanged();
}

QString SettingsManager::accentColor() const {
    return getValue("accentColor", "#00C7D9");
}

void SettingsManager::setAccentColor(const QString &val) {
    if (accentColor() == val) return;
    setValue("accentColor", val);
    emit accentColorChanged();
}

bool SettingsManager::notificationSound() const
{
    return getValue("notificationSound", "0") == "1";
}
void SettingsManager::setShowNotifications(bool &val) {
    if (showNotifications() == val)
        return;
    setValue("showNotifications", val ? "1" : "0");
    emit showNotificationsChanged();
}
void SettingsManager::setNotificationSound(bool &val) {
    if (notificationSound() == val)
        return;
    setValue("notificationSound", val ? "1" : "0");
    emit notificationSoundChanged();
}

bool SettingsManager::showNotifications() const
{
    return getValue("showNotifications", "0") == "1";
}
