/*
    Projective Curve Viewer
    Copyright (C) 2016  Sebastian Bozlee

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "renderarea.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QVector4D>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QDir>

#include <QTime>

RenderArea::RenderArea(QWidget* parent) : QOpenGLWidget(parent)
{
    view_rotation.setToIdentity();
    start = QTime::currentTime();
    start.start();

    last_frame_time = QTime::currentTime();

    startOfSecond = QTime::currentTime();
}

RenderArea::~RenderArea()
{
    while (functions.size() > 0)
    {
        delete functions[functions.size() - 1];
        functions.erase(functions.end() - 1);
        function_colors.erase(function_colors.end() - 1);
    }
}

void RenderArea::setFunction(int index, Term* f)
{
    if (functions[index])
        delete functions[index];

    functions[index] = f;
}

void RenderArea::setFunctionColor(int index, QVector3D color)
{
    function_colors[index] = color;
}

void RenderArea::addFunction(QVector3D color)
{
    functions.push_back(0);
    function_colors.push_back(color);
}

void RenderArea::deleteFunction(int index)
{
    delete functions[index];
    functions.erase(functions.begin() + index);
    function_colors.erase(function_colors.begin() + index);
}

void RenderArea::setYScale(float newScale)
{
    if (newScale > 40.0f)
        newScale = 40.0f;
    if (newScale < 0.0001f)
        newScale = 0.0001f;
    vertical_scale = newScale;
    horizontal_scale = vertical_scale*this->width() / float(this->height());
}

void RenderArea::snapToXYPlane()
{
    view_rotation.setToIdentity();
    update();
}

void RenderArea::snapToXZPlane()
{
    view_rotation = QMatrix4x4(1, 0, 0, 0,
                               0, 0, 1, 0,
                               0, -1, 0, 0,
                               0, 0, 0, 1);
    update();
}

void RenderArea::snapToYZPlane()
{
    view_rotation = QMatrix4x4(0, 0, -1, 0,
                               0, 1, 0, 0,
                               1, 0, 0, 0,
                               0, 0, 0, 1);
    update();
}

// copy-pasted from an online tutorial:
// https://github.com/OpenGLInsights/OpenGLInsightsCode/blob/master/Chapter%2026%20Indexing%20Multiple%20Vertex%20Arrays/common/shader.cpp
GLuint load_shaders(const char * vertex_file_path,const char * fragment_file_path, QOpenGLFunctions* f){

    // Create the shaders
    GLuint VertexShaderID = f->glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = f->glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ifstream::in);
    if(VertexShaderStream.is_open()){
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }else{
        std::cout << "Impossible to open " << vertex_file_path << "." << std::endl
                  << "Try copying the shaders from the source directory to the application directory." << std::endl;
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }else{
        std::cout << "Impossible to open " << fragment_file_path << "." << std::endl
                  << "Try copying the fragment shader from the source directory to the application directory." << std::endl;
        getchar();
        return 0;
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    std::cout << "Compiling shader : " << vertex_file_path << std::endl;
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    f->glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    f->glCompileShader(VertexShaderID);

    // Check Vertex Shader
    f->glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    f->glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        f->glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        std::cout << &VertexShaderErrorMessage[0] << std::endl;
    }

    // Compile Fragment Shader
    std::cout << "Compiling shader : " << fragment_file_path << std::endl;
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    f->glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    f->glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    f->glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    f->glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        f->glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        std::cout << &FragmentShaderErrorMessage[0] << std::endl;
    }



    // Link the program
    std::cout << "Linking program" << std::endl;
    GLuint ProgramID = f->glCreateProgram();
    f->glAttachShader(ProgramID, VertexShaderID);
    f->glAttachShader(ProgramID, FragmentShaderID);
    f->glLinkProgram(ProgramID);

    // Check the program
    f->glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    f->glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        f->glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cout << &ProgramErrorMessage[0] << std::endl;
    }


    f->glDetachShader(ProgramID, VertexShaderID);
    f->glDetachShader(ProgramID, FragmentShaderID);

    f->glDeleteShader(VertexShaderID);
    f->glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void RenderArea::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.:
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    QDir application_directory(QCoreApplication::applicationDirPath());
    QString frag_file = application_directory.filePath("fragmentshader.frag");
    QString vert_file = application_directory.filePath("vertexshader.vert");

    GLuint programid = load_shaders(vert_file.toStdString().c_str(), frag_file.toStdString().c_str(), f);
    f->glUseProgram(programid);

    f->glGenBuffers(1, &vbuffer_handle);

    vertexColor_handle = f->glGetUniformLocation(programid, "vertexColor");

    if (vertexColor_handle == -1)
    {
        std::cout << "vertexColor uniform handle not found." << std::endl;
        exit(1);
    }

    f->glLineWidth(2.0f);
}

void RenderArea::resizeGL(int w, int h)
{
    // Update projection matrix and other size related settings:
    projection.setToIdentity();
    projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);

    horizontal_scale = vertical_scale*w / float(h);
}

// Build up the curve using a recursive marching squares algorithm.
// That is, we sample values of the relevant function on a square grid.
// Where there are sign changes, we either recurse to use a higher resolution
// grid, or add vertices between the sample points where signs changed.
void RenderArea::addVerticesPatch(int index, int res, double x_min, double x_max, double y_min, double y_max, std::vector<QVector3D>* active_vertices,
                                  int recursion_depth)
{
    const int recursion_res = 2;
    const int MAX_RECURSION_DEPTH = 1;

    double xstep = (x_max - x_min)/res;
    double ystep = (y_max - y_min)/res;

    // Compute function values at grid points.
    double* vals = new double[(res+1)*(res+1)];

    for (int i = 0; i <= res; i++)
    {
        for (int j = 0; j <= res; j++)
        {
            double x = x_min + xstep*i;
            double y = y_min + ystep*j;

            // Testing.
            vals[(res + 1)*i + j] = functions[index]->eval(view_rotation*QVector4D(x,y,1,1), s, t);
        }
    }

    for (int i = 0; i < res; i++)
    {
        for (int j = 0; j < res; j++)
        {
            double x = x_min + xstep*i;
            double y = y_min + ystep*j;

            double val_ll = vals[(res + 1)*i + j];
            double val_lr = vals[(res + 1)*(i + 1) + j];
            double val_ul = vals[(res + 1)*i + j + 1];
            double val_ur = vals[(res + 1)*(i + 1) + j + 1];

            // If there are sign changes and we are below the maximum recursion depth,
            // recurse to get a higher quality approximation.
            if (recursion_depth < MAX_RECURSION_DEPTH)
            {
                if (val_ll*val_lr < 0 || val_ul*val_ur < 0 || val_ll*val_ul < 0 || val_lr*val_ur < 0)
                {
                    addVerticesPatch(index, recursion_res, x, x + xstep, y, y + ystep, active_vertices, recursion_depth + 1);
                }
            }
            else
            {
                int times_flopped = 0;

                if (val_ll*val_lr <= 0)
                {
                    active_vertices->push_back(QVector3D((-val_ll * xstep)/(val_lr - val_ll) + x, y, 0));
                    times_flopped++;
                }
                if (val_ul*val_ur <= 0)
                {
                    active_vertices->push_back(QVector3D((-val_ul * xstep)/(val_ur - val_ul) + x, y + ystep, 0));
                    times_flopped++;
                }
                if (val_ll*val_ul <= 0)
                {
                    active_vertices->push_back(QVector3D(x, (-val_ll * ystep)/(val_ul - val_ll) + y, 0));
                    times_flopped++;
                }
                if (val_lr*val_ur <= 0)
                {
                    active_vertices->push_back(QVector3D(x + xstep, (-val_lr * ystep)/(val_ur - val_lr) + y, 0));
                    times_flopped++;
                }

                // We want to leave an even number of vertices for this square;
                // adjacent vertices in the list are paired into lines.
                if (times_flopped == 1 || times_flopped == 3)
                    active_vertices->pop_back();
            }
        }
    }

    delete vals;
}

void RenderArea::draw_functions(QOpenGLFunctions* f)
{
    for (unsigned int index = 0; index < functions.size(); index++)
    {
        if (functions[index])
        {
            std::vector<QVector3D> active_vertices;

            const int res = 200;

            addVerticesPatch(index, res, -horizontal_scale, horizontal_scale, -vertical_scale, vertical_scale, &active_vertices, 0);

            int num_vertices = active_vertices.size() < MAX_NUM_VERTICES ? active_vertices.size() : MAX_NUM_VERTICES;

            for (int i = 0; i < num_vertices; i++)
            {
                active_vertices[i].setX(active_vertices[i].x() / horizontal_scale);
                active_vertices[i].setY(active_vertices[i].y() / vertical_scale);
            }

            f->glLineWidth(3.0f);
            f->glEnableVertexAttribArray(0);

            f->glBindBuffer(GL_ARRAY_BUFFER, vbuffer_handle);
            f->glVertexAttribPointer(
                        0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        (void*)0
            );

            f->glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices*3, (float*)active_vertices.data(), GL_DYNAMIC_DRAW);

            f->glUniform3fv(vertexColor_handle, 1, (float*)&(function_colors[index]));

            f->glDrawArrays(GL_LINES, 0, num_vertices);

            f->glDisableVertexAttribArray(0);
            f->glDisableVertexAttribArray(1);

            //delete vertex_array;
        }
    }
}

void RenderArea::add_line_vertices(float a, float b, float c, std::vector<QVector3D>* vertex_vector)
{
    // Check if line not visible. (Nearest point is farther from 0 than length of diagonal)
    if (c*c > (vertical_scale*vertical_scale + horizontal_scale*horizontal_scale)*(a*a + b*b))
        return;

    // Use better-behaved denominator to calculate intersections with screen edges...
    if (fabs(a) > fabs(b))
    {
        float x1 = (-b*vertical_scale - c)/a;
        float x2 = (b*vertical_scale - c)/a;
        vertex_vector->push_back(QVector3D(x1 / horizontal_scale, 1, 1));
        vertex_vector->push_back(QVector3D(x2 / horizontal_scale, -1, 1));
    }
    else
    {
        float y1 = (-a*horizontal_scale - c)/b;
        float y2 = (a*horizontal_scale - c)/b;
        vertex_vector->push_back(QVector3D(1, y1 / vertical_scale, 1));
        vertex_vector->push_back(QVector3D(-1, y2 / vertical_scale, 1));
    }
}

void RenderArea::draw_axes(QOpenGLFunctions* f)
{
    std::vector<QVector3D> vertex_vector;
    std::vector<QVector3D> color_vector;

    const float* m = view_rotation.constData();

    /*std::cout << m[0 + 0*4] << ", " << m[0 + 1*4] << ", " << m[0 + 2*4] << std::endl;
    std::cout << m[1 + 0*4] << ", " << m[1 + 1*4] << ", " << m[1 + 2*4] << std::endl;
    std::cout << m[2 + 0*4] << ", " << m[2 + 1*4] << ", " << m[2 + 2*4] << std::endl << std::endl;*/

    // Hacky.
    add_line_vertices(m[0 + 0*4],m[0 + 1*4],m[0 + 2*4], &vertex_vector);
    int red_verts = vertex_vector.size();
    add_line_vertices(m[1 + 0*4],m[1 + 1*4],m[1 + 2*4], &vertex_vector);
    int green_verts = vertex_vector.size() - red_verts;
    add_line_vertices(m[2 + 0*4],m[2 + 1*4],m[2 + 2*4], &vertex_vector);
    int blue_verts = vertex_vector.size() - green_verts - red_verts;

    int num_vertices = vertex_vector.size();

    float vertex_array[6*3];

    for (int i = 0; i < num_vertices; i++)
    {
        vertex_array[3*i] = vertex_vector[i].x();
        vertex_array[3*i + 1] = vertex_vector[i].y();
        vertex_array[3*i + 2] = vertex_vector[i].z();
    }

    f->glLineWidth(2.0f);
    f->glEnableVertexAttribArray(0);

    f->glBindBuffer(GL_ARRAY_BUFFER, vbuffer_handle);
    f->glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
    );

    f->glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices*3, vertex_array, GL_DYNAMIC_DRAW);

    f->glUniform3f(vertexColor_handle, 1.0, 0.5, 0.5);
    f->glDrawArrays(GL_LINES, 0, red_verts);
    f->glUniform3f(vertexColor_handle, 0.5, 1.0, 0.5);
    f->glDrawArrays(GL_LINES, red_verts, green_verts);
    f->glUniform3f(vertexColor_handle, 0.5, 0.5, 1.0);
    f->glDrawArrays(GL_LINES, red_verts + green_verts, blue_verts);

    f->glDisableVertexAttribArray(0);
}

