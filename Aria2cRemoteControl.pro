# -------------------------------------------------
# Project created by QtCreator 2010-02-14T09:57:44
# -------------------------------------------------
QT += network \
    xml
# The application version
VERSION = 0.1.0

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TARGET = Aria2cRemoteControl
TEMPLATE = app
SOURCES += main.cpp \
    aria2cremote.cpp \
    tabwidget.cpp \
    listview.cpp \
    mainlistview.cpp \
    adduri.cpp \
    add_http_ftp_magnetlink.cpp \
    selecttorrentfiles.cpp \
    bencodeparser.cpp \
    metainfo.cpp \
    util.cpp \
    xmlrpc.cpp \
    xmlrpc/variant.cpp \
    xmlrpc/response.cpp \
    xmlrpc/request.cpp \
    xmlrpc/client.cpp \
    download.cpp \
    workthread.cpp \
    reguestthread.cpp \
    globaloptions.cpp \
    localoptions.cpp \
    transfer.cpp \
    addserver.cpp \
    server.cpp \
    aria2options.cpp \
    qtreewidgetitemex.cpp
HEADERS += aria2cremote.h \
    tabwidget.h \
    listview.h \
    mainlistview.h \
    adduri.h \
    add_http_ftp_magnetlink.h \
    selecttorrentfiles.h \
    bencodeparser.h \
    metainfo.h \
    util.h \
    xmlrpc.h \
    xmlrpc/variant.h \
    xmlrpc/response.h \
    xmlrpc/request.h \
    xmlrpc/client.h \
    download.h \
    workthread.h \
    reguestthread.h \
    globaloptions.h \
    localoptions.h \
    transfer.h \
    addserver.h \
    server.h \
    aria2options.h \
    bargraph.h \
    qtreewidgetitemex.h
FORMS += aria2cremote.ui \
    tabwidget.ui \
    listview.ui \
    mainlistview.ui \
    adduri.ui \
    add_http_ftp_magnetlink.ui \
    selecttorrentfiles.ui \
    globaloptions.ui \
    localoptions.ui \
    transfer.ui \
    addserver.ui \
    aria2options.ui
RESOURCES += Aria2cRemote.qrc

win32 {
    RC_FILE = Aria2cRemoteControl.rc
    DEFINES += BUILDTIME=\\\"$$system('echo %time%')\\\"
    DEFINES += BUILDDATE=\\\"$$system('echo %date%')\\\"
} else {
    DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M.%s')\\\"
    DEFINES += BUILDDATE=\\\"$$system(date '+%d/%m/%y')\\\"
}
