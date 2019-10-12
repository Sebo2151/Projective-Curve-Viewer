/*
    Projective Curve Viewer
    Copyright (C) 2016  Sebastian Bozlee

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QTime>
#include "term.h"

class RenderArea : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = 0);
    ~RenderArea();

    // Warning: takes control of the pointer f, so generally send in a clone.
    void setFunction(int index, Term* f);
    void setFunctionColor(int index, QVector3D color);
    void addFunction(QVector3D color);
    void deleteFunction(int index);
    void setVirtualTimeFactor(double new_virtual_time_factor) { this->virtual_time_factor = new_virtual_time_factor; }


    void setYScale(float newScale);

signals:

public slots:
    void snapToXYPlane();
    void snapToXZPlane();
    void snapToYZPlane();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    QSize sizeHint() const {
        return QSize(1000,1000);
    }

    void addVerticesPatch(int index, int res, double x_min, double x_max, double y_min, double y_max, std::vector<QVector3D>* active_vertices,
                                      int recursion_depth = 0);

    void draw_functions(QOpenGLFunctions* f);
    void draw_axes(QOpenGLFunctions* f);
    void add_line_vertices(float a, float b, float c, std::vector<QVector3D>* vertex_vector);
    void free_function_data();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);

    QMatrix4x4 projection;
    GLuint vbuffer_handle;
    GLint vertexColor_handle;
    std::vector<Term*> functions;
    std::vector<QVector3D> function_colors;

    const GLuint MAX_NUM_VERTICES = 100000;
    const GLuint HORIZONTAL_RESOLUTION = 100;
    const GLuint VERTICAL_RESOLUTION = 100;

    float vertical_scale = 2.001f;
    float horizontal_scale = 2.001f;
    QMatrix4x4 view_rotation;

    QMatrix4x4 view_rotation_clicked;

    float mouse_clicked_x = 0;
    float mouse_clicked_y = 0;

    QTime start;
    QTime last_frame_time;
    double virtual_time_factor = 1.0;
    double virtual_time_elapsed;
    double s = 1;
    double t = 0;

    QTime startOfSecond;
    int frames_this_second;
};

#endif // RENDERAREA_H
