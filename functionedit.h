#ifndef FUNCTIONEDIT_H
#define FUNCTIONEDIT_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QColor>
#include <QVector3D>

#include "term.h"

class FunctionEdit : public QVBoxLayout
{
Q_OBJECT

public:
    FunctionEdit(int index);
    virtual ~FunctionEdit()
    {
        if (f)
            delete f;
        delete lineEdit;
        delete colorButton;
        delete deleteButton;
        delete hboxLayout;
    }

    void setIndex(int new_index) { index = new_index; }
    int getIndex() { return index; }
    Term* getFunctionClone() { return f->Clone(); }
    QVector3D getColor() { return QVector3D(color.redF(), color.greenF(), color.blueF()); }

signals:
    void functionUpdated(int index);
    void colorUpdated(int index);
    void deleted(int index);

private slots:
    void handleFunctionUpdate();
    void handleColorButton();
    void handleDeleteButton();

private:
    Term* f = 0;
    QColor color;
    int index;

    QLineEdit* lineEdit;
    QPushButton* colorButton;
    QPushButton* deleteButton;
    QHBoxLayout* hboxLayout;
};

#endif // FUNCTIONEDIT_H
