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
class QProgressBar;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QDialog *_connectDialog;

    QWidget *_content;
    QTableWidget *_fileTable;
    QLabel *_currentProgressText, *_totalProgressText;
    QProgressBar *_currentProgress, *_totalProgress;

    bool _idle;

    void setupMainView();
    void setupConnectView();

private slots:
    void onQueueFilesClicked();
    void onSendFilesClicked();

public:
    MainWindow(QWidget *parent = 0);

    void closeEvent(QCloseEvent *event) override;
};

#endif // _DOW_MAINWINDOW_H_
