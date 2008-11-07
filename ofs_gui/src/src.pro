TEMPLATE = app
CONFIG += release \
          warn_on \
	  thread \
          qt
TARGET = ../bin/ofs_gui
SOURCES += guimanager.cpp \
ofsadvancedsettingsdlg.cpp \
ofscachingsettingsdlg.cpp \
ofsconfirmfiledeletedlg.cpp \
ofs_gui.cpp \
ofspropertiesdlg.cpp
HEADERS += guimanager.h \
ofsadvancedsettingsdlg.h \
ofscachingsettingsdlg.h \
ofsconfirmfiledeletedlg.h \
ofspropertiesdlg.h
FORMS += OFS_AdvancedSettings.ui \
OFS_CachingSettings.ui \
OFS_ConfirmFileDelete.ui \
OFS_Properties.ui
OBJECTS_DIR = ./tmp/

MOC_DIR = ./moc/

