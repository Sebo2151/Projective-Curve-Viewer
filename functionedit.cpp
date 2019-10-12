#include "functionedit.h"

#include <QColorDialog>
#include <QTimer>
#include <QPalette>

#include <iostream>

FunctionEdit::FunctionEdit(int index)
{
    // I'm not content with these colors.
    const QColor defaultColors[6] =
    {
        QColor::fromRgb(0,0,0),
        QColor::fromRgb(0x07,0x74,0xa9),
        QColor::fromRgb(0xff,0x6c,0x00),
        QColor::fromRgb(0xad,0x00,0xad),
        QColor::fromRgb(0x75,0xad,0x00),
        QColor::fromRgb(0x5d,0xe8,0xe4)
    };

    // Initialize function-related info
    color = defaultColors[index % 6];
    f = 0;
    this->index = index;

    // Set up GUI aspects...

    lineEdit = new QLineEdit();
    colorButton = new QPushButton("Color");
    deleteButton = new QPushButton("Delete");

    // Add as layout...
    this->addWidget(lineEdit, 1);
    hboxLayout = new QHBoxLayout();
    this->addLayout(hboxLayout,0);
    hboxLayout->addWidget(colorButton, 0);
    hboxLayout->addWidget(deleteButton, 0);

    connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(handleFunctionUpdate()));
    connect(colorButton, SIGNAL(pressed()), this, SLOT(handleColorButton()));
    connect(deleteButton, SIGNAL(pressed()), this, SLOT(handleDeleteButton()));

    // This incantation will set the focus to the newly constructed line edit.
    QTimer::singleShot(0,lineEdit,SLOT(setFocus()));
}

// Called when the formula has been updated.
// Parses it, simplifies it, homogenizes it, then passes it on to the Main Window.
void FunctionEdit::handleFunctionUpdate()
{
    try
    {
        if (f)
            delete f;

        Term* f_temp = Term::parseTerm(lineEdit->text().toStdString());
        Term* f_temp2 = f_temp->simplify();
        delete f_temp;

        int degree = 0;
        f = f_temp2->homogenize(&degree);
        delete f_temp2;

        f->print();
        std::cout << std::endl;

        QPalette palette = QPalette();
        palette.setColor(lineEdit->backgroundRole(), QColor::fromRgb(0xff, 0xff, 0xff));
        lineEdit->setPalette(palette);

        emit functionUpdated(index);
    }
    catch (BadTermException bte)
    {
        // To do: get this diplayed in a statusbar on the mainwindow
        // std::cout << bte.getErrorMessage() << std::endl;

        QPalette palette = QPalette();
        palette.setColor(lineEdit->backgroundRole(), QColor::fromRgb(0xff, 100, 100));
        lineEdit->setPalette(palette);

        f = 0;
    }
}

// Color button has been pressed.
// Run a color dialog, then pass on the selected color to the Main Window.
void FunctionEdit::handleColorButton()
{
    QColorDialog d;
    QColor temp_color = d.getColor(color, 0, "Graph Color");
    if (temp_color.isValid())
    {
        color = temp_color;
        emit colorUpdated(index);
    }
}

// Delete button has been pressed.
// Pass the word to the Main Window, which will handle deleting this.
void FunctionEdit::handleDeleteButton()
{
    emit deleted(index);
}
