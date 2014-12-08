#-------------------------------------------------
#
# Project created by QtCreator 2014-01-24T14:23:02
#
#-------------------------------------------------

greaterThan(QT_VERSION, 5.2): QT += quick

TARGET = visualizer
TEMPLATE = app

CONFIG += c++11 qtquickcompiler

INCLUDEPATH += include

unix:macx {

    QMAKE_MAC_SDK = macosx10.10

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

    LIBS += -framework OpenCL
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

    LIBS += -lOpenCL
}

win32 {
    INCLUDEPATH += "C:\opencv\build\include" \
                   "C:\Program Files (x86)\GDCM 2.4\include\gdcm-2.4" \
                   "C:\Program Files (x86)\AMD APP SDK\2.9\include"

    !contains(QMAKE_HOST.arch, x86_64) {
            QMAKE_LFLAGS *= /MACHINE:X86
            LIBS += -L"C:\GDCM\build86\bin\Debug" \
                    -L"C:\opencv\build\x86\vc12\lib" \
                    -L"C:\Program Files (x86)\AMD APP SDK\2.9\lib\x86"
    }
    else {
        contains(QMAKE_HOST.arch, x86_64):{
            QMAKE_LFLAGS *= /MACHINE:X64
            LIBS += -L"C:\GDCM\build86_64\bin\Debug" \
                    -L"C:\opencv\build\x64\vc12\lib" \
                    -L"C:\Program Files (x86)\AMD APP SDK\2.9-1\lib\x86_64"
        }
    }



    LIBS += -lopencv_core249 \
            -lopencv_highgui249 \
            -lopencv_imgproc249

    LIBS += -lgdcmCommon \
            -lgdcmDICT \
            -lgdcmMSFF \
            -lgdcmDSED \
            -lgdcmIOD

    LIBS += -lOpenCL
}

SOURCES +=  src/main.cpp \
            src/UserUI/AppWindow.cpp \
            src/UserUI/ModelViewer.cpp \
            src/Parser/DicomReader.cpp \
            src/Parser/Reconstructor.cpp \
            src/Parser/StlReader.cpp \
            src/Render/AbstractRenderer.cpp \
            src/Render/ModelRenderer.cpp \
            src/Model/AbstractModel.cpp \
            src/Model/StlModel.cpp \
            src/Scene/ModelScene.cpp \
            src/Model/PointsModel.cpp \
            src/Model/AbstractModelWithPoints.cpp \
            src/Scene/AbstractScene.cpp \
            src/Viewport/Viewport.cpp \
            src/Viewport/ViewportArray.cpp \
            src/UserUI/ConsoleLogger.cpp \
            src/Model/EvaluatorModel.cpp \
            src/Info/PointsInfo.cpp \
            src/Info/ViewRangeInfo.cpp \
            src/Info/ShaderInfo.cpp \
            src/UserUI/TextureNode.cpp \
            src/Camera/Camera.cpp \
            src/Model/VolumeModel.cpp \
            src/Parser/AbstractParser.cpp \
            src/Model/AxesModel.cpp \
    src/Message/AbstractMessage.cpp \
    src/Message/SettingsMessage.cpp \
    src/Scene/BlueprintQueue.cpp \
    src/Scene/SceneObject.cpp \
    src/Scene/LightSource.cpp \
    src/Scene/Material.cpp \
    src/Scene/Texture.cpp \
    src/UserUI/NetUI.cpp

HEADERS  += include/UserUI/AppWindow.h \
            include/UserUI/ModelViewer.h \
            include/Parser/ctprocessing.hpp \
            include/Parser/parallelprocessing.hpp \
            include/Parser/DicomReader.h \
            include/Parser/Reconstructor.h \
            include/Parser/StlReader.h \
            include/Render/AbstractRenderer.h \
            include/Render/ModelRenderer.h \
            include/Model/AbstractModel.h \
            include/Model/StlModel.h \
            include/Info/ModelInfo.h \
            include/Info/MaterialInfo.h \
            include/Info/LightInfo.h \
            include/Info/Info.h \
            include/Info/TextureInfo.h \
            include/Info/ShaderInfo.h \
            include/Scene/AbstractScene.h \
            include/Scene/ModelScene.h \
            include/Parser/Helpers.hpp \
            include/Info/PointsInfo.h \
            include/Model/PointsModel.h \
            include/Model/AbstractModelWithPoints.h \
            include/Viewport/Viewport.h \
            include/Viewport/ViewportArray.h \
            include/UserUI/TextureNode.h \
            include/UserUI/ConsoleLogger.h \
            include/Model/VertexVC.h \
            include/Model/VertexVN.h \
            include/Model/VertexVT.h \
            include/Info/ViewRangeInfo.h \
            include/Model/EvaluatorModel.h \
            include/Camera/Camera.h \
            include/Model/VolumeModel.h \
            include/Info/VolumeInfo.h \
            include/Parser/AbstractParser.h \
            include/Model/AxesModel.h \
    include/Message/AbstractMessage.h \
    include/Message/SettingsMessage.h \
    include/Model/ModelFactory.h \
    include/Scene/BlueprintQueue.h \
    include/Scene/SceneObjectList.h \
    include/Scene/SceneObject.h \
    include/Scene/LightSource.h \
    include/Scene/Material.h \
    include/Scene/Texture.h \
    include/UserUI/NetUI.h

RESOURCES += resources.qrc
