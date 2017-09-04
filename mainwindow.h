/*
 * This File is part of the DoW Desktop Client
 * Copyright (C) 2017  Henning Hoppe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DOW_MAINWINDOW_H_
#define _DOW_MAINWINDOW_H_

#include <QMainWindow>

class QTableWidget;
class QDialog;
class QPushButton;
class QGridLayout;
class QProgressBar;
class QLabel;
class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QDialog *_connectDialog, *_settingsDialog;

    QWidget* _content;
    QTableWidget *_fileTable;
    QLabel *_currentProgressText, *_totalProgressText;
    QProgressBar *_currentProgress, *_totalProgress;
    QSettings* _settings;

    bool _idle;

    void setupMainView();
    void setupSettingsView();
    void setupConnectView();

private slots:
    void onConnectClicked();
    void onQueueFilesClicked();
    void onSendFilesClicked();

    void onFileMenuSettingsClicked();
    void onFileMenuExitClicked();
    void onHelpMenuAboutClicked();
    void onHelpMenuAboutQtClicked();

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

    void closeEvent(QCloseEvent *event) override;
};

#endif // _DOW_MAINWINDOW_H_
