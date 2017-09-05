#ifndef SETTINGS_H
#define SETTINGS_H

#include <QApplication>
#include <QSettings>

class Settings final
{
    inline static const QSettings& instance()
    {
        static QSettings settings(QSettings::Format::NativeFormat, QSettings::Scope::UserScope, QApplication::organizationName(), QApplication::applicationName());
        return settings;
    }

public:
    Settings() = delete;
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator = (const Settings&) = delete;
    Settings& operator = (Settings&&) = delete;

    inline static void resetToDefault()
    {
        instance().clear();
    }

    inline static void setValue(const QString& key, const QVariant& value)
    {
        instance().setValue(key, value);
    }

    inline static QVariant getValue(const QString& key)
    {
        return instance().value(key);
    }

    inline static bool isSet(const QString& key)
    {
        return instance().contains(key);
    }
};

#endif // SETTINGS_H
