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

#ifndef XBELTREE_H
#define XBELTREE_H

#include <QDomDocument>
#include <QHash>
#include <QIcon>
#include <QTreeWidget>
#include <QMap>
#include <QStack>
#include <QDataStream>
#include <QFile>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include "config.h"
#include "jspinbox.h"
#include "jcombobox.h"
#include "jlineedit.h"

#define _address 0
#define _comport 1
#define _registers 2
#define _cycle 3
#define _proto 4
#define _maska 5
#define _begin_TU 6
#define _end_TU 7
#define _tic 8
#define _mqpassword 9
#define _potential 10
#define _current 11
#define _powerA 12
#define _powerQ 13
#define _powerS 14
#define _mbus_listorder 0
#define _dcon_listorder 1
#define _mqpassword_length 8
#define _mq_maskasize 32

class XbelTree : public QTreeWidget
{
    Q_OBJECT

    friend class JSpinBox;
    friend class JComboBox;
    friend class JLineEdit;

public:
    XbelTree(QWidget *parent = 0);

    bool read(QIODevice *device);
    bool write(QIODevice *device);
    bool writeConfig(QFile *device);

private slots:
    void updateDomElement(QTreeWidgetItem *item, int column);
    void ItemClicked(QTreeWidgetItem* item, int column);

public slots:
    void AddTS();
    void AddTIT();
    void AddTU();
    void AddMQ();
    int CreateConfig();
    void SaveConfig();

signals:
    void MessageToStatusBar(QString message);

private:
    void parseFolderElement(const QDomElement &element,
                            QTreeWidgetItem *parentItem = 0);
    QTreeWidgetItem *createItem(const QDomElement &element,
                                QTreeWidgetItem *parentItem = 0);
    bool AddBookmarkNode(QDomElement *parent, QString name, QString value, QMap<QString, QString> *properties=NULL);
    bool SetNodeTitle(QDomElement *fold, QString value);
    QDomElement AddFolderNode(QDomElement *parent, QString value, QMap<QString, QString> *properties=NULL);
    void Reset();
    void setRootNode();
    void keyPressEvent ( QKeyEvent * event );
    void RenumDevices();
    void AddTSTIT(int type, QString name);
    int parseTSTITTU(QDomElement* root, unsigned char type);
    int parseTIT(QDomElement* root);
    int parseTU(QDomElement* root);
    int ParseItem( QDomElement *e, QString name );
    int ParseProto(QString proto);
    int MaskaParse(GLOBALSTRUCT *ts, QDomElement *root);
    int SubMaskaParse(GLOBALSTRUCT *ts, QDomElement *root);
    int EnumBitsParse(GLOBALSTRUCT *ts, QDomElement *root);
    int MQPasswordParse(GLOBALSTRUCT *ts, QDomElement *e);
    int MQMaskaParse(GLOBALSTRUCT *ts, QDomElement *e);
    void CreateHeaderFolder(QDomElement *fold, QString name, int type);
    QStack<GLOBALSTRUCT *> stack;
    QDomDocument domDocument;
    QMap<QTreeWidgetItem *, Qt::ItemFlags> MyItemFlags;
    QIcon folderIcon;
    QIcon bookmarkIcon;
    QMap<QString, int> keywords;
    int MerquryCurrentBit;

protected:
    QHash<QTreeWidgetItem *, QDomElement> domElementForItem;
};

#endif
