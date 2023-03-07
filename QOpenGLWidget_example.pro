QT       += core gui widgets opengl openglwidgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    $$PWD/include

SOURCES += \
    src/Cube.cpp \
    src/Material.cpp \
    src/Mesh.cpp \
    src/SceneManager.cpp \
    src/main.cpp \
    src/MainWindow.cpp \

HEADERS += \
    include/Cube.h \
    include/Material.h \
    include/Mesh.h \
    include/Shape.h \
    include/SceneManager.h \
    include/MainWindow.h

FORMS += \
    ui/SceneManager.ui \
    ui/MainWindow.ui

RESOURCES += \
    resources/shaders.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
