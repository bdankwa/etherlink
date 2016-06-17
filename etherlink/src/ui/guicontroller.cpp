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

#define EXTRACT_BIT(word, k) ((word & (1 << k)) >> k)


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
    connect(mainWind->actionPause, SIGNAL(triggered()), this , SLOT(pauseStream()));
    connect(mainWind->actionStop, SIGNAL(triggered()), this , SLOT(stopStream()));
    main->show();
    app.exec();
}

void GuiController::streamPacket(unsigned int* packets, unsigned int size, int portIdx)
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
		processPacket1(packets);
		break;
	case PACKET2:
		//process packet2
		printf("GOT PACKET 2\n");
		processPacket2(packets);
		break;
	case PACKET3:
		//process packet3
		printf("GOT PACKET 3\n");
		processPacket3(packets);
		break;
	case PACKET4:
		//process packet4
		printf("GOT PACKET 4\n");
		processPacket4(packets);
		break;
	case USERDEF:
		//process user packet
		printf("GOT PACKET 5\n");
		processPacket5(packets);
		break;
	case FAULTS:
		processFaults(packets);
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
	ereader->resume();
    ereader->start();
}

void GuiController::pauseStream()
{
    ereader->pause();
}

void GuiController::stopStream()
{
    ereader->pause();
}

void GuiController::processPacket1(unsigned int* packets)
{

}

void GuiController::processPacket2(unsigned int* packets)
{

}

void GuiController::processPacket3(unsigned int* packets)
{

}

void GuiController::processPacket4(unsigned int* packets)
{

}

void GuiController::processPacket5(unsigned int* packets)
{

}

