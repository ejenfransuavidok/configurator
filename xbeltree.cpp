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
#include <QDebug>

#include "xbeltree.h"

static int devices = 0;

XbelTree::XbelTree(QWidget *parent)
    : QTreeWidget(parent)
{
    QStringList labels;
    labels << QString::fromUtf8("Наименование параметра") << QString::fromUtf8("Значение параметра");
    header()->setResizeMode(QHeaderView::Stretch);
    header()->setStyleSheet( "font: bold italic; background-color: green;" );
    setHeaderLabels(labels);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
    keywords["address"] = _address;
    keywords["comport"] = _comport;
    keywords["registers"] = _registers;
    keywords["cycle"] = _cycle;
    keywords["proto"] = _proto;
    keywords["maska"] = _maska;
    keywords["begin_TU"] = _begin_TU;
    keywords["end_TU"] = _end_TU;
    keywords["tic"] = _tic;
    keywords["mqpassword"] = _mqpassword;
    keywords["potential"] = _potential;
    keywords["current"] = _current;
    keywords["power-A"] = _powerA;
    keywords["power-Q"] = _powerQ;
    keywords["power-S"] = _powerS;
}

bool XbelTree::read(QIODevice *device)
{
    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!domDocument.setContent(device, true, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::information(window(), tr("DOM Bookmarks"),
                                 tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return false;
    }
    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "xbel") {
        QMessageBox::information(window(), tr("DOM Bookmarks"),
                                 tr("The file is not an XBEL file."));
        return false;
    } else if (root.hasAttribute("version")
               && root.attribute("version") != "1.0") {
        QMessageBox::information(window(), tr("DOM Bookmarks"),
                                 tr("The file is not an XBEL version 1.0 "
                                    "file."));
        return false;
    }
    Reset();
    return true;
}

bool XbelTree::write(QIODevice *device)
{
    const int IndentSize = 4;
    QTextStream out(device);
    domDocument.save(out, IndentSize);
    return true;
}

bool XbelTree::writeConfig(QFile *device)
{
    FILE * file= fopen(device->fileName().toLocal8Bit().data(), "wb");
    while (!stack.isEmpty()) {
        GLOBALSTRUCT* ts = stack.pop();
        fwrite(ts, sizeof(GLOBALSTRUCT), 1, file);
        delete( ts );
        ts = 0;
    }
    fclose(file);
    emit(MessageToStatusBar(QString::fromUtf8("конфигурация сохранена")));
    return true;
}

void XbelTree::updateDomElement(QTreeWidgetItem *item, int column)
{
    qDebug() << "changed!!!";
}

