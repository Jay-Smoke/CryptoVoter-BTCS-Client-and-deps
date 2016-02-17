#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T09:24:10
#
#-------------------------------------------------


QT += core gui
QT += network
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cryptoVoter-btcs
TEMPLATE = app

win32 {
    RC_FILE = cryptovoter.rc
}

SOURCES += main.cpp\
        mainwindow.cpp \
    cryptovoter.cpp \
    answer.cpp \
    question.cpp \
    questionsdaohard.cpp \
    questionapidao.cpp \
    chartform.cpp \
    pieview.cpp\
    processform.cpp \
    version.cpp \
    questionapidaorequestmanager.cpp \
    aboutform.cpp \
    configuration.cpp \
    configform.cpp \
    instructionsform.cpp \
    eulaform.cpp \
    voteregister.cpp \
    votelogregister.cpp \
    votetransactionsform.cpp

HEADERS  += mainwindow.h \
    cryptovoter.h \
    answer.h \
    chartform.h \
    question.h \
    questionsdaohard.h \
    iquestionsdao.h \
    questionapidao.h \
    pieview.h\
    processform.h \
    version.h \
    questionapidaorequestmanager.h \
    aboutform.h \
    configuration.h \
    configform.h \
    instructionsform.h \
    eulaform.h \
    voteregister.h \
    votelogregister.h \
    votetransactionsform.h

FORMS    += mainwindow.ui \
    chartform.ui \
    processform.ui \
    aboutform.ui \
    configform.ui \
    instructionsform.ui \
    eulaform.ui \
    votetransactionsform.ui

RESOURCES += \
    res.qrc

DEFINES +=  \'SVN_REV=\"$(SVN_REVISION)\"\' \
            \'SVN_URL=\"$(SVN_URL)\"\' \
            \'JNK_BLD=\"$(BUILD_DISPLAY_NAME)\"\'