void RenderArea::paintGL()
{   
    QTime beforeFrame = QTime::currentTime();

    virtual_time_elapsed += last_frame_time.elapsed()*0.001*virtual_time_factor;

    s = cos(virtual_time_elapsed);
    t = sin(virtual_time_elapsed);

    last_frame_time = QTime::currentTime();

    // Draw the scene:
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);

    draw_axes(f);
    draw_functions(f);

    frames_this_second++;
    if (startOfSecond.elapsed() >= 1000)
    {
        std::cout << "FPS: " << frames_this_second
                  << ", msec/frame: " << (double)startOfSecond.elapsed() / frames_this_second << std::endl;
        std::cout << "This frame: " << beforeFrame.elapsed() << " msecs." << std::endl;
        startOfSecond.start();
        frames_this_second = 0;
    }

    this->update();
}

QMatrix4x4 rotation_between(QVector3D v1, QVector3D v2)
{
    QMatrix4x4 rot_diff;

    v1.normalize();
    v2.normalize();

    QVector3D v = QVector3D::crossProduct(v1,
                                          v2);

    if (QVector3D::dotProduct(v1, v2) > 0)
        rot_diff.rotate(asin(v.length())*180/3.1415926535, v);
    else
        rot_diff.rotate(180 - asin(v.length())*180/3.1415926535, v);

    return rot_diff;
}