void XbelTree::parseFolderElement(const QDomElement &element,
                                  QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item = createItem(element, parentItem);
    QString title = element.firstChildElement("title").text();
    if (title.isEmpty())
        title = QObject::tr("Folder");
    item->setFlags(item->flags());
    item->setIcon(0, folderIcon);
    if( !domElementForItem[item].attribute("device").isNull() ) {
        if( QString(domElementForItem[item].attribute("device")).toInt() >= devices )
            devices++;
        item->setText(0, QString::fromUtf8("%1 - %2").arg(title).arg(domElementForItem[item].attribute("device")));
    }
    else
        item->setText(0, title);
    bool folded = (element.attribute("folded") != "no");
    setItemExpanded(item, !folded);
    QDomElement child = element.firstChildElement();
    while (!child.isNull()) {
        if (child.tagName() == "folder") {
            parseFolderElement(child, item);
        } else if (child.tagName() == "bookmark") {
            QTreeWidgetItem *childItem = createItem(child, item);
            QString title = child.firstChildElement("title").text();
            if (title.isEmpty())
                title = QObject::tr("Folder");
            childItem->setFlags(item->flags());
            childItem->setIcon(0, bookmarkIcon);
            childItem->setText(0, title);
            if( child.attribute("property") == "clicked" ) {
                childItem->setFlags(childItem->flags() | Qt::ItemIsUserCheckable);
                if( child.attribute("href") == "1" )
                    childItem->setCheckState(1, Qt::Checked);
                else
                    childItem->setCheckState(1, Qt::Unchecked);
            }
            if( child.attribute("name") == "proto") {
                JComboBox *cmb = new JComboBox(childItem);
                QString variants = child.attribute("variants");
                QStringList qsl = variants.split("|");
                int currentIndex = child.attribute("href").toInt();
                for(int i=0; i<qsl.size(); i++) {
                    cmb->addItem( qsl[i] );
                }
                cmb->setCurrentIndex(currentIndex);
                this->setItemWidget( childItem, 1, cmb );
                childItem->setIcon(0, QIcon(":/images/protocol.png"));
            }
            if( child.attribute("name") == "cycle") {
                childItem->setIcon(0, QIcon(":/images/cycle.png"));
            }
            if( child.attribute("name") == "registers") {
                childItem->setIcon(0, QIcon(":/images/registers.png"));
            }
            if( child.attribute("name") == "comport") {
                childItem->setIcon(0, QIcon(":/images/com_port.png"));
            }
            if( child.attribute("name") == "address") {
                childItem->setIcon(0, QIcon(":/images/address.png"));
            }
            if( child.attribute("name") == "mbit") {
                childItem->setIcon(0, QIcon(":/images/switch.png"));
            }
            if( child.attribute("name") == "mqpassword") {
                JLineEdit *qle = new JLineEdit(childItem);
                qle->setMaxLength(_mqpassword_length);
                qle->setEchoMode(QLineEdit::Password);
                qle->setText(child.attribute("href"));
                this->setItemWidget( childItem, 1, qle);
                childItem->setIcon(0, QIcon(":/images/password.png"));
            }
            if( child.attribute("name") == "registers"||child.attribute("name") == "cycle"||child.attribute("name") == "comport"||child.attribute("name") == "address"||child.attribute("name") == "begin_TU"||child.attribute("name") == "end_TU"||child.attribute("name") == "tic") {
                JSpinBox *sbx = new JSpinBox(childItem);
                sbx->setMaximum(0xffff);
                sbx->setMinimum(0);
                sbx->setValue(child.attribute("href").toInt());
                this->setItemWidget( childItem, 1, sbx );
            }
            if( (!child.attributeNode("ico").isNull())&&(child.attribute("ico") != "") ) {
                childItem->setIcon(0, QIcon(child.attribute("ico")));
            }
        } else if (child.tagName() == "separator") {
            QTreeWidgetItem *childItem = createItem(child, item);
            childItem->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
            childItem->setText(0, QString(30, 0xB7));
        }
        child = child.nextSiblingElement();
    }
}

QTreeWidgetItem *XbelTree::createItem(const QDomElement &element,
                                      QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item;
    if (parentItem) {
        item = new QTreeWidgetItem(parentItem);
    } else {
        item = new QTreeWidgetItem(this);
    }
    domElementForItem.insert(item, element);
    return item;
}

void XbelTree::AddTS()
{
    AddTSTIT(TS, QString::fromUtf8("Устройство телесигнализации"));
    emit(MessageToStatusBar(QString::fromUtf8("Добавлено устройство ТС")));
}

void XbelTree::AddTIT()
{
    AddTSTIT(TIT, QString::fromUtf8("Устройство телеизмерений"));
    emit(MessageToStatusBar(QString::fromUtf8("Добавлено устройство ТИТ")));
}

