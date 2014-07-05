#include "jcombobox.h"

JComboBox::JComboBox(QTreeWidgetItem *parent_)
    : parent(parent_) {
    connect(this, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SetCurrentIndex(int)));
}

void JComboBox::SetCurrentIndex(int value) {
    XbelTree *p = dynamic_cast<XbelTree*>(parent->treeWidget());
    QDomElement qde = p->domElementForItem[parent];
    qde.setAttribute("href", value);
    qDebug() << "SetCurrentIndex" << p << qde.attribute("href");
}

