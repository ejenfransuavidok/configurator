#ifndef JSPINBOX_H
#define JSPINBOX_H

#include <QSpinBox>
#include <QTreeWidget>
#include <QDebug>
#include "xbeltree.h"

class JSpinBox : public QSpinBox {
    Q_OBJECT

public:
    JSpinBox(QTreeWidgetItem *parent_);

private slots:
    void ValueChanged(int);

private:
    QTreeWidgetItem *parent;
};

#endif // JSPINBOX_H
