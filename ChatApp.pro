QT += quick virtualkeyboard qml websockets network concurrent widgets

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
    src/core/securestorage.cpp \
    src/core/errorhandler.cpp \
    src/core/logger.cpp \
    src/core/servicelocator.cpp \
    src/core/thememanager.cpp \
    src/core/translationmanager.cpp \
    src/core/notificationmanager.cpp \
    src/models/messagemodel.cpp \
    src/models/conversationmodel.cpp \
    src/models/messagelistproxy.cpp \
    src/network/websocketclient.cpp \
    src/network/networkclient.cpp \
    src/network/requestbatcher.cpp \
    src/services/chatservice.cpp \
    src/services/fileupload_methods.cpp \
    src/database/databaseconfig.cpp \
    src/dao/userdao.cpp \
    src/dao/conversationdao.cpp \
    src/dao/messagedao.cpp \
    src/dao/friendrequestdao.cpp \
    src/utils/timeformatter.cpp \
    src/utils/imagecache.cpp \
    src/utils/messagecache.cpp \
    src/utils/inputvalidator.cpp \
    src/utils/messagesearch.cpp \
    src/utils/emojiprovider.cpp \

HEADERS += \
    src/models/conversation.h \
    src/models/message.h \
    src/models/user.h \
    src/network/websocketclient.h \
    src/network/networkclient.h \
    src/network/requestbatcher.h \
    src/singleton.h \
    src/core/singletonmanager.h \
    src/core/appsettings.h \
    src/core/securestorage.h \
    src/core/errorhandler.h \
    src/core/logger.h \
    src/core/servicelocator.h \
    src/core/thememanager.h \
    src/core/translationmanager.h \
    src/core/notificationmanager.h \
    src/models/basemodel.h \
    src/models/messagemodel.h \
    src/models/conversationmodel.h \
    src/models/messagelistproxy.h \
    src/services/chatservice.h \
    src/database/databaseconfig.h \
    src/dao/userdao.h \
    src/dao/conversationdao.h \
    src/dao/messagedao.h \
    src/dao/friendrequestdao.h \
    src/utils/timeformatter.h \
    src/utils/imagecache.h \
    src/utils/messagecache.h \
    src/utils/inputvalidator.h \
    src/utils/messagesearch.h \
    src/utils/emojiprovider.h \

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
