#-------------------------------------------------
#
# Project created by QtCreator 2015-03-26T15:24:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ClothingRecognition
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    displaywidget.cpp \
    clothingsearcher.cpp \
    facedetector.cpp

HEADERS  += mainwindow.h \
    displaywidget.h \
    clothingsearcher.h \
    facedetector.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv
LIBS += \
/usr/local/lib/libopencv_core.so \
/usr/local/lib/libopencv_highgui.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_imgproc.so \
/usr/local/lib/libopencv_features2d.so \
/usr/local/lib/libopencv_nonfree.so \
/usr/local/lib/libopencv_flann.so \
/usr/local/lib/libopencv_calib3d.so \
/usr/local/lib/libopencv_ml.so \
/usr/local/lib/libopencv_gpu.so \
/usr/local/lib/libopencv_objdetect.so \

