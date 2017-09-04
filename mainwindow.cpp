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

#include "mainwindow.h"

#include <QAction>
#include <QMenuBar>
#include <QApplication>
#include <QStatusBar>
#include <QGridLayout>
#include <QTableWidget>
#include <QFileDialog>
#include <QDir>
#include <QPushButton>
#include <QMessageBox>
#include <QProgressBar>
#include <QLabel>
#include <QSettings>
#include <QCloseEvent>

#define nameOf(thing) #thing

namespace {
    QString defaultPath;
    bool darkThemeEnabled;
}

void MainWindow::setupMainView()
{
    QWidget* _buttonSpacer = new QWidget(_content);
    QWidget* _progressContainer = new QWidget(_content);

    _fileTable->setHorizontalHeaderLabels({
                                              QStringLiteral("Filename"),
                                              QStringLiteral("Progress"),
                                          });

    QPushButton *_connectButton = new QPushButton(QStringLiteral("Connect"), _content);
    QPushButton* _queueFilesButton = new QPushButton(QStringLiteral("Queue Files"), _content);
    QPushButton* _sendFilesButton = new QPushButton(QStringLiteral("Send Files"), _content);

    _buttonSpacer->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
    _connectButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _queueFilesButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _sendFilesButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _currentProgressText->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _totalProgressText->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QObject::connect(_connectButton, &QAbstractButton::clicked, this, &MainWindow::onConnectClicked);
    QObject::connect(_queueFilesButton, &QAbstractButton::clicked, this, &MainWindow::onQueueFilesClicked);
    QObject::connect(_sendFilesButton, &QAbstractButton::clicked, this, &MainWindow::onSendFilesClicked);

    _currentProgress->setMinimum(0);
    _currentProgress->setMaximum(100);
    _currentProgress->setTextVisible(true);

    _totalProgress->setMinimum(0);
    _totalProgress->setMaximum(100);
    _totalProgress->setTextVisible(true);

    QGridLayout* layout = new QGridLayout(_content);
    layout->addWidget(_fileTable, 0, 0, 1, -1);
    layout->addWidget(_connectButton, 1, 0);
    layout->addWidget(_queueFilesButton, 1, 1);
    layout->addWidget(_buttonSpacer, 1, 2, Qt::AlignHCenter);
    layout->addWidget(_sendFilesButton, 1, 3);
    layout->addWidget(_progressContainer, 2, 0, 1, -1);

    QGridLayout* _progressLayout = new QGridLayout(_progressContainer);
    _progressLayout->addWidget(_currentProgressText, 0, 0);
    _progressLayout->addWidget(_currentProgress, 0, 1);
    _progressLayout->addWidget(_totalProgressText, 1, 0);
    _progressLayout->addWidget(_totalProgress, 1, 1);
    _progressLayout->setMargin(0);

    QAction* _fileMenuSettings = new QAction(QStringLiteral("Settings"), this);
    _fileMenuSettings->setShortcut(QKeySequence::Preferences);
    _fileMenuSettings->setStatusTip(QStringLiteral("Adjust some minor things"));
    QObject::connect(_fileMenuSettings, &QAction::triggered, this, &MainWindow::onFileMenuSettingsClicked);

    QAction* _fileMenuExit = new QAction(QStringLiteral("Exit"), this);
    _fileMenuExit->setShortcut(QKeySequence::Quit);
    _fileMenuExit->setStatusTip(QStringLiteral("Quit DoW"));
    QObject::connect(_fileMenuExit, &QAction::triggered, this, &MainWindow::onFileMenuExitClicked);

    QAction* _helpMenuAbout = new QAction(QStringLiteral("About"), this);
    _helpMenuAbout->setStatusTip(QStringLiteral("Information about DoW"));
    QObject::connect(_helpMenuAbout, &QAction::triggered, this, &MainWindow::onHelpMenuAboutClicked);

    QAction* _helpMenuAboutQt = new QAction(QStringLiteral("About Qt"), this);
    _helpMenuAboutQt->setStatusTip(QStringLiteral("Powered by Qt"));
    QObject::connect(_helpMenuAboutQt, &QAction::triggered, this, &MainWindow::onHelpMenuAboutQtClicked);

    menuBar()->addMenu(QStringLiteral("File"))->addActions({_fileMenuSettings, _fileMenuExit});
    menuBar()->addMenu(QStringLiteral("Help"))->addActions({_helpMenuAbout, _helpMenuAboutQt});
}

void MainWindow::setupSettingsView()
{
    QGridLayout* layout = new QGridLayout(_connectDialog);
}

void MainWindow::setupConnectView()
{

}

void MainWindow::onConnectClicked()
{

}

void MainWindow::onQueueFilesClicked()
{
    QFileDialog::getOpenFileNames(this, QStringLiteral("Select files"), defaultPath, QStringLiteral("All files (*.*)"));
}

void MainWindow::onSendFilesClicked()
{
    _totalProgress->setValue(50);
    _idle = false;
}

void MainWindow::onFileMenuSettingsClicked()
{
    _settingsDialog->show();
}

void MainWindow::onFileMenuExitClicked()
{
    close();
}

void MainWindow::onHelpMenuAboutClicked()
{
    QMessageBox::about(this, QStringLiteral("About"), QStringLiteral("Some info text"));
}

void MainWindow::onHelpMenuAboutQtClicked()
{
    QMessageBox::aboutQt(this);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      _connectDialog(new QDialog(this)),
      _settingsDialog(new QDialog(this)),
      _content(new QWidget(this)),
      _fileTable(new QTableWidget(0, 2, this)),
      _currentProgressText(new QLabel(QStringLiteral("File 0/0"), _content)),
      _totalProgressText(new QLabel(QStringLiteral("Total Progress"), _content)),
      _currentProgress(new QProgressBar(_content)),
      _totalProgress(new QProgressBar(_content)),
      _settings(new QSettings(QSettings::Format::NativeFormat, QSettings::Scope::UserScope, QApplication::organizationName(), QApplication::applicationName(), this)),
      _idle(true)
{
    if (!_settings->contains(QStringLiteral(nameOf(defaultPath)))) {
        _settings->setValue(QStringLiteral(nameOf(defaultPath)), QString());
    }

    if (!_settings->contains(QStringLiteral(nameOf(darkThemeEnabled)))) {
        _settings->setValue(QStringLiteral(nameOf(darkThemeEnabled)), false);
    }

    auto val = _settings->value(QStringLiteral(nameOf(defaultPath))).toString();
    defaultPath = val == QString() ? QDir::homePath() : val;

    if (_settings->value(QStringLiteral(nameOf(darkThemeEnabled))).toBool()) {
        // TODO: get a dark theme and apply it when necessary
    }

    _connectDialog->setWindowModality(Qt::WindowModal);
    _settingsDialog->setWindowModality(Qt::WindowModal);

    setupMainView();
    setupSettingsView();
    setupConnectView();

    statusBar()->showMessage(QStringLiteral("Not connected"));
    setCentralWidget(_content);
    setWindowIcon(QIcon(QStringLiteral(":/icon.ico")));
    setWindowTitle(QStringLiteral("Data-over-WiFi Desktop"));
    setMinimumSize(400, 300);
    resize(minimumSize());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_idle) {
        event->accept();
    }
    else if (QMessageBox::warning(this,
                                  QStringLiteral("Warning!"),
                                  QStringLiteral("There is a transmission in progress. If you quit now, all unsent data won't be transmitted.\n\nQuit anyway?"),
                                  QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                                  QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes) {
        event->accept();
    }
    else {
        event->ignore();
    }
}
