/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "xbeltree.h"

MainWindow::MainWindow()
{
    xbelTree = new XbelTree;
    xbelTree->setStyleSheet( " QTreeWidget { font: bold italic large; background-color: #eee; } "
                             " QTreeWidget { background-image: url(:/images/chip5.png); } ");
    setCentralWidget(xbelTree);

    createActions();
    createMenus();

    statusBar()->showMessage(tr("Ready"));

    setWindowTitle(tr("DOM Bookmarks"));
    resize(480, 320);
}

void MainWindow::open()
{
#if defined(Q_OS_SYMBIAN)
    // Look for bookmarks on the same drive where the application is installed to,
    // if drive is not read only. QDesktopServices::DataLocation does this check,
    // and returns writable drive.
    QString bookmarksFolder =
            QDesktopServices::storageLocation(QDesktopServices::DataLocation).left(1);
    bookmarksFolder.append(":/Data/qt/saxbookmarks");
    QDir::setCurrent(bookmarksFolder);
#endif
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open Bookmark File"),
                                         QDir::currentPath(),
                                         tr("XBEL Files (*.xbel *.xml)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SAX Bookmarks"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    if (xbelTree->read(&file))
        statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::saveAs()
{
#if defined(Q_OS_SYMBIAN)
    // Look for bookmarks on the same drive where the application is installed to,
    // if drive is not read only. QDesktopServices::DataLocation does this check,
    // and returns writable drive.
    QString bookmarksFolder =
            QDesktopServices::storageLocation(QDesktopServices::DataLocation).left(1);
    bookmarksFolder.append(":/Data/qt/saxbookmarks");
    QDir::setCurrent(bookmarksFolder);
#endif
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save Bookmark File"),
                                         QDir::currentPath(),
                                         tr("XBEL Files (*.xbel *.xml)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SAX Bookmarks"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    if (xbelTree->write(&file))
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveConfigAs()
{
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save config i7888 File"),
                                         QDir::currentPath(),
                                         tr("DAT Files (*.dat)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SAX Bookmarks"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    if (xbelTree->writeConfig(&file))
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About DOM Bookmarks"),
                      tr("The <b>DOM Bookmarks</b> example demonstrates how to "
                         "use Qt's DOM classes to read and write XML "
                         "documents."));
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAsAct = new QAction(tr("&Save As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));


    addTSAct = new QAction(QString::fromUtf8("Устройство ТС"), this);
    connect(addTSAct, SIGNAL(triggered()), xbelTree, SLOT(AddTS()));
    addTSAct->setShortcut(QKeySequence(tr("Ctrl+S")));

    addTITAct = new QAction(QString::fromUtf8("Устройство ТИТ"), this);
    connect(addTITAct, SIGNAL(triggered()), xbelTree, SLOT(AddTIT()));
    addTITAct->setShortcut(QKeySequence(tr("Ctrl+T")));

    addTUAct = new QAction(QString::fromUtf8("Устройство ТУ"), this);
    connect(addTUAct, SIGNAL(triggered()), xbelTree, SLOT(AddTU()));
    addTUAct->setShortcut(QKeySequence(tr("Ctrl+U")));

    addMQAct = new QAction(QString::fromUtf8("Счетчик Меркурий"), this);
    connect(addMQAct, SIGNAL(triggered()), xbelTree, SLOT(AddMQ()));
    addMQAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

    createConfig = new QAction(QString::fromUtf8("Создать конфигурацию"), this);
    connect(createConfig, SIGNAL(triggered()), xbelTree, SLOT(CreateConfig()));
    createConfig->setShortcut(QKeySequence(tr("Ctrl+M")));

    saveConfig = new QAction(QString::fromUtf8("Сохранить конфигурацию"), this);
    connect(saveConfig, SIGNAL(triggered()), this, SLOT(saveConfigAs()));
    saveConfig->setShortcut(QKeySequence(tr("Shift+Ctrl+C")));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(xbelTree, SIGNAL(MessageToStatusBar(QString)), this, SLOT(MessageToStatusBar(QString)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(exitAct);

    menuBar()->addSeparator();

    addMenu = menuBar()->addMenu(QString::fromUtf8("Добавить устройство"));
    addMenu->addAction(addTSAct);
    addMenu->addAction(addTITAct);
    addMenu->addAction(addTUAct);
    addMenu->addAction(addMQAct);

    menuBar()->addSeparator();

    addMenu = menuBar()->addMenu(QString::fromUtf8("Создать конфигурацию"));
    addMenu->addAction(createConfig);
    addMenu->addAction(saveConfig);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::MessageToStatusBar(QString message)
{
    statusBar()->showMessage(message);
}
