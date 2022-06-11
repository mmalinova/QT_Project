#-------------------------------------------------
#
# Project created by QtCreator 2022-05-15T18:24:10
#
#-------------------------------------------------

QT += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += E:\Programs\OpenCV\opencv\release\install\include

LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_core454.dll
LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_highgui454.dll
LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_imgcodecs454.dll
LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_imgproc454.dll
LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_features2d454.dll
LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_calib3d454.dll
LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_video454.dll
LIBS += E:\Programs\OpenCV\opencv\release\bin\libopencv_videoio454.dll


DISTFILES +=


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