void XbelTree::AddMQ() {
    QDomElement *fold = new QDomElement();
    CreateHeaderFolder(fold, QString::fromUtf8("Счетчик электроэнергии Меркурий"), MERQURY);
    QMap<QString, QString> properties;
    properties["name"] = "address";
    AddBookmarkNode(fold, QString::fromUtf8("адрес"), "0x01", &properties);
    properties["name"] = "comport";
    AddBookmarkNode(fold, QString::fromUtf8("компорт"), "2", &properties);
    properties["name"] = "cycle";
    AddBookmarkNode(fold, QString::fromUtf8("цикл опроса"), "3000", &properties);
    properties["name"] = "mqpassword";
    AddBookmarkNode(fold, QString::fromUtf8("пароль"), "000000", &properties);
    QString characteristics[23][5] = {
        {QString::fromUtf8("Напряжения"),                   "U",  "potential",  "folder",   ""},
        {QString::fromUtf8("Напряжение фазы A"),            "UA", "potential",  "clicked",  ":/images/U.png"},
        {QString::fromUtf8("Напряжение фазы B"),            "UB", "potential",  "clicked",  ":/images/U.png"},
        {QString::fromUtf8("Напряжение фазы C"),            "UC", "potential",  "clicked",  ":/images/U.png"},
        {QString::fromUtf8("Токи"),                         "I",  "current",    "folder",   ""},
        {QString::fromUtf8("Ток фазы A"),                   "IA", "current",    "clicked",  ":/images/I.png"},
        {QString::fromUtf8("Ток фазы B"),                   "IB", "current",    "clicked",  ":/images/I.png"},
        {QString::fromUtf8("Ток фазы C"),                   "IC", "current",    "clicked",  ":/images/I.png"},
        {QString::fromUtf8("Активные мощности"),            "P",  "power-A",    "folder",   ""},
        {QString::fromUtf8("Активная мощность фазы A"),     "PA", "power-A",    "clicked",  ":/images/P.png"},
        {QString::fromUtf8("Активная мощность фазы B"),     "PB", "power-A",    "clicked",  ":/images/P.png"},
        {QString::fromUtf8("Активная мощность фазы C"),     "PC", "power-A",    "clicked",  ":/images/P.png"},
        {QString::fromUtf8("Суммарная активная мощность"),  "SP", "power-A",    "clicked",  ":/images/P.png"},
        {QString::fromUtf8("Реактивные мощности"),          "Q",  "power-Q",    "folder",   ""},
        {QString::fromUtf8("Реактивная мощность фазы A"),   "QA", "power-Q",    "clicked",  ":/images/Q.png"},
        {QString::fromUtf8("Реактивная мощность фазы B"),   "QB", "power-Q",    "clicked",  ":/images/Q.png"},
        {QString::fromUtf8("Реактивная мощность фазы C"),   "QC", "power-Q",    "clicked",  ":/images/Q.png"},
        {QString::fromUtf8("Суммарная реактивная мощность"),"SQ", "power-Q",    "clicked",  ":/images/Q.png"},
        {QString::fromUtf8("Полные мощности"),              "S",  "power-S",    "folder",   ""},
        {QString::fromUtf8("Полная мощность фазы A"),       "SA", "power-S",    "clicked",  ":/images/S.png"},
        {QString::fromUtf8("Полная мощность фазы B"),       "SB", "power-S",    "clicked",  ":/images/S.png"},
        {QString::fromUtf8("Полная мощность фазы C"),       "SC", "power-S",    "clicked",  ":/images/S.png"},
        {QString::fromUtf8("Суммарная полная мощность"),    "SS", "power-S",    "clicked",  ":/images/S.png"},
    };
    QDomElement folder;
    for(int i=0; i<23; i++) {
        if( characteristics[i][3] == "folder" ) {
            properties.clear();
            properties["name"] = characteristics[i][2];
            folder = AddFolderNode(fold, characteristics[i][0], &properties);
        }
        else {
            properties.clear();
            properties["name"] = characteristics[i][2];
            properties["property"] = characteristics[i][3];
            properties["phase"] = characteristics[i][1];
            properties["ico"] = characteristics[i][4];
            AddBookmarkNode(&folder, characteristics[i][0], "", &properties);
        }
    }
    Reset();
    emit(MessageToStatusBar(QString::fromUtf8("Добавлено устройство Меркурий")));
}

void XbelTree::CreateHeaderFolder(QDomElement *fold, QString name, int type) {
    if( domDocument.isNull() ){
        setRootNode();
    }
    QDomElement root = domDocument.documentElement();
    *fold = domDocument.createElement("folder");
    QDomElement title = domDocument.createElement("title");
    QDomText text = domDocument.createTextNode(name);
    title.appendChild(text);
    fold->appendChild(title);
    fold->setAttribute("folded", "yes");
    fold->setAttribute("device", devices++);
    fold->setAttribute("type", type);
    root.appendChild(*fold);
}

