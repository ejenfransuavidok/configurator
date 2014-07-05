#include "jspinbox.h"

JSpinBox::JSpinBox(QTreeWidgetItem *parent_)
    : parent(parent_) {
    connect(this, SIGNAL(valueChanged(int)),
            this, SLOT(ValueChanged(int)));
}

void JSpinBox::ValueChanged(int value) {
    XbelTree *p = dynamic_cast<XbelTree*>(parent->treeWidget());
    QDomElement qde = p->domElementForItem[parent];
    qde.setAttribute("href", this->value());
    qDebug() << p << qde.attribute("href");
}
