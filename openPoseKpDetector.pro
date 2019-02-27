QT += core
QT -= gui

CONFIG += c++11

TARGET = openPoseKpDetector
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../../../ProjectIncludes/jsonlib.pri)


INCLUDEPATH += C:\dev\openpose\include\
            C:\dev\openpose\3rdparty\windows\opencv\include\
            C:\dev\openpose\3rdparty\windows\caffe3rdparty\include\
            C:\dev\openpose\3rdparty\windows\caffe\include2\
            C:\dev\openpose\3rdparty\windows\caffe\include \
            ../jsonTest/

HEADERS+=  ../jsonTest/keypointdetector.h \
"C:\dev\openpose\include\openpose\headers.hpp"\
            C:\dev\openpose\include\openpose\core\cvMatToOpInput.hpp\
             C:\dev\openpose\include\openpose\core\scaleAndSizeExtractor.hpp\
            C:\dev\openpose\include\openpose\core\array.hpp\
            C:\dev\openpose\include\openpose\utilities\keypoint.hpp



LIBS +=C:\dev\openpose\build\src\openpose\Release\openpose.lib\
          C:\dev\cuda\lib\x64\cudnn.lib\
        "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64\cudart.lib"\
        "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64\cublas.lib"\
        "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64\cusolver.lib"\
        "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64\curand.lib"\
        C:\dev\openpose\3rdparty\windows\opencv\x64\vc14\lib\opencv_world310.lib\
        C:\dev\openpose\3rdparty\windows\opencv\x64\vc14\lib\opencv_world310d.lib\
        C:\dev\openpose\3rdparty\windows\caffe3rdparty\lib\gflags.lib\
        C:\dev\openpose\3rdparty\windows\caffe3rdparty\lib\glog.lib\
        C:\dev\openpose\3rdparty\windows\caffe\lib\caffe.lib