void XbelTree::AddTSTIT(int type, QString name)
{
    QDomElement *fold = new QDomElement();
    CreateHeaderFolder(fold, name, type);
    QMap<QString, QString> properties;
    properties["name"] = "address";
    AddBookmarkNode(fold, QString::fromUtf8("адрес"), "0x01", &properties);
    properties["name"] = "comport";
    AddBookmarkNode(fold, QString::fromUtf8("компорт"), "2", &properties);
    properties["name"] = "registers";
    AddBookmarkNode(fold, QString::fromUtf8("регистры"), "5", &properties);
    properties["name"] = "cycle";
    AddBookmarkNode(fold, QString::fromUtf8("цикл опроса"), "3000", &properties);
    properties.clear();
    properties["name"] = "proto";
    properties["variants"] = "MODBUS|DCON";
    AddBookmarkNode(fold, QString::fromUtf8("протокол"), "MODBUS", &properties);
    properties.clear();
    properties["name"] = "maska";
    QDomElement mask = AddFolderNode(fold, QString::fromUtf8("маска"), &properties);
    properties.clear();
    properties["name"] = "mbit";
    properties["enum"] = QString("%1").arg(0);
    int i, j;
    for(i = 0; i<32; i++){
        QMap<QString, QString> prop;
        prop["submaska"] = QString("%1").arg(i);
        QDomElement submask = AddFolderNode(&mask, QString::fromUtf8("с %1 по %2").arg(i*8).arg((i+1)*8-1), &prop);
        for(j=0; j<8; j++){
            properties["enum"] = QString("%1").arg(i*8+j);
            properties["property"] = "clicked";
            AddBookmarkNode(&submask, QString("%1").arg(i*8+j), "1", &properties);
        }
    }
    Reset();
}

void XbelTree::AddTU()
{
    QDomElement *fold = new QDomElement();
    CreateHeaderFolder(fold, QString::fromUtf8("Устройство телеуправления"), TU);
    QMap<QString, QString> properties;
    properties["name"] = "address";
    AddBookmarkNode(fold, QString::fromUtf8("адрес"), "0x01", &properties);
    properties["name"] = "comport";
    AddBookmarkNode(fold, QString::fromUtf8("компорт"), "2", &properties);
    properties["name"] = "begin_TU";
    properties["ico"] = ":/images/code.png";
    AddBookmarkNode(fold, QString::fromUtf8("адресное пространство ТУ - начало"), "0x0000", &properties);
    properties["name"] = "end_TU";
    properties["ico"] = ":/images/code.png";
    AddBookmarkNode(fold, QString::fromUtf8("адресное пространство ТУ - окончание"), "0x0009", &properties);
    properties.clear();
    properties["name"] = "proto";
    properties["variants"] = "MODBUS|DCON";
    AddBookmarkNode(fold, QString::fromUtf8("протокол"), "MODBUS", &properties);
    properties.clear();
    properties["name"] = "tic";
    properties["ico"] = ":/images/cycle.png";
    AddBookmarkNode(fold, QString::fromUtf8("Время удержания реле(DCON)"), "10", &properties);
    Reset();
    emit(MessageToStatusBar(QString::fromUtf8("Добавлено устройство ТУ")));
}

bool XbelTree::AddBookmarkNode(QDomElement *parent, QString name, QString value, QMap<QString, QString> *properties)
{
    if( parent->isNull() ) {
        Q_ASSERT(parent);
        return false;
    }
    QDomElement elem = domDocument.createElement("bookmark");
    QDomElement title = domDocument.createElement("title");
    QDomText text = domDocument.createTextNode(name);
    elem.setAttribute("href", value);
    if( properties ){
        foreach(const QString &property, properties->keys()){
            elem.setAttribute(property, properties->value(property));
        }
    }
    title.appendChild(text);
    elem.appendChild(title);
    parent->appendChild(elem);
    return true;
}

QDomElement XbelTree::AddFolderNode(QDomElement *parent, QString value, QMap<QString, QString> *properties)
{
    Q_ASSERT(parent);
    QDomElement elem = domDocument.createElement("folder");
    QDomElement title = domDocument.createElement("title");
    QDomText text = domDocument.createTextNode(value);
    if( properties ){
        foreach(const QString &property, properties->keys()){
            elem.setAttribute(property, properties->value(property));
        }
    }
    title.appendChild(text);
    elem.appendChild(title);
    parent->appendChild(elem);
    return parent->lastChildElement("folder");
}

bool XbelTree::SetNodeTitle(QDomElement *fold, QString value)
{
    if( !fold )
        return false;
    fold->setAttribute("folded", "yes");
    QDomElement title = domDocument.createElement("title");
    QDomText text = domDocument.createTextNode(value);
    title.appendChild(text);
    fold->appendChild(title);
    return true;
}

