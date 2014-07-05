#include "jlineedit.h"

JLineEdit::JLineEdit(QTreeWidgetItem *parent_)
    : parent(parent_) {
    connect(this, SIGNAL(textChanged(QString)),
            this, SLOT(SetText(QString)));
}

void JLineEdit::SetText(QString value) {
    XbelTree *p = dynamic_cast<XbelTree*>(parent->treeWidget());
    QDomElement qde = p->domElementForItem[parent];
    qde.setAttribute("href", value);
    qDebug() << value;
}
