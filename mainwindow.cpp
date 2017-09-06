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
#include <QCheckBox>
#include <QDir>
#include <QMenuBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QTableWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QProgressBar>
#include <QLabel>
#include <QCloseEvent>
#include <QHeaderView>

void MainWindow::setupMainView()
{
    auto buttonSpacer = new QWidget(_content);
    auto progressContainer = new QWidget(_content);
    progressContainer->hide();

    _fileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    _fileTable->setHorizontalHeaderLabels({ QStringLiteral("Filename") });

    auto connectButton = new QPushButton(QStringLiteral("Connect"), _content);
    auto queueFilesButton = new QPushButton(QStringLiteral("Queue Files"), _content);
    auto sendFilesButton = new QPushButton(QStringLiteral("Send Files"), _content);

    buttonSpacer->setSizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
    connectButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    queueFilesButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    sendFilesButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _currentProgressText->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    _totalProgressText->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QObject::connect(connectButton, &QPushButton::clicked, _connectDialog, &QDialog::show);
    QObject::connect(queueFilesButton, &QPushButton::clicked, this, &MainWindow::onQueueFilesClicked);
    QObject::connect(sendFilesButton, &QPushButton::clicked, this, &MainWindow::onSendFilesClicked);
    QObject::connect(sendFilesButton, &QPushButton::clicked, progressContainer, &QWidget::show);

    _currentProgress->setMinimum(0);
    _currentProgress->setMaximum(100);
    _currentProgress->setTextVisible(true);

    _totalProgress->setMinimum(0);
    _totalProgress->setMaximum(100);
    _totalProgress->setTextVisible(true);

    auto layout = new QGridLayout(_content);
    layout->addWidget(_fileTable, 0, 0, 1, -1);
    layout->addWidget(connectButton, 1, 0);
    layout->addWidget(queueFilesButton, 1, 1);
    layout->addWidget(buttonSpacer, 1, 2, Qt::AlignHCenter);
    layout->addWidget(sendFilesButton, 1, 3);
    layout->addWidget(progressContainer, 2, 0, 1, -1);

    auto progressLayout = new QGridLayout(progressContainer);
    progressLayout->addWidget(_currentProgressText, 0, 0);
    progressLayout->addWidget(_currentProgress, 0, 1);
    progressLayout->addWidget(_totalProgressText, 1, 0);
    progressLayout->addWidget(_totalProgress, 1, 1);
    progressLayout->setMargin(0);

    auto fileMenuExit = new QAction(QStringLiteral("Exit"), this);
    fileMenuExit->setShortcut(QKeySequence::Quit);
    fileMenuExit->setStatusTip(QStringLiteral("Quit DoW"));
    QObject::connect(fileMenuExit, &QAction::triggered, this, &MainWindow::close);

    auto helpMenuAbout = new QAction(QStringLiteral("About"), this);
    helpMenuAbout->setStatusTip(QStringLiteral("Information about DoW"));
    QObject::connect(helpMenuAbout, &QAction::triggered, [this](){
        QMessageBox::about(this, QStringLiteral("About"), QStringLiteral("Some info text"));
    });

    auto helpMenuAboutQt = new QAction(QStringLiteral("About Qt"), this);
    helpMenuAboutQt->setStatusTip(QStringLiteral("Powered by Qt"));
    QObject::connect(helpMenuAboutQt, &QAction::triggered, [this](){
        QMessageBox::aboutQt(this);
    });

    menuBar()->addMenu(QStringLiteral("File"))->addActions({ fileMenuExit });
    menuBar()->addMenu(QStringLiteral("Help"))->addActions({ helpMenuAbout, helpMenuAboutQt });
}

void MainWindow::setupConnectView()
{
    _connectDialog->setFixedSize(300, 225);
    _connectDialog->setWindowModality(Qt::WindowModal);
    _connectDialog->setWindowTitle(QStringLiteral("Connect to device"));

    auto layout = new QGridLayout(_connectDialog);
    (void)layout;
}

void MainWindow::onQueueFilesClicked()
{
    auto files = QFileDialog::getOpenFileNames(this, QStringLiteral("Select files"), QDir::homePath(), QStringLiteral("All files (*.*)"));

    for (const auto& file : files) {
        int rowCount = _fileTable->rowCount();
        _fileTable->insertRow(rowCount);
        _fileTable->setCellWidget(rowCount, 0, new QLabel(file, _fileTable));
    }
}

void MainWindow::onSendFilesClicked()
{
    _totalProgress->setValue(50);
    _idle = false;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _connectDialog(new QDialog(this)),
      _content(new QWidget(this)),
      _fileTable(new QTableWidget(0, 1, this)),
      _currentProgressText(new QLabel(QStringLiteral("File 0/0"), _content)),
      _totalProgressText(new QLabel(QStringLiteral("Total Progress"), _content)),
      _currentProgress(new QProgressBar(_content)),
      _totalProgress(new QProgressBar(_content)),
      _idle(true)
{
    setupMainView();
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
