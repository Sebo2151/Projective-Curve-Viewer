/*
    Projective Curve Viewer
    Copyright (C) 2016  Sebastian Bozlee

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "term.h"
#include "renderarea.h"
#include "functionedit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void handleAddCurveButton();
    void handleFunctionUpdate(int index);
    void handleFunctionColorUpdate(int index);
    void handleFunctionDeletePressed(int index);
    void handleAbout(bool t);
    void handleAnimationSpeedSlider(int value);
    void handleQuickStartMessage(bool t);

private:
    Ui::MainWindow *ui;

    std::vector<FunctionEdit*> functionEdits;
    RenderArea* render_area;
};

#endif // MAINWINDOW_H
