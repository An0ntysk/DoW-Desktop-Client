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
#include <QLineEdit>
#include <QCloseEvent>
#include <QHeaderView>
#include <QCheckBox>

#define nameOf(thing) #thing

namespace {
    QString defaultPath;
    bool darkThemeEnabled;
}

void MainWindow::setupMainView()
{
    QWidget *buttonSpacer = new QWidget(_content);
    QWidget *progressContainer = new QWidget(_content);

    _fileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    _fileTable->setHorizontalHeaderLabels({
                                              QStringLiteral("Filename"),
                                              QStringLiteral("Progress"),
                                          });

    QPushButton *connectButton = new QPushButton(QStringLiteral("Connect"), _content);
    QPushButton *queueFilesButton = new QPushButton(QStringLiteral("Queue Files"), _content);
    QPushButton *sendFilesButton = new QPushButton(QStringLiteral("Send Files"), _content);

    buttonSpacer->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
    connectButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    queueFilesButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    sendFilesButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _currentProgressText->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _totalProgressText->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QObject::connect(connectButton, &QPushButton::clicked, _connectDialog, &QDialog::show);
    QObject::connect(queueFilesButton, &QPushButton::clicked, this, &MainWindow::onQueueFilesClicked);
    QObject::connect(sendFilesButton, &QPushButton::clicked, this, &MainWindow::onSendFilesClicked);

    _currentProgress->setMinimum(0);
    _currentProgress->setMaximum(100);
    _currentProgress->setTextVisible(true);

    _totalProgress->setMinimum(0);
    _totalProgress->setMaximum(100);
    _totalProgress->setTextVisible(true);

    QGridLayout *layout = new QGridLayout(_content);
    layout->addWidget(_fileTable, 0, 0, 1, -1);
    layout->addWidget(connectButton, 1, 0);
    layout->addWidget(queueFilesButton, 1, 1);
    layout->addWidget(buttonSpacer, 1, 2, Qt::AlignHCenter);
    layout->addWidget(sendFilesButton, 1, 3);
    layout->addWidget(progressContainer, 2, 0, 1, -1);

    QGridLayout *progressLayout = new QGridLayout(progressContainer);
    progressLayout->addWidget(_currentProgressText, 0, 0);
    progressLayout->addWidget(_currentProgress, 0, 1);
    progressLayout->addWidget(_totalProgressText, 1, 0);
    progressLayout->addWidget(_totalProgress, 1, 1);
    progressLayout->setMargin(0);

    QAction *fileMenuSettings = new QAction(QStringLiteral("Settings"), this);
    fileMenuSettings->setShortcut(QKeySequence::Preferences);
    fileMenuSettings->setStatusTip(QStringLiteral("Adjust some minor things"));
    QObject::connect(fileMenuSettings, &QAction::triggered, _settingsDialog, &QDialog::show);

    QAction *fileMenuExit = new QAction(QStringLiteral("Exit"), this);
    fileMenuExit->setShortcut(QKeySequence::Quit);
    fileMenuExit->setStatusTip(QStringLiteral("Quit DoW"));
    QObject::connect(fileMenuExit, &QAction::triggered, this, &MainWindow::close);

    QAction *helpMenuAbout = new QAction(QStringLiteral("About"), this);
    helpMenuAbout->setStatusTip(QStringLiteral("Information about DoW"));
    QObject::connect(helpMenuAbout, &QAction::triggered, [this](){
        QMessageBox::about(this, QStringLiteral("About"), QStringLiteral("Some info text"));
    });

    QAction *helpMenuAboutQt = new QAction(QStringLiteral("About Qt"), this);
    helpMenuAboutQt->setStatusTip(QStringLiteral("Powered by Qt"));
    QObject::connect(helpMenuAboutQt, &QAction::triggered, [this](){
        QMessageBox::aboutQt(this);
    });

    menuBar()->addMenu(QStringLiteral("File"))->addActions({fileMenuSettings, fileMenuExit});
    menuBar()->addMenu(QStringLiteral("Help"))->addActions({helpMenuAbout, helpMenuAboutQt});
}

void MainWindow::setupSettingsView()
{
    // 325, because I think it looks good
    // 68, because it's the smallest possible height
    _settingsDialog->setFixedSize(325, 68);
    _settingsDialog->setWindowModality(Qt::WindowModal);
    _settingsDialog->setWindowTitle(QStringLiteral("Settings"));

    QLineEdit *searchDir = new QLineEdit(_settingsDialog);
    searchDir->setEnabled(false);
    searchDir->setText(defaultPath);
    searchDir->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);

    QPushButton *selectDefaultDirButton = new QPushButton(QStringLiteral("..."), _settingsDialog);
    selectDefaultDirButton->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
    QObject::connect(selectDefaultDirButton, &QPushButton::clicked, [this, searchDir]() -> void {
        QString newDir = QFileDialog::getExistingDirectory(this, QStringLiteral("Select new default directory"), QDir::homePath(), QFileDialog::Option::ShowDirsOnly);

        if (!newDir.isEmpty()) {
             searchDir->setText(newDir);
        }
    });

    QCheckBox *darkThemeCheckBox = new QCheckBox(QStringLiteral("Come to the dark side"), _settingsDialog);

    QGridLayout *layout = new QGridLayout(_settingsDialog);
    layout->addWidget(searchDir, 0, 0);
    layout->addWidget(selectDefaultDirButton, 0, 1, Qt::AlignRight);
    layout->addWidget(darkThemeCheckBox, 1, 0, 1, -1, Qt::AlignLeft);
}

void MainWindow::setupConnectView()
{
    _connectDialog->setFixedSize(300, 225);
    _connectDialog->setWindowModality(Qt::WindowModal);
    _connectDialog->setWindowTitle(QStringLiteral("Connect to device"));

    QGridLayout *layout = new QGridLayout(_connectDialog);
    (void)layout;
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

MainWindow::MainWindow(QWidget *parent)
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
    (void)darkThemeEnabled; // To silence the 'Unused variable'-warning

    if (!_settings->contains(QStringLiteral(nameOf(defaultPath)))) {
        _settings->setValue(QStringLiteral(nameOf(defaultPath)), QString());
    }

    if (!_settings->contains(QStringLiteral(nameOf(darkThemeEnabled)))) {
        _settings->setValue(QStringLiteral(nameOf(darkThemeEnabled)), false);
    }

    auto val = _settings->value(QStringLiteral(nameOf(defaultPath))).toString();
    defaultPath = val.isEmpty() ? QDir::homePath() : val;

    if (_settings->value(QStringLiteral(nameOf(darkThemeEnabled))).toBool()) {
        // TODO: get a dark theme and set it up
    }

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