QMatrix4x4 translation_between(QVector3D v1, QVector3D v2)
{
    QVector3D diff = v2 - v1;

    return QMatrix4x4(1, 0, 0, diff.x(),
                      0, 1, 0, diff.y(),
                      0, 0, 1, diff.z(),
                      0, 0, 0, 1);
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        float mouse_now_x = (event->x()/(this->width()*0.5f) - 1.0f)*horizontal_scale;
        float mouse_now_y = -(event->y()/(this->height()*0.5f) - 1.0f)*vertical_scale;

        QVector3D v1 = QVector3D(mouse_now_x,mouse_now_y,1);
        QVector3D v2 = QVector3D(mouse_clicked_x, mouse_clicked_y, 1);

        Qt::KeyboardModifiers mods = QGuiApplication::keyboardModifiers();
        if (!(mods & Qt::ShiftModifier))
        {   // Rotate
            view_rotation = view_rotation_clicked*rotation_between(v1, v2);
        }
        else
        {   // Translate.
            // Warning: this currently translates the curves, but not the axes.
            view_rotation = view_rotation_clicked*translation_between(v1, v2);
        }
    }
    this->update();
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    view_rotation_clicked = view_rotation;

    mouse_clicked_x = (event->x()/(this->width()*0.5f) - 1.0f)*horizontal_scale;
    mouse_clicked_y = -(event->y()/(this->height()*0.5f) - 1.0f)*vertical_scale;

}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{

}

