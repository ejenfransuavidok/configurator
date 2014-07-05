#ifndef JLINEEDIT_H
#define JLINEEDIT_H

#include <QLineEdit>
#include <QTreeWidget>
#include <QDebug>
#include <QString>
#include "xbeltree.h"

class JLineEdit : public QLineEdit {
    Q_OBJECT

public:
    JLineEdit(QTreeWidgetItem *parent_);

private slots:
    void SetText(QString);

private:
    QTreeWidgetItem *parent;
};

#endif // JLINEEDIT_H
