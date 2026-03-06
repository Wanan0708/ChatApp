QT += quick virtualkeyboard qml websockets network sql

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/core/singletonmanager.cpp \
    src/core/appsettings.cpp \
    src/models/messagemodel.cpp \
    src/models/conversationmodel.cpp \
    src/network/websocketclient.cpp \
    src/network/networkclient.cpp \
    src/services/chatservice.cpp \
    src/database/databaseconfig.cpp \
    src/database/databasemanager.cpp \
    src/dao/userdao.cpp \
    src/dao/conversationdao.cpp \
    src/dao/messagedao.cpp \
    src/dao/friendrequestdao.cpp \
    src/utils/timeformatter.cpp \

HEADERS += \
    src/models/conversation.h \
    src/models/message.h \
    src/models/user.h \
    src/network/websocketclient.h \
    src/network/networkclient.h \
    src/singleton.h \
    src/core/singletonmanager.h \
    src/core/appsettings.h \
    src/models/basemodel.h \
    src/models/messagemodel.h \
    src/models/conversationmodel.h \
    src/services/chatservice.h \
    src/database/databaseconfig.h \
    src/database/databasemanager.h \
    src/dao/userdao.h \
    src/dao/conversationdao.h \
    src/dao/messagedao.h \
    src/dao/friendrequestdao.h \
    src/utils/timeformatter.h \

RESOURCES += qml.qrc \
    image.qrc

# 高DPI支持
QMAKE_LFLAGS += $$QMAKE_LFLAGS_HIGH_DPI_AWARE

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# place build outputs in project-root/bin instead of default build directory
DESTDIR = $$PWD/bin
# ensure object and moc files also go into bin to keep root clean
OBJECTS_DIR = $$PWD/bin/obj
MOC_DIR = $$PWD/bin/moc
RCC_DIR = $$PWD/bin/rcc