void XbelTree::Reset()
{
    clear();
    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
               this, SLOT(updateDomElement(QTreeWidgetItem*,int)));
    disconnect(this, SIGNAL(itemClicked ( QTreeWidgetItem*,int)),
               this, SLOT(ItemClicked(QTreeWidgetItem*,int)));
    QDomElement root = domDocument.documentElement();
    QDomElement child = root.firstChildElement("folder");
    while (!child.isNull()) {
        parseFolderElement(child);
        child = child.nextSiblingElement("folder");
    }
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this, SLOT(updateDomElement(QTreeWidgetItem*,int)));
    connect(this, SIGNAL(itemClicked ( QTreeWidgetItem*,int)),
            this, SLOT(ItemClicked(QTreeWidgetItem*,int)));
}

void XbelTree::setRootNode()
{
    QDomElement xbel = domDocument.createElement("xbel");
    xbel.setAttribute("version", "1.0");
    domDocument.appendChild(xbel);
}

void XbelTree::keyPressEvent ( QKeyEvent * event )
{
    QTreeWidget::keyPressEvent(event);
    if( !this->selectedItems().isEmpty() ) {
        if(event->key() == Qt::Key_Delete) {
            QList<QTreeWidgetItem*> list = this->selectedItems();
            for (int i = 0; i < list.size(); ++i) {
                if( !domElementForItem[list.at(i)].attribute("device").isNull() ) {
                    QDomNode parent = domElementForItem[list.at(i)].parentNode();
                    parent.removeChild(domElementForItem[list.at(i)]);
                    RenumDevices();
                    Reset();
                    return;
                }
            }
        }
    }
}

void XbelTree::RenumDevices()
{
    devices = 0;
    QDomElement docElem = domDocument.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if( !e.attribute("device").isNull() ){
                e.setAttribute("device", devices++);
                qDebug() << e.attribute("device");
            }
        }
        n = n.nextSibling();
    }
}

int XbelTree::CreateConfig()
{
    QDomElement docElem = domDocument.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            if( !e.attribute("device").isNull() ){
                int type = e.attribute("type").toInt();
                switch(type){
                case TS:
                    parseTSTITTU(&e, TS);
                    break;
                case TIT:
                    parseTSTITTU(&e, TIT);
                    break;
                case TU:
                    parseTSTITTU(&e, TU);
                    break;
                case MERQURY:
                    MerquryCurrentBit = 0;
                    parseTSTITTU(&e, MERQURY);
                    break;
                }
            }
        }
        n = n.nextSibling();
    }
    emit(MessageToStatusBar(QString::fromUtf8("конфигурация создана")));
    return 0;
}

int XbelTree::parseTSTITTU(QDomElement* root, unsigned char type)
{
    QDomNode n = root->firstChild();
    GLOBALSTRUCT *ts = new GLOBALSTRUCT;
    int result;
    for(int i=0; i<_mq_maskasize; i++)
        ts->maska[i] = 0;
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if( (!e.isNull())&&(!e.attribute("name").isNull()) ) {
            QString name = e.attribute("name");
            if( !keywords.contains(name) ) {
                emit(MessageToStatusBar(QString::fromUtf8("не найдено %1").arg(name)));
                Q_ASSERT(0);
            }
            switch(keywords[name]){
            case _address:
                if( (result = ParseItem(&e, "address")) == -1 )
                    Q_ASSERT(0);
                else
                    ts->mbus = result;
                break;
            case _comport:
                if( (result = ParseItem(&e, "comport")) == -1 )
                    Q_ASSERT(0);
                else
                    ts->com = result;
                break;
            case _registers:
                if( (result = ParseItem(&e, "registers")) == -1 )
                    Q_ASSERT(0);
                else
                    ts->regs = result;
                break;
            case _cycle:
                if( (result = ParseItem(&e, "cycle")) == -1 )
                    Q_ASSERT(0);
                else
                    ts->cycle = result;
                break;
            case _proto:
                result = ParseProto(e.attribute("href"));
                if( result == -1 )
                    Q_ASSERT(0);
                else
                    ts->proto = result;
                break;
            case _begin_TU:
                if( (result = ParseItem(&e, "begin_TU")) == -1 )
                    Q_ASSERT(0);
                else
                    ts->begin = result;
                break;
            case _end_TU:
                if( (result = ParseItem(&e, "end_TU")) == -1 )
                    Q_ASSERT(0);
                else
                    ts->end = result;
                break;
            case _maska:
                result = MaskaParse(ts, &e);
                break;
            case _mqpassword:
                result = MQPasswordParse(ts, &e);
                break;
            case _potential: case _current: case _powerA: case _powerQ: case _powerS:
                result = MQMaskaParse(ts, &e);
                break;
            }
        }
        n = n.nextSibling();
    }
    ts->type = type;
    stack.push(ts);
    qDebug() << "TS-TIT" << type;
    return 0;
}