void GuiController::processFaults(unsigned int* faults)
{
	unsigned int cbitFaultWord;
	unsigned int pbitFaultWord;
	unsigned int nvmFaultWord;
	unsigned int bootFaultWord;
	unsigned int mcbspFaultWord;
	unsigned int rs422FaultWord;
	unsigned int ethernetStatusWord;
	unsigned int ethernetFaultWord;
	unsigned int ptmFaultWord;

	cbitFaultWord = faults[0];
	pbitFaultWord = faults[1];
	nvmFaultWord = faults[2];
	bootFaultWord = faults[3];
	mcbspFaultWord = faults[4];
	rs422FaultWord = faults[5];
	ethernetStatusWord = faults[6];
	ethernetFaultWord = faults[7];
	ptmFaultWord = faults[8];

	mainWind->lineEdit_cbit0->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 0)));
	mainWind->lineEdit_cbit1->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 1)));
	mainWind->lineEdit_cbit2->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 2)));
	mainWind->lineEdit_cbit3->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 3)));
	mainWind->lineEdit_cbit4->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 4)));
	mainWind->lineEdit_cbit5->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 5)));
	mainWind->lineEdit_cbit6->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 6)));
	mainWind->lineEdit_cbit7->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 7)));
	mainWind->lineEdit_cbit8->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 8)));
	mainWind->lineEdit_cbit9->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 9)));
	mainWind->lineEdit_cbit10->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 10)));
	mainWind->lineEdit_cbit11->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 11)));
	mainWind->lineEdit_cbit12->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 12)));
	mainWind->lineEdit_cbit13->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 13)));
	mainWind->lineEdit_cbit14->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 14)));
	mainWind->lineEdit_cbit15->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 15)));
	mainWind->lineEdit_cbit16->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 16)));
	mainWind->lineEdit_cbit17->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 17)));
	mainWind->lineEdit_cbit18->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 18)));
	mainWind->lineEdit_cbit19->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 19)));
	mainWind->lineEdit_cbit20->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 20)));
	mainWind->lineEdit_cbit21->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 21)));
	mainWind->lineEdit_cbit22->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 22)));
	mainWind->lineEdit_cbit23->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 23)));
	mainWind->lineEdit_cbit24->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 24)));
	mainWind->lineEdit_cbit25->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 25)));
	mainWind->lineEdit_cbit26->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 26)));
	mainWind->lineEdit_cbit27->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 27)));
	mainWind->lineEdit_cbit28->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 28)));
	mainWind->lineEdit_cbit29->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 29)));
	mainWind->lineEdit_cbit30->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 30)));
	mainWind->lineEdit_cbit31->setText(QString::number(EXTRACT_BIT(cbitFaultWord, 31)));

	mainWind->lineEdit_pbit0->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 0)));
	mainWind->lineEdit_pbit1->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 1)));
	mainWind->lineEdit_pbit2->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 2)));
	mainWind->lineEdit_pbit3->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 3)));
	mainWind->lineEdit_pbit4->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 4)));
	mainWind->lineEdit_pbit5->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 5)));
	mainWind->lineEdit_pbit6->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 6)));
	mainWind->lineEdit_pbit7->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 7)));
	mainWind->lineEdit_pbit8->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 8)));
	mainWind->lineEdit_pbit9->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 9)));
	mainWind->lineEdit_pbit10->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 10)));
	mainWind->lineEdit_pbit11->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 11)));
	mainWind->lineEdit_pbit12->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 12)));
	mainWind->lineEdit_pbit13->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 13)));
	mainWind->lineEdit_pbit14->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 14)));
	mainWind->lineEdit_pbit15->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 15)));
	mainWind->lineEdit_pbit16->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 16)));
	mainWind->lineEdit_pbit17->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 17)));
	mainWind->lineEdit_pbit18->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 18)));
	mainWind->lineEdit_pbit19->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 19)));
	mainWind->lineEdit_pbit20->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 20)));
	mainWind->lineEdit_pbit21->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 21)));
	mainWind->lineEdit_pbit22->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 22)));
	mainWind->lineEdit_pbit23->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 23)));
	mainWind->lineEdit_pbit24->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 24)));
	mainWind->lineEdit_pbit25->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 25)));
	mainWind->lineEdit_pbit26->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 26)));
	mainWind->lineEdit_pbit27->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 27)));
	mainWind->lineEdit_pbit28->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 28)));
	mainWind->lineEdit_pbit29->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 29)));
	mainWind->lineEdit_pbit30->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 30)));
	mainWind->lineEdit_pbit31->setText(QString::number(EXTRACT_BIT(pbitFaultWord, 31)));













	mainWind->lineEdit_ethStatus_0->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 0)));
	mainWind->lineEdit_ethStatus_1->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 1)));
	mainWind->lineEdit_ethStatus_2->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 2)));
	mainWind->lineEdit_ethStatus_3->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 3)));
	mainWind->lineEdit_ethStatus_4->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 4)));
	mainWind->lineEdit_ethStatus_5->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 5)));
	mainWind->lineEdit_ethStatus_6->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 6)));
	mainWind->lineEdit_ethStatus_7->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 7)));
	mainWind->lineEdit_ethStatus_8->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 8)));
	mainWind->lineEdit_ethStatus_9->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 9)));
	mainWind->lineEdit_ethStatus_10->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 10)));
	mainWind->lineEdit_ethStatus_11->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 11)));
	mainWind->lineEdit_ethStatus_12->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 12)));
	mainWind->lineEdit_ethStatus_13->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 13)));
	mainWind->lineEdit_ethStatus_14->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 14)));
	mainWind->lineEdit_ethStatus_15->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 15)));
	mainWind->lineEdit_ethStatus_16->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 16)));
	mainWind->lineEdit_ethStatus_17->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 17)));
	mainWind->lineEdit_ethStatus_18->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 18)));
	mainWind->lineEdit_ethStatus_19->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 19)));
	mainWind->lineEdit_ethStatus_20->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 20)));
	mainWind->lineEdit_ethStatus_21->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 21)));
	mainWind->lineEdit_ethStatus_22->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 22)));
	mainWind->lineEdit_ethStatus_23->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 23)));
	mainWind->lineEdit_ethStatus_24->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 24)));
	mainWind->lineEdit_ethStatus_25->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 25)));
	mainWind->lineEdit_ethStatus_26->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 26)));
	mainWind->lineEdit_ethStatus_27->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 27)));
	mainWind->lineEdit_ethStatus_28->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 28)));
	mainWind->lineEdit_ethStatus_29->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 29)));
	mainWind->lineEdit_ethStatus_30->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 30)));
	mainWind->lineEdit_ethStatus_31->setText(QString::number(EXTRACT_BIT(ethernetStatusWord, 31)));

	mainWind->lineEdit_ethFault_0->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 0)));
	mainWind->lineEdit_ethFault_1->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 1)));
	mainWind->lineEdit_ethFault_2->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 2)));
	mainWind->lineEdit_ethFault_3->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 3)));
	mainWind->lineEdit_ethFault_4->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 4)));
	mainWind->lineEdit_ethFault_5->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 5)));
	mainWind->lineEdit_ethFault_6->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 6)));
	mainWind->lineEdit_ethFault_7->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 7)));
	mainWind->lineEdit_ethFault_8->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 8)));
	mainWind->lineEdit_ethFault_9->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 9)));
	mainWind->lineEdit_ethFault_10->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 10)));
	mainWind->lineEdit_ethFault_11->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 11)));
	mainWind->lineEdit_ethFault_12->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 12)));
	mainWind->lineEdit_ethFault_13->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 13)));
	mainWind->lineEdit_ethFault_14->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 14)));
	mainWind->lineEdit_ethFault_15->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 15)));
	mainWind->lineEdit_ethFault_16->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 16)));
	mainWind->lineEdit_ethFault_17->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 17)));
	mainWind->lineEdit_ethFault_18->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 18)));
	mainWind->lineEdit_ethFault_19->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 19)));
	mainWind->lineEdit_ethFault_20->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 20)));
	mainWind->lineEdit_ethFault_21->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 21)));
	mainWind->lineEdit_ethFault_22->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 22)));
	mainWind->lineEdit_ethFault_23->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 23)));
	mainWind->lineEdit_ethFault_24->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 24)));
	mainWind->lineEdit_ethFault_25->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 25)));
	mainWind->lineEdit_ethFault_26->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 26)));
	mainWind->lineEdit_ethFault_27->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 27)));
	mainWind->lineEdit_ethFault_28->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 28)));
	mainWind->lineEdit_ethFault_29->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 29)));
	mainWind->lineEdit_ethFault_30->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 30)));
	mainWind->lineEdit_ethFault_31->setText(QString::number(EXTRACT_BIT(ethernetFaultWord, 31)));

	mainWind->lineEdit_ptm0->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 0)));
	mainWind->lineEdit_ptm1->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 1)));
	mainWind->lineEdit_ptm2->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 2)));
	mainWind->lineEdit_ptm3->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 3)));
	mainWind->lineEdit_ptm4->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 4)));
	mainWind->lineEdit_ptm5->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 5)));
	mainWind->lineEdit_ptm6->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 6)));
	mainWind->lineEdit_ptm7->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 7)));
	mainWind->lineEdit_ptm8->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 8)));
	mainWind->lineEdit_ptm9->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 9)));
	mainWind->lineEdit_ptm10->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 10)));
	mainWind->lineEdit_ptm11->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 11)));
	mainWind->lineEdit_ptm12->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 12)));
	mainWind->lineEdit_ptm13->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 13)));
	mainWind->lineEdit_ptm14->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 14)));
	mainWind->lineEdit_ptm15->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 15)));
	mainWind->lineEdit_ptm16->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 16)));
	mainWind->lineEdit_ptm17->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 17)));
	mainWind->lineEdit_ptm18->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 18)));
	mainWind->lineEdit_ptm19->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 19)));
	mainWind->lineEdit_ptm20->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 20)));
	mainWind->lineEdit_ptm21->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 21)));
	mainWind->lineEdit_ptm22->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 22)));
	mainWind->lineEdit_ptm23->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 23)));
	mainWind->lineEdit_ptm24->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 24)));
	mainWind->lineEdit_ptm25->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 25)));
	mainWind->lineEdit_ptm26->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 26)));
	mainWind->lineEdit_ptm27->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 27)));
	mainWind->lineEdit_ptm28->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 28)));
	mainWind->lineEdit_ptm29->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 29)));
	mainWind->lineEdit_ptm30->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 30)));
	mainWind->lineEdit_ptm31->setText(QString::number(EXTRACT_BIT(ptmFaultWord, 31)));


}