void RenderArea::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        // Qt::KeyboardModifiers mods = QGuiApplication::keyboardModifiers();

        // Zoom in/out
        float old_vertical_scale = vertical_scale;
        float old_horizontal_scale = horizontal_scale;
        this->setYScale(vertical_scale*pow(1.001,-event->delta()));

        // Adjust rotation so that cursor remains at same projective point.
        QPoint cursor_pos = this->mapFromGlobal(QCursor::pos());

        float mouse_x_orig = (cursor_pos.x()/(this->width()*0.5f) - 1.0f)*old_horizontal_scale;
        float mouse_y_orig = -(cursor_pos.y()/(this->height()*0.5f) - 1.0f)*old_vertical_scale;

        float mouse_x_now = (cursor_pos.x()/(this->width()*0.5f) - 1.0f)*horizontal_scale;
        float mouse_y_now = -(cursor_pos.y()/(this->height()*0.5f) - 1.0f)*vertical_scale;

        std::cout << "(" << mouse_x_orig << ", " << mouse_y_orig << ") -> ("
                         << mouse_x_now << ", " << mouse_y_now << ")" << std::endl;

        QVector3D v1 = QVector3D(mouse_x_orig, mouse_y_orig, 1);
        QVector3D v2 = QVector3D(mouse_x_now, mouse_y_now, 1);

        view_rotation = view_rotation*rotation_between(v2, v1);
    }
    std::cout << vertical_scale << std::endl;
    update();
}
