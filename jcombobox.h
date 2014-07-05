#ifndef JCOMBOBOX_H
#define JCOMBOBOX_H

#include <QComboBox>
#include <QTreeWidget>
#include <QDebug>
#include "xbeltree.h"

class JComboBox : public QComboBox {
    Q_OBJECT

public:
    JComboBox(QTreeWidgetItem *parent_);

private slots:
    void SetCurrentIndex(int);

private:
    QTreeWidgetItem *parent;
};

#endif // JCOMBOBOX_H
