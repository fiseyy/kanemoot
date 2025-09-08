#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QSettings>
#include <QObject>
class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(bool enableUpdates READ enableUpdates WRITE setEnableUpdates NOTIFY enableUpdatesChanged)
    Q_PROPERTY(QString accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool notificationSound READ notificationSound WRITE setNotificationSound NOTIFY notificationSoundChanged)
    Q_PROPERTY(bool showNotifications READ showNotifications WRITE setShowNotifications NOTIFY showNotificationsChanged)

public:
    static SettingsManager &instance();

    SettingsManager(const SettingsManager &) = delete;
    SettingsManager &operator=(const SettingsManager &) = delete;

    ~SettingsManager() = default;

    bool autoStart() const;
    void setAutoStart(bool val);

    bool enableUpdates() const;
    void setEnableUpdates(bool val);

    QString theme() const;
    void setTheme(const QString &val);

    QString accentColor() const;
    void setAccentColor(const QString &val);

    bool notificationSound() const;
    void setNotificationSound(bool &val);

    bool showNotifications() const;
    void setShowNotifications(bool &val);

    void setValue(const QString &key, const QString &value);
    QString getValue(const QString &key, const QString &defaultValue = QString()) const;
    void removeValue(const QString &key);

signals:
    void autoStartChanged();
    void enableUpdatesChanged();
    void accentColorChanged();
    void themeChanged();
    void notificationSoundChanged();
    void showNotificationsChanged();
private:
    SettingsManager(const QString &appName, const QString &orgName);
    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H
