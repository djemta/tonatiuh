######################################################################
# Automatically generated by qmake (2.01a) jue 8. feb 13:55:05 2007
######################################################################
 
TEMPLATE = app
CONFIG       += qt warn_on thread debug_and_release 

include( ../config.pri )

TARGET = Tonatiuh   

DEPENDPATH += . \
              src 
QT += xml opengl svg


 
win32: {
	RC_FILE = Tonatiuh.rc
}

mac:{
	ICON = icons/Tonatiuh.icns
}

# Input
HEADERS += *.h
FORMS += *.ui
SOURCES += *.cpp
RESOURCES += tonatiuh.qrc
 

CONFIG(debug, debug|release) {
	DESTDIR = ../bin/debug
}
else{
	DESTDIR=../bin/release
}
	
QMAKE_CLEAN -= *.rc  