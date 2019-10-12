/*
    Projective Curve Viewer
    Copyright (C) 2016  Sebastian Bozlee

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>
#include <iostream>
#include <QColorDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    render_area = new RenderArea(this);
    ui->mainHorizontalLayout->addWidget(render_area, 1, Qt::Alignment());
    render_area->setMinimumSize(300,300);
    ui->sidebarVerticalLayout->setStretch(0, 1); // Stretch the curves part to fill sidebar.

    connect(ui->addCurveButton, SIGNAL(pressed()), this, SLOT(handleAddCurveButton()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(handleAbout(bool)));
    connect(ui->actionQuick_Start_Guide, SIGNAL(triggered(bool)), this, SLOT(handleQuickStartMessage(bool)));

    connect(ui->snapToXYButton, SIGNAL(pressed()), render_area, SLOT(snapToXYPlane()));
    connect(ui->snapToXZButton, SIGNAL(pressed()), render_area, SLOT(snapToXZPlane()));
    connect(ui->snapToYZButton, SIGNAL(pressed()), render_area, SLOT(snapToYZPlane()));

    connect(ui->animationSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(handleAnimationSpeedSlider(int)));

    // Start with one curve available.
    handleAddCurveButton();

    this->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete render_area;

    for (unsigned int i = 0; i < functionEdits.size(); i++)
        delete functionEdits[i];
}

void MainWindow::handleAddCurveButton()
{
    FunctionEdit* fe = new FunctionEdit(functionEdits.size());

    functionEdits.push_back(fe);

    connect(fe, SIGNAL(functionUpdated(int)), this, SLOT(handleFunctionUpdate(int)));
    connect(fe, SIGNAL(colorUpdated(int)), this, SLOT(handleFunctionColorUpdate(int)));
    connect(fe, SIGNAL(deleted(int)), this, SLOT(handleFunctionDeletePressed(int)));

    ui->curvesVerticalLayout->addLayout(fe);

    render_area->addFunction(fe->getColor());
}

void MainWindow::handleFunctionUpdate(int index)
{
    render_area->setFunction(index, functionEdits[index]->getFunctionClone());
    render_area->update();
}

void MainWindow::handleFunctionColorUpdate(int index)
{
    render_area->setFunctionColor(index, functionEdits[index]->getColor());
    render_area->update();
}

void MainWindow::handleFunctionDeletePressed(int index)
{
    render_area->deleteFunction(index);

    // Free associated memory.
    FunctionEdit* fe = functionEdits[index];
    delete fe;

    // Delete pointer from array.
    functionEdits.erase(functionEdits.begin() + index);

    // Update what they think their indices are.
    for (unsigned int i = index; i < functionEdits.size(); i++)
    {
        functionEdits[i]->setIndex(i);
    }

    render_area->update();
}

void MainWindow::handleAbout(bool t)
{
    QMessageBox::about(this, "About Projective Curve Viewer",
                       "Projective Curve Viewer is distributed in the hope that it will be "
                       "useful, but WITHOUT ANY WARRANTY; without even the implied warranty of "
                       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General "
                       "Public License for more details. \n\n"

                       "You should have received a copy of the GNU General Public License "
                       "along with this program.  If not, see <http://www.gnu.org/licenses/>.");
}

void MainWindow::handleQuickStartMessage(bool t)
{
    QMessageBox::about(this, "Quick Start Guide",
                       "Enter a polynomial in x, y, and z to display its zero locus. "
                       "Parentheses, integer expressions, and the symbols +, -, ^, * are supported. "
                       "Polynomials are automatically homogenized by adding multiples of z. \n\n"
                       "Click and drag to view different parts of the curve. "
                       "Zoom in and out with the mouse wheel. \n\n"
                       "In addition, there is a projective parameter [s : t] for animating "
                       "one-parameter families of curves. "
                       "To use this, enter a polynomial homogeneous in the variables s and t.");
}

void MainWindow::handleAnimationSpeedSlider(int value)
{
    const double c = exp(-20 / 10.0);

    if (value < 55 && value > 45)
        render_area->setVirtualTimeFactor(0);
    else if (value >= 55)
        render_area->setVirtualTimeFactor((exp((value - 75) / 10.0) - c)/(1 - c));
    else
        render_area->setVirtualTimeFactor((-exp(-(value - 25) / 10.0) + c)/(1 - c));
}
