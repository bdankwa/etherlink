#------------------------------------------------------------------------------
# Makefile for building code to test EEC EMU Inteface
#------------------------------------------------------------------------------
UIMAINWINDOW = etherlinkMainWindow
TESTWINDOW = testWindow
SRCDIR = ./src/
DESIGNDIR = ./gui/
UISRCDIR = $(SRCDIR)ui/

QTDIR = /usr/include/qt4/
QTCOREDIR = $(QTDIR)QtCore/
QTGUIDIR = $(QTDIR)QtGui/

#QTDIR = /usr/include/Qt/
#QTCOREDIR = /usr/include/QtCore/
#QTGUIDIR = /usr/include/QtGui/

CNTRLDIR = $(SRCDIR)controller/
INFRADIR = $(SRCDIR)infrustructure/
UTILSDIR = $(SRCDIR)utils/


CNTRLOBJ = $(OBJDIR)buffer.o $(OBJDIR)process_ethernet.o $(OBJDIR)EthernetTransmit.o $(OBJDIR)moc_EthernetTransmit.o
INFRAOBJ = $(OBJDIR)ethernet.o $(OBJDIR)A664Filter.o $(OBJDIR)moc_A664Filter.o
UTILSOBJ = $(OBJDIR)utils.o

UIOBJECTS =  $(OBJDIR)$(UIMAINWINDOW).o $(OBJDIR)$(TESTWINDOW).o $(OBJDIR)moc_guicontroller.o $(OBJDIR)guicontroller.o \
			 $(OBJDIR)moc_EthernetReader.o $(OBJDIR)EthernetReader.o

OBJDIR = ./obj/

OBJECTS = $(UIOBJECTS)  $(CNTRLOBJ) $(INFRAOBJ) $(UTILSOBJ)  $(OBJDIR)etherlink.o
TARGET = etherlink
QTINC = -I$(QTDIR) -I$(QTCOREDIR) -I$(QTGUIDIR) -I$(CNTRLDIR)
APPINC = -I$(CNTRLDIR) -I$(INFRADIR) -I$(UTILSDIR)
UIINC = -I$(UISRCDIR) 
QTLIB = qt
GPP = g++
GCC = gcc
MOC = moc
UIC = uic
CFLAGS = -Wall -c
LFLAGS = -lQtCore -lQtGui -lpthread 
#------------------------------------------------------------------------------



all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(GPP) -L/usr/lib64/ -L$(QTCOREDIR) -L$(QTGUIDIR) -L$(APPINC) $(LFLAGS) $(OBJECTS) -o $(TARGET)

$(OBJDIR)etherlink.o : $(SRCDIR)etherlink.cpp
	$(GPP) $(CFLAGS) $(UIINC) $(QTINC) $(APPINC) $< -o $@

$(OBJDIR)$(UIMAINWINDOW).o : $(UISRCDIR)$(UIMAINWINDOW).cpp $(UISRCDIR)$(UIMAINWINDOW).h
	$(GPP) $(CFLAGS) $(QTINC) $< -o $@

$(OBJDIR)$(TESTWINDOW).o : $(UISRCDIR)$(TESTWINDOW).cpp $(UISRCDIR)$(TESTWINDOW).h
	$(GPP) $(CFLAGS) $(QTINC) $< -o $@


$(OBJDIR)moc_EthernetTransmit.o : $(CNTRLDIR)moc_EthernetTransmit.cpp $(CNTRLDIR)EthernetTransmit.h
	$(GPP) $(CFLAGS) $(QTINC) $< -o $@

$(OBJDIR)EthernetTransmit.o : $(CNTRLDIR)EthernetTransmit.cpp $(CNTRLDIR)EthernetTransmit.h
	$(GPP) $(CFLAGS) $(QTINC) $(APPINC) $< -o $@
	
$(CNTRLDIR)moc_EthernetTransmit.cpp : $(CNTRLDIR)EthernetTransmit.h
	$(MOC) $(QTINC) $< -o $@


	
$(OBJDIR)moc_A664Filter.o : $(UISRCDIR)moc_A664Filter.cpp $(INFRADIR)A664Filter.h
	$(GPP) $(CFLAGS) $(QTINC) $< -o $@

$(OBJDIR)A664Filter.o : $(INFRADIR)A664Filter.cpp $(INFRADIR)A664Filter.h
	$(GPP) $(CFLAGS) $(QTINC) $(APPINC) $< -o $@
	
$(UISRCDIR)moc_A664Filter.cpp : $(INFRADIR)A664Filter.h
	$(MOC) $(QTINC) $< -o $@

$(OBJDIR)moc_EthernetReader.o : $(UISRCDIR)moc_EthernetReader.cpp $(UISRCDIR)EthernetReader.h
	$(GPP) $(CFLAGS) $(QTINC) $< -o $@

$(OBJDIR)EthernetReader.o : $(UISRCDIR)EthernetReader.cpp $(UISRCDIR)EthernetReader.h
	$(GPP) $(CFLAGS) $(QTINC) $< -o $@

$(UISRCDIR)moc_EthernetReader.cpp : $(UISRCDIR)EthernetReader.h
	$(MOC) $(QTINC) $< -o $@

$(OBJDIR)moc_guicontroller.o : $(UISRCDIR)moc_guicontroller.cpp $(UISRCDIR)guicontroller.h
	$(GPP) $(CFLAGS) $(QTINC) $(APPINC) $< -o $@

$(OBJDIR)guicontroller.o : $(UISRCDIR)guicontroller.cpp $(UISRCDIR)guicontroller.h
	$(GPP) $(CFLAGS) $(QTINC) $(APPINC) $< -o $@

$(UISRCDIR)moc_guicontroller.cpp : $(UISRCDIR)guicontroller.h
	$(MOC) $(QTINC) $< -o $@

$(UISRCDIR)$(UIMAINWINDOW).cpp : $(DESIGNDIR)etherlink.ui
	$(UIC) $< -o $@
	
$(UISRCDIR)$(UIMAINWINDOW).h : $(DESIGNDIR)etherlink.ui
	$(UIC) $< -o $@
	
$(UISRCDIR)$(TESTWINDOW).cpp : $(DESIGNDIR)ethernetTest.ui
	$(UIC) $< -o $@
	
$(UISRCDIR)$(TESTWINDOW).h : $(DESIGNDIR)ethernetTest.ui
	$(UIC) $< -o $@



	

	
$(OBJDIR)process_ethernet.o : $(CNTRLDIR)process_ethernet.c
	$(GPP) $(CFLAGS) $(APPINC) $(CNTRLDIR)process_ethernet.c -o $(OBJDIR)process_ethernet.o

$(OBJDIR)buffer.o : $(CNTRLDIR)buffer.c
	$(GPP) $(CFLAGS) $(APPINC) $(CNTRLDIR)buffer.c -o $(OBJDIR)buffer.o
	
$(OBJDIR)ethernet.o : $(INFRADIR)ethernet.c
	$(GPP) $(CFLAGS) $(APPINC) $(INFRADIR)ethernet.c -o $(OBJDIR)ethernet.o

$(OBJDIR)utils.o : $(UTILSDIR)utils.c
	$(GPP) $(CFLAGS) $(APPINC) $(UTILSDIR)utils.c -o $(OBJDIR)utils.o

clean:

	rm -f $(OBJECTS) $(TARGET) $(UISRCDIR)etherlink* $(UISRCDIR)moc*
