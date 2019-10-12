#-------------------------------------------------
#
# Project created by QtCreator 2016-06-06T13:32:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "Projective Curve Viewer"
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    binaryop.cpp \
    term.cpp \
    variable.cpp \
    numericalterm.cpp \
    renderarea.cpp \
    functionedit.cpp

HEADERS  += mainwindow.h \
    binaryop.h \
    term.h \
    variable.h \
    numericalterm.h \
    renderarea.h \
    functionedit.h

FORMS    += mainwindow.ui

DISTFILES += \
    vertexshader.vert \
    fragmentshader.frag

install_it.path = %{buildDir}
install_it.files += %{sourceDir}/vertexshader.vert %{sourceDir}/fragmentshader.frag

INSTALLS += install_it
