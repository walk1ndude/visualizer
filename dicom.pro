#-------------------------------------------------
#
# Project created by QtCreator 2014-01-24T14:23:02
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += quick gui

TARGET = dicom
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += include

unix:macx {

    INCLUDEPATH += /usr/local/include/gdcm-2.4 \
                   /usr/local/include

    LIBS += -L/usr/local/lib -lgdcmCommon \
                            -lgdcmDICT \
                            -lgdcmMSFF \
                            -lgdcmDSED \
                            -lgdcmIOD \
                            -lopencv_core \
                            -lopencv_imgproc \
                            -lopencv_highgui
}

unix:!macx {
    INCLUDEPATH += /usr/include/gdcm-2.4

    LIBS += -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui

    LIBS += -lgdcmCommon \
            -lgdcmDICT \
            -lgdcmMSFF \
            -lgdcmDSED \
            -lgdcmIOD
}

win32 {
    INCLUDEPATH += C:\opencv\build\include \
                   C:\GDCM\gdcm\include

    !contains(QMAKE_HOST.arch, x86_64) {
            QMAKE_LFLAGS *= /MACHINE:X86
    }
    else {
        contains(QMAKE_HOST.arch, x86_64):{
            QMAKE_LFLAGS *= /MACHINE:X64
        }
    }

    LIBS += -L"C:\GDCM\gdcm\bin\MinSizeRel" \
            -L"C:\opencv\build\lib\Release"

    LIBS += -lopencv_core249 \
            -lopencv_highgui249 \
            -lopencv_imgproc249

    LIBS += -lgdcmCommon \
            -lgdcmDICT \
            -lgdcmMSFF \
            -lgdcmDSED \
            -lgdcmIOD
}

SOURCES += src/main.cpp\
           src/dicomreader.cpp\
           src/matrixstack.cpp \
           src/openglitem.cpp \
           src/sliceviewer.cpp \
           src/appwindow.cpp \
           src/headmodel.cpp \
           src/viewport.cpp \
           src/viewports.cpp

HEADERS  += include/dicomreader.h \
            include/ctprocessing.hpp \
            include/matrixstack.h \
            include/openglitem.h \
            include/sliceviewer.h \
            include/appwindow.h \
            include/headmodel.h \
            include/viewport.h \
            include/viewports.h

RESOURCES += resources.qrc