int XbelTree::MQMaskaParse(GLOBALSTRUCT *ts, QDomElement *e) {
    QDomNode n = e->firstChild().nextSibling();//skip title
    while(!n.isNull()) {
        QDomElement el = n.toElement();
        qDebug() << el.tagName();
        unsigned char value = el.attribute("href").toInt();
        int nbit = MerquryCurrentBit++;
        int nbyte = nbit / 8;
        int bit = nbit % 8;
        unsigned char byte = value << bit;
        ts->maska[nbyte] |= byte;
        n = n.nextSibling();
    }
}

int XbelTree::MQPasswordParse(GLOBALSTRUCT *ts, QDomElement *e) {
    QString password = e->attribute("href");
    char pass[_mqpassword_length];
    QString s;
    int d;
    if( password.length() != _mqpassword_length ) {
        qDebug() << "Password length must be _mqpassword_length!!!";
        Q_ASSERT(0);
        return 0;
    } else {
        for(int i=0; i<_mqpassword_length; i++) {
            switch(password[i].toAscii()){
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
            case '8': case '9': case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                s[0] = password[i];
                d = s.toInt(0, 10);
                if( !d )
                    d = s.toInt(0, 16);
                if( !d ) {
                    Q_ASSERT(0);
                }
                pass[i] = d;
                break;
            default:
                qDebug() << "Non compatible symbol!";
                Q_ASSERT(0);
            }
        }
    }
    memcpy(ts->password, pass, _mqpassword_length);
    return 1;
}

int XbelTree::MaskaParse(GLOBALSTRUCT *ts, QDomElement *root)
{
    QDomNode n = root->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if( (!e.isNull())&&(!e.attribute("submaska").isNull()) ){
            SubMaskaParse(ts, &e);
        }
        n = n.nextSibling();
    }
}

int XbelTree::SubMaskaParse(GLOBALSTRUCT *ts, QDomElement *root)
{
    QDomNode n = root->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if( (!e.isNull())&&(!e.attribute("enum").isNull()) ){
            EnumBitsParse(ts, &e);
        }
        n = n.nextSibling();
    }
}

int XbelTree::EnumBitsParse(GLOBALSTRUCT *ts, QDomElement *root)
{
    QDomNode n = root->firstChild();
    unsigned char value = root->attribute("href").toInt();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if( (!e.isNull())&&(e.tagName() == "title") ){
            int nbit = e.text().toInt();
            int nbyte = nbit / 8;
            int bit = nbit % 8;
            unsigned char byte = value << bit;
            ts->maska[nbyte] |= byte;
        }
        n = n.nextSibling();
    }
}

int XbelTree::ParseItem( QDomElement *e, QString name )
{
    bool ok;
    int result = e->attribute("href").toInt(&ok);
    if( !ok ) {
        result = e->attribute("href").toInt(&ok, 16);
    }
    if( !ok ) {
        emit(MessageToStatusBar(QString("%1 invalid %2").arg(name).arg(e->attribute("href"))));
        return -1;
    }
    return result;
}

int XbelTree::ParseProto(QString proto)
{
    if( proto.toInt() == _mbus_listorder ){
        return PROTO_MBUS;
    } else if( proto.toInt() == _dcon_listorder ){
        return PROTO_DCON;
    }
    emit(MessageToStatusBar(QString("proto invalid %1").arg(proto)));
    return -1;
}

int XbelTree::parseTIT(QDomElement* root)
{
    //qDebug() << "parse TIT";
    return 0;
}

int XbelTree::parseTU(QDomElement* root)
{
    //qDebug() << "parse TU";
    return 0;
}

void XbelTree::SaveConfig()
{
    qDebug() << "save config";
}

void XbelTree::ItemClicked(QTreeWidgetItem* item, int column)
{
    QString property = domElementForItem[item].attribute("property");
    if((property == "clicked")&&(column == 1)) {
        if(item->checkState(column) == Qt::Checked ) {
            domElementForItem[item].setAttribute("href", "1");
        }
        else {
            domElementForItem[item].setAttribute("href", "0");
        }
    }
    qDebug() << item << "clicked column: " << column << " item: " << item->text(column);
}
