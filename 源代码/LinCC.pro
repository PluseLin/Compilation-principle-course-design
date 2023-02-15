QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    grammarui.cpp \
    help.cpp \
    lexicalui.cpp \
    main.cpp \
    parserui.cpp \
    predtableui.cpp \
    semanticui.cpp \
    src/AsmGenerator.cpp \
    src/Attrbute.cpp \
    src/LL1Grammar.cpp \
    src/LexicalAnalyzer.cpp \
    src/Parser.cpp \
    src/SemanticAnalyzer.cpp \
    ui.cpp

HEADERS += \
    grammarui.h \
    help.h \
    include/Attrbute.h \
    include/LL1Grammar.h \
    include/LexicalAnalyzer.h \
    include/Parser.h \
    include/SemanticAnalyzer.h \
    lexicalui.h \
    parserui.h \
    predtableui.h \
    semanticui.h \
    src/AsmGenerator.h \
    src/Attrbute.h \
    src/LL1Grammar.h \
    src/LexicalAnalyzer.h \
    src/Parser.h \
    src/SemanticAnalyzer.h \
    ui.h

FORMS += \
    grammarui.ui \
    help.ui \
    lexicalui.ui \
    parserui.ui \
    predtableui.ui \
    semanticui.ui \
    ui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
