#include <qapplication.h>
#include <QString>
#include <QTextDocument>
#include <stdlib.h>
#include "guicontroller.h"

#define PORT_PACKET1	0
#define PORT_PACKET2	1

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

    ereader = new EthernetReader(&((args->global_data->rxBuff)[PORT_PACKET1]));
    connect(ereader, SIGNAL(newPacket(unsigned int*, unsigned int)), this, SLOT(streamPacket(unsigned int*, unsigned int)) );

    //ereader->moveToThread(QApplication::instance()->thread());

    app.setActiveWindow( main );
    mainWind->setupUi(main);
    //connect(mainWind->lineEdit_SpdVibPacketNumber, SIGNAL(textChanged(QString)), this , SLOT(readPacketNumber(QString)));
    connect(mainWind->actionStart, SIGNAL(triggered()), this , SLOT(startStream()));
    main->show();
    app.exec();
}

void GuiController::streamPacket(unsigned int* packet, unsigned int size)
{
	static int num = 0;
	//TODO Determine packet type

	//TODO Use switch statement to set appropriate tab info
	printf("new packet signal received, processing..\n");
	mainWind->lineEdit_SpdVibPacketNumber->setText(QString::number(size));

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


