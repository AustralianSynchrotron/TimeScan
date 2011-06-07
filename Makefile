#############################################################################
# Makefile for building: TimeScanMX
# Generated by qmake (2.01a) (Qt 4.7.3) on: Tue Jun 7 17:58:10 2011
# Project:  TimeScanMX.pro
# Template: app
# Command: /usr/bin/qmake -spec /usr/share/qt4/mkspecs/linux-g++ QMLJSDEBUGGER_PATH=/usr/share/qtcreator/qml/qmljsdebugger -o Makefile TimeScanMX.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include/qwt5 -I/usr/include/qwt-qt4/ -I. -I.
LINK          = g++
LFLAGS        = -Wl,-O1 -Wl,-rpath,/usr/lib64/qt4
LIBS          = $(SUBLIBS)  -L/usr/lib64/qt4 -lqwt -lqtpv -lblitz -lpoptmx -lQtGui -L/usr/lib64 -L/usr/lib64/qt4 -L/usr/X11R6/lib -lQtCore -lgthread-2.0 -lrt -lglib-2.0 -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		qchartmx.cpp moc_qchartmx.cpp \
		qrc_qchartmx.cpp
OBJECTS       = main.o \
		qchartmx.o \
		moc_qchartmx.o \
		qrc_qchartmx.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_phonon.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		TimeScanMX.pro
QMAKE_TARGET  = TimeScanMX
DESTDIR       = 
TARGET        = TimeScanMX

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_qchartmx.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: TimeScanMX.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_phonon.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib64/qt4/libQtGui.prl \
		/usr/lib64/qt4/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ QMLJSDEBUGGER_PATH=/usr/share/qtcreator/qml/qmljsdebugger -o Makefile TimeScanMX.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_phonon.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib64/qt4/libQtGui.prl:
/usr/lib64/qt4/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ QMLJSDEBUGGER_PATH=/usr/share/qtcreator/qml/qmljsdebugger -o Makefile TimeScanMX.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/TimeScanMX1.0.0 || $(MKDIR) .tmp/TimeScanMX1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/TimeScanMX1.0.0/ && $(COPY_FILE) --parents qchartmx.h .tmp/TimeScanMX1.0.0/ && $(COPY_FILE) --parents qchartmx.qrc .tmp/TimeScanMX1.0.0/ && $(COPY_FILE) --parents main.cpp qchartmx.cpp .tmp/TimeScanMX1.0.0/ && $(COPY_FILE) --parents qchartmx.ui .tmp/TimeScanMX1.0.0/ && (cd `dirname .tmp/TimeScanMX1.0.0` && $(TAR) TimeScanMX1.0.0.tar TimeScanMX1.0.0 && $(COMPRESS) TimeScanMX1.0.0.tar) && $(MOVE) `dirname .tmp/TimeScanMX1.0.0`/TimeScanMX1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/TimeScanMX1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_qchartmx.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_qchartmx.cpp
moc_qchartmx.cpp: ui_qchartmx.h \
		qchartmx.h
	/usr/bin/moc $(DEFINES) $(INCPATH) qchartmx.h -o moc_qchartmx.cpp

compiler_rcc_make_all: qrc_qchartmx.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_qchartmx.cpp
qrc_qchartmx.cpp: qchartmx.qrc \
		Graph1-small.png
	/usr/bin/rcc -name qchartmx qchartmx.qrc -o qrc_qchartmx.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_qchartmx.h
compiler_uic_clean:
	-$(DEL_FILE) ui_qchartmx.h
ui_qchartmx.h: qchartmx.ui
	/usr/bin/uic qchartmx.ui -o ui_qchartmx.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean compiler_uic_clean 

####### Compile

main.o: main.cpp qchartmx.h \
		ui_qchartmx.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

qchartmx.o: qchartmx.cpp qchartmx.h \
		ui_qchartmx.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qchartmx.o qchartmx.cpp

moc_qchartmx.o: moc_qchartmx.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_qchartmx.o moc_qchartmx.cpp

qrc_qchartmx.o: qrc_qchartmx.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o qrc_qchartmx.o qrc_qchartmx.cpp

####### Install

install_target: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/bin/ || $(MKDIR) $(INSTALL_ROOT)/bin/ 
	-$(INSTALL_PROGRAM) "$(QMAKE_TARGET)" "$(INSTALL_ROOT)/bin/$(QMAKE_TARGET)"
	-$(STRIP) "$(INSTALL_ROOT)/bin/$(QMAKE_TARGET)"

uninstall_target:  FORCE
	-$(DEL_FILE) "$(INSTALL_ROOT)/bin/$(QMAKE_TARGET)"
	-$(DEL_DIR) $(INSTALL_ROOT)/bin/ 


install:  install_target  FORCE

uninstall: uninstall_target   FORCE

FORCE:

