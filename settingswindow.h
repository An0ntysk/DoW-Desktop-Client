#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

class QGridLayout;
class QLineEdit;
class QPushButton;
class QCheckBox;

class SettingsWindow final : public QDialog
{
    QGridLayout *_layout;
    QLineEdit *_searchDir;
    QCheckBox *_darkThemeCheckBox;
    QPushButton *_resetButton, *_selectDefaultDirButton;

private slots:
    void onResetClick();
    void onSelectDefaultDirClick();

public:
    SettingsWindow(QWidget *parent = nullptr);
};

#endif // SETTINGSWINDOW_H
