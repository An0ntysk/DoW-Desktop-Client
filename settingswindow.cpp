#include "settingswindow.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDir>

void SettingsWindow::onResetClick()
{
    QString homePath
}

void SettingsWindow::onSelectDefaultDirClick()
{
}

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent),
      _layout(new QGridLayout(this)),
      _searchDir(new QLineEdit(this)),
      _darkThemeCheckBox(new QCheckBox(QStringLiteral("Dark theme"), this)),
      _resetButton(new QPushButton(QStringLiteral("Reset"), this)),
      _selectDefaultDirButton(new QPushButton(QStringLiteral("Browse"), this))
{
    setFixedSize(325, 68);
    setWindowModality(Qt::WindowModal);
    setWindowTitle(QStringLiteral("Settings"));
}
