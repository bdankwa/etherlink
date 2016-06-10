#include <qapplication.h>
#include <QString>
#include <QTextDocument>
#include <stdlib.h>
#include "guicontroller.h"

#define PACKET1	0
#define PACKET2	1
#define PACKET3	2
#define PACKET4	3
#define USERDEF	4
#define FAULTS	5


using namespace Ui;

GuiController::GuiController(UIThreadArgs_t* threadParams)
{
	 mainWind = new Ui_MainWindow();
	 args = threadParams;


}

void GuiController::showGui()
{
    QApplication app( args->argc, args->argv );

    QMainWindow* main = new QMainWindow();

    ereader = new EthernetReader((args->global_data->rxBuff), NUM_PORTS);
    connect(ereader, SIGNAL(newPacket(unsigned int*, unsigned int, int)), this, SLOT(streamPacket(unsigned int*, unsigned int, int)) );

    //ereader->moveToThread(QApplication::instance()->thread());

    app.setActiveWindow( main );
    mainWind->setupUi(main);
    //connect(mainWind->lineEdit_SpdVibPacketNumber, SIGNAL(textChanged(QString)), this , SLOT(readPacketNumber(QString)));
    connect(mainWind->actionStart, SIGNAL(triggered()), this , SLOT(startStream()));
    main->show();
    app.exec();
}

void GuiController::streamPacket(unsigned int* packet, unsigned int size, int portIdx)
{
	static int num = 0;
	//TODO Determine packet type

	//TODO Use switch statement to set appropriate tab info
	//printf("new packet signal received, processing..\n");
	mainWind->lineEdit_SpdVibPacketNumber->setText(QString::number(size));

	switch(portIdx){

	case PACKET1:
		//process packet1
		printf("GOT PACKET 1\n");
		break;
	case PACKET2:
		//process packet2
		printf("GOT PACKET 2\n");
		break;
	case PACKET3:
		//process packet3
		printf("GOT PACKET 3\n");
		break;
	case PACKET4:
		//process packet4
		printf("GOT PACKET 4\n");
		break;
	case USERDEF:
		//process user packet
		printf("GOT PACKET 5\n");
		break;
	case FAULTS:
		processFaults();
		printf("GOT PACKET 6\n");
		break;
	default:
		;
	}

}

void GuiController::readPacketNumber(QString packetNumberString)
{
	QTextDocument* text = new QTextDocument(packetNumberString);
	int packetNum = packetNumberString.toInt();
	mainWind->lineEdit_SpdVibRefreshRate->setText(packetNumberString);
	mainWind->plainTextEdit_3->setDocument(text);

}

void GuiController::startStream()
{
    ereader->start();
}

void GuiController::processFaults()
{

}

