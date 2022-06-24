#made by ms

QT       += core gui network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


LIBS += \
    -lpthread \
    -lboost_chrono \
    -lboost_date_time \
    -lboost_regex \
    -lboost_system \
    -lboost_thread

SOURCES += \
    GripperPage.cpp \
    OrderPickingPage.cpp \
    StatusDialog.cpp \
    VisionPage.cpp \
    cobotpage.cpp \
    cv_to_qt.cpp \
    impl/AsioTelnetClient.cpp \
    lift/RBCAN.cpp \
    lift/RBDaemon.cpp \
    lift/RBLANComm.cpp \
    lift/RBMotorController.cpp \
    liftpage.cpp \
    main.cpp \
    mainwindow.cpp \
    mobilepage.cpp \
    networkthread.cpp \
    qmylabel.cpp \
    telnetclient.cpp \
    websocketpage.cpp

HEADERS += \
    CommonHeader.h \
    GripperPage.h \
    OrderPickingPage.h \
    StatusDialog.h \
    VisionPage.h \
    cobotpage.h \
    cv_to_qt.h \
    impl/AsioTelnetClient.h \
    impl/TelnetProtocol.h \
    lift/RBCAN.h \
    lift/RBDaemon.h \
    lift/RBDataType.h \
    lift/RBLANComm.h \
    lift/RBMotorController.h \
    lift/RBSharedMemory.h \
    liftpage.h \
    mainwindow.h \
    mobilepage.h \
    networkthread.h \
    qmylabel.h \
    telnetclient.h \
    websocketpage.h

FORMS += \
    GripperPage.ui \
    OrderPickingPage.ui \
    StatusDialog.ui \
    VisionPage.ui \
    cobotpage.ui \
    liftpage.ui \
    mainwindow.ui \
    mobilepage.ui \
    websocketpage.ui




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resource.qrc

# OpenCV library all
INCLUDEPATH += /usr/include/opencv4/
LIBS += -L/usr/lib/x86_64-linux-gnu/
LIBS += -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgcodecs \
        -lopencv_imgproc \
        -lopencv_calib3d \
        -lopencv_features2d \
        -lopencv_flann \
        -lopencv_objdetect \
        -lopencv_photo \
        -lopencv_video \
        -lopencv_videoio \
        -lboost_system


