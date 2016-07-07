#include <qapplication.h>
#include <QString>
#include <QTextDocument>
#include <QIntValidator>
#include <stdlib.h>
#include <stdio.h>
#include "guicontroller.h"
#include "A664Filter.h"
#include "process_ethernet.h"

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
	 testWind = new Ui_Test();

	 args = threadParams;
	 connectionStatus = args->global_data->connection;
	 redPalette = new QPalette();
	 redPalette->setColor(QPalette::WindowText, Qt::red);
	 greenPalette = new QPalette();
	 greenPalette->setColor(QPalette::WindowText, Qt::darkGreen);

	 packet1 = (unsigned int *)malloc(sizeof(unsigned int) * 1024);
	 packet2 = (unsigned int *)malloc(sizeof(unsigned int) * 1024);
	 packet3 = (unsigned int *)malloc(sizeof(unsigned int) * 1024);

	 packet1_idx = 0;
	 packet2_idx = 0;
	 packet3_idx = 0;
	 loopback_rx_count = 0;

	 streamingPaused = true;
	 testing = false;

}

void GuiController::showGui()
{
    QApplication app( args->argc, args->argv );

    main = new QMainWindow();
    test = new QMainWindow();

    app.setActiveWindow( main );
    mainWind->setupUi(main);
    testWind->setupUi(test);

    ereader = new EthernetReader((args->global_data->rxBuff), NUM_PORTS);
   // a664Filter = new A664Filter();
    ewriter = new EthernetTransmit();

    connect(ereader, SIGNAL(newPacket(unsigned int*, unsigned int, int)), this, SLOT(streamPacket(unsigned int*, unsigned int, int)) );
    connect(ewriter, SIGNAL(newTxPacket(unsigned int*, unsigned int)), this, SLOT(processTxEvent(unsigned int*, unsigned int)) );
    connect(ewriter, SIGNAL(daignosticsComplete()), this, SLOT(showDiagnosticsResults()) );

	mainWind->lineEdit_SpdVibPacketNumber->setValidator( new QIntValidator(0, 1023, this));
	mainWind->lineEdit_PressPacketNumber->setValidator( new QIntValidator(0, 255, this));
	mainWind->lineEdit_CalPacketNumber->setValidator( new QIntValidator(0, 255, this));

	mainWind->lineEdit_SpdVibRefreshRate->setValidator( new QIntValidator(25, 1000, this));
	mainWind->lineEdit_PressRefreshRate->setValidator( new QIntValidator(25, 1000, this));
	mainWind->lineEdit_CalRefreshRate->setValidator( new QIntValidator(25, 1000, this));

    connect(mainWind->lineEdit_SpdVibPacketNumber, SIGNAL(textChanged(QString)), this , SLOT(readPacket1Index(QString)));
    connect(mainWind->lineEdit_PressPacketNumber, SIGNAL(textChanged(QString)), this , SLOT(readPacket2Index(QString)));
    connect(mainWind->lineEdit_CalPacketNumber, SIGNAL(textChanged(QString)), this , SLOT(readPacket3Index(QString)));

    connect(mainWind->lineEdit_SpdVibRefreshRate, SIGNAL(textChanged(QString)), this , SLOT(readPacket1RefreshRate(QString)));
    connect(mainWind->lineEdit_PressRefreshRate, SIGNAL(textChanged(QString)), this , SLOT(readPacket2RefreshRate(QString)));
    connect(mainWind->lineEdit_CalRefreshRate, SIGNAL(textChanged(QString)), this , SLOT(readPacket3RefreshRate(QString)));

    connect(mainWind->pushButton_spdVib_dec, SIGNAL(clicked()), this , SLOT(decrementPacket1Idx()));
    connect(mainWind->pushButton_spdVib_inc, SIGNAL(clicked()), this , SLOT(incrementPacket1Idx()));

    connect(mainWind->pushButton_Press_dec, SIGNAL(clicked()), this , SLOT(decrementPacket2Idx()));
    connect(mainWind->pushButton_Press_inc, SIGNAL(clicked()), this , SLOT(incrementPacket2Idx()));

    connect(mainWind->pushButton_Cal_dec, SIGNAL(clicked()), this , SLOT(decrementPacket3Idx()));
    connect(mainWind->pushButton_Cal_inc, SIGNAL(clicked()), this , SLOT(incrementPacket3Idx()));

    connect(mainWind->actionStart, SIGNAL(triggered()), this , SLOT(startStream()));
    connect(mainWind->actionPause, SIGNAL(triggered()), this , SLOT(pauseStream()));
    connect(mainWind->actionStop, SIGNAL(triggered()), this , SLOT(stopStream()));
    connect(mainWind->actionDiagnostics, SIGNAL(triggered()), this , SLOT(showDiagnosticsWindow()));
    //connect(mainWind->actionStream, SIGNAL(triggered()), this , SLOT(showStreamWindow()));



    connect(testWind->startTestButton, SIGNAL(clicked()), this , SLOT(startDiagnostics()));
    connect(testWind->cancelTestButton, SIGNAL(clicked()), this , SLOT(cancelDiagnostics()));


	connectionLabel = new QLabel(mainWind->statusbar);
	mainWind->statusbar->addPermanentWidget(connectionLabel);
    connectionLabel->setText("NO RX TRAFFIC!");
    mainWind->statusbar->showMessage("Stopped");

    connectionLabel->setPalette(*redPalette);

    messageBox = new QMessageBox();


    //setFault(mainWind->lineEdit_cbit0, 0);
    //setFault(mainWind->lineEdit_cbit1, 1);

    main->show();
    app.exec();
}

void GuiController::streamPacket(unsigned int* packets, unsigned int size, int portIdx)
{
	static int num = 0;
	static int k = 0;


	if(!connectionStatus){
		connectionLabel->setText("NO RX TRAFFIC!");
		connectionLabel->setPalette(*redPalette);
	}
	else{
		connectionLabel->setText("CONNECTED!");
		connectionLabel->setPalette(*greenPalette);
	}

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
		//loopback_rx_count++;
		if(((k = k + 1) % 2) == 0){
			loopback_rx_count++;
			k = 0;
		}
		processLoopback(packets);
		//printf("GOT LOOPBACK PACKET\n");
		break;
	}

}

void GuiController::readPacket1Index(QString packetNumberString)
{
	int idx = packetNumberString.toInt();

	packet1_idx = (idx >= 0 && idx <= 1023)? idx : ((idx < 0)? 0: 1023);
}

void GuiController::readPacket2Index(QString packetNumberString)
{
	int idx = packetNumberString.toInt();
	packet2_idx = (idx >= 0 && idx <= 255)? idx : ((idx < 0)? 0: 255);
}

void GuiController::readPacket3Index(QString packetNumberString)
{
	int idx = packetNumberString.toInt();
	packet3_idx = (idx >= 0 && idx <= 255)? idx : ((idx < 0)? 0: 255);
}


void GuiController::readPacket1RefreshRate(QString packetNumberString)
{
	int rate = packetNumberString.toInt();

	packet1_refreshRate = (rate >= 25 && rate <= 1000)? rate : ((rate < 25)? 25: 1000);
	ereader->setRefreshRate(packet1_refreshRate);
}

void GuiController::readPacket2RefreshRate(QString packetNumberString)
{
	int rate = packetNumberString.toInt();

	packet2_refreshRate = (rate >= 25 && rate <= 1000)? rate : ((rate < 25)? 25: 1000);
	ereader->setRefreshRate(packet2_refreshRate);
}

void GuiController::readPacket3RefreshRate(QString packetNumberString)
{
	int rate = packetNumberString.toInt();

	packet3_refreshRate = (rate >= 25 && rate <= 1000)? rate : ((rate < 25)? 25: 1000);
	ereader->setRefreshRate(packet3_refreshRate);
}


void GuiController::decrementPacket1Idx()
{
	int idx = packet1_idx;
	idx = (idx-- <= 0) ? 0 : idx ;
	mainWind->lineEdit_SpdVibPacketNumber->setText(QString::number(idx));
	processPacket1(packet1);
}

void GuiController::decrementPacket2Idx()
{
	int idx = packet2_idx;
	idx = (idx-- <= 0) ? 0 : idx ;
	mainWind->lineEdit_PressPacketNumber->setText(QString::number(idx));
	processPacket2(packet2);

}

void GuiController::decrementPacket3Idx()
{
	int idx = packet3_idx;
	idx = (idx-- <= 0) ? 0 : idx ;
	mainWind->lineEdit_CalPacketNumber->setText(QString::number(idx));
	processPacket3(packet3);
}

void GuiController::incrementPacket1Idx()
{
	int idx = packet1_idx;
	idx = (idx++ >= 1023) ? 1023 : idx ;
	mainWind->lineEdit_SpdVibPacketNumber->setText(QString::number(idx));
	processPacket1(packet1);
}

void GuiController::incrementPacket2Idx()
{
	int idx = packet2_idx;
	idx = (idx++ >= 255) ? 255 : idx ;
	mainWind->lineEdit_PressPacketNumber->setText(QString::number(idx));
	processPacket2(packet2);

}

void GuiController::incrementPacket3Idx()
{
	int idx = packet3_idx;
	idx = (idx++ >= 255) ? 255 : idx ;
	mainWind->lineEdit_CalPacketNumber->setText(QString::number(idx));
	processPacket3(packet3);

}

void GuiController::startStream()
{
	if(!(test->isVisible())){
		ereader->resume();
	    ereader->start();
	    mainWind->statusbar->showMessage("Streaming...");
	    streamingPaused = false;

	    mainWind->pushButton_spdVib_dec->setEnabled(false);
	    mainWind->pushButton_spdVib_inc->setEnabled(false);
	    mainWind->pushButton_Press_dec->setEnabled(false);
	    mainWind->pushButton_Press_inc->setEnabled(false);
	    mainWind->pushButton_Cal_dec->setEnabled(false);
	    mainWind->pushButton_Cal_inc->setEnabled(false);
	    //connectionLabel->setText("CONNECTED TO EMU!");
	}

}

void GuiController::pauseStream()
{
	if(!(test->isVisible())){
	    ereader->pause();
	    mainWind->statusbar->showMessage("Paused");
	    streamingPaused = true;

	    mainWind->pushButton_spdVib_dec->setEnabled(true);
	    mainWind->pushButton_spdVib_inc->setEnabled(true);
	    mainWind->pushButton_Press_dec->setEnabled(true);
	    mainWind->pushButton_Press_inc->setEnabled(true);
	    mainWind->pushButton_Cal_dec->setEnabled(true);
	    mainWind->pushButton_Cal_inc->setEnabled(true);

	}

}

void GuiController::stopStream()
{
	if(!(test->isVisible())){
	    ereader->pause();
	    mainWind->statusbar->showMessage("Stopped");
	    streamingPaused = true;

	    mainWind->pushButton_spdVib_dec->setEnabled(true);
	    mainWind->pushButton_spdVib_inc->setEnabled(true);
	    mainWind->pushButton_Press_dec->setEnabled(true);
	    mainWind->pushButton_Press_inc->setEnabled(true);
	    mainWind->pushButton_Cal_dec->setEnabled(true);
	    mainWind->pushButton_Cal_inc->setEnabled(true);
	}

}

void GuiController::showDiagnosticsWindow()
{
	if(!(test->isVisible())){

	    mainWind->statusbar->showMessage("Loopback Mode");
	    //ewriter->cancel();
		//testWind->label_loopback_tx_packets->setText(QString::number(0));
		//testWind->label_loopback_rx_packets->setText(QString::number(0));

		//a664Filter->start();

	    resetDiagnostics();


		testing = true;

		//testWind->lable_CapacityTest_Status->setText("Ready");
		//testWind->cancelTestButton->setEnabled(false);

		ereader->pause();

		test->show();
	}

}

void GuiController::showStreamWindow()
{
	testing = false;

}

void GuiController::startDiagnostics()
{
	char* args[12];

	testWind->lable_CapacityTest_Status->setText("Running");
	testWind->startTestButton->setEnabled(false);
	testWind->cancelTestButton->setEnabled(true);
	testWind->comboBox_channel_select->setEnabled(false);
	loopback_rx_count = 0;
	loopback_tx_count = 0;
	ereader->resume();
    ereader->start();

	ewriter->reset(testWind->comboBox_channel_select->currentIndex());
    //resetDiagnostics();
	ewriter->start();

}

void GuiController::resetDiagnostics()
{
	loopback_rx_count = 0;
	loopback_tx_count = 0;
	ewriter->cancel();
	ewriter->reset(testWind->comboBox_channel_select->currentIndex());

	testWind->label_loopback_tx_packets->setText(QString::number(loopback_tx_count));
	testWind->label_loopback_rx_packets->setText(QString::number(loopback_rx_count));
	testWind->lable_CapacityTest_Status->setText("Ready");
	testWind->cancelTestButton->setEnabled(false);
	testWind->startTestButton->setEnabled(true);

	testWind->progressBar_loopback->setValue(0);


}

void GuiController::processTxEvent(unsigned int* txBuf, unsigned int sent_bytes)
{
	loopback_tx_count = ewriter->getTxCount();

	testWind->label_loopback_tx_packets->setText(QString::number(loopback_tx_count));
	testWind->label_loopback_rx_packets->setText(QString::number(loopback_rx_count));

	testWind->progressBar_loopback->setValue((double)loopback_tx_count/MAX_TEST_LOOPS * 100);

}

void GuiController::cancelDiagnostics()
{
	testWind->lable_CapacityTest_Status->setText("cancelled");
	ewriter->cancel();
	testWind->startTestButton->setEnabled(true);
	//loopback_tx_count = 0;
	//loopback_rx_count = 0;

}

void GuiController::showDiagnosticsResults()
{
	testWind->startTestButton->setEnabled(true);
	testWind->comboBox_channel_select->setEnabled(true);

	if(loopback_tx_count == loopback_rx_count ){
		testWind->lable_CapacityTest_Status->setText("PASS");
	}
	else{
		testWind->lable_CapacityTest_Status->setText("FAIL");
	}

	testWind->cancelTestButton->setEnabled(false);

}


void GuiController::processPacket1(unsigned int* packets)
{
	if(!streamingPaused){
		memset(packet1, 0, sizeof(unsigned int)*1024);
		memcpy(packet1, (packets + 17*packet1_idx), sizeof(unsigned int)*1024);
	}

	//mainWind->lineEdit_spdVib_Header_raw->setText(QString::number(packet1_idx, 16));

	mainWind->lineEdit_spdVib_Header_raw->setText(QString::number((*(packets+(17*packet1_idx))),16));
	mainWind->lineEdit_spdVib_Status_raw->setText(QString::number((*(packets+(17*packet1_idx) + 1)),16));
	mainWind->lineEdit_spdVib_N1CA_raw->setText(QString::number((*(packets+(17*packet1_idx) + 2)),16));
	mainWind->lineEdit_spdVib_N2CA_raw->setText(QString::number((*(packets+(17*packet1_idx) + 3)),16));
	mainWind->lineEdit_spdVib_N1CB_raw->setText(QString::number((*(packets+(17*packet1_idx) + 4)),16));
	mainWind->lineEdit_spdVib_N2CB_raw->setText(QString::number((*(packets+(17*packet1_idx) + 5)),16));
	mainWind->lineEdit_spdVib_N3A_raw->setText(QString::number((*(packets+(17*packet1_idx) + 6)),16));
	mainWind->lineEdit_spdVib_N1T_raw->setText(QString::number((*(packets+(17*packet1_idx) + 7)),16));
	mainWind->lineEdit_spdVib_Wdt_raw->setText(QString::number((*(packets+(17*packet1_idx) + 8)),16));
	mainWind->lineEdit_spdVib_Unused1_raw->setText(QString::number((*(packets+(17*packet1_idx) + 9)),16));
	mainWind->lineEdit_spdVib_ADCSmpl_raw->setText(QString::number((*(packets+(17*packet1_idx) + 10)),16));
	mainWind->lineEdit_spdVib_GearVib_raw->setText(QString::number((*(packets+(17*packet1_idx) + 11)),16));
	mainWind->lineEdit_spdVib_EngVib1_raw->setText(QString::number((*(packets+(17*packet1_idx) + 12)),16));
	mainWind->lineEdit_spdVib_EngVib2_raw->setText(QString::number((*(packets+(17*packet1_idx) + 13)),16));
	mainWind->lineEdit_spdVib_OPRDet_raw->setText(QString::number((*(packets+(17*packet1_idx) + 14)),16));
	mainWind->lineEdit_spdVib_CFPGANum_raw->setText(QString::number((*(packets+(17*packet1_idx) + 15)),16));
	mainWind->lineEdit_spdVib_Unused2_raw->setText(QString::number((*(packets+(17*packet1_idx) + 16)),16));

	//Perform binary to mv conversion
}

void GuiController::processPacket2(unsigned int* packets)
{
	if(!streamingPaused){
		memset(packet2, 0, sizeof(unsigned int)*256);
		memcpy(packet2, (packets + 17*packet2_idx), sizeof(unsigned int)*256);
	}

	mainWind->lineEdit_Press_Header_raw->setText(QString::number((*(packets+(17*packet2_idx))),16));
	mainWind->lineEdit_Press_Status_raw->setText(QString::number((*(packets+(17*packet2_idx) + 1)),16));
	mainWind->lineEdit_Press_P160_raw->setText(QString::number((*(packets+(17*packet2_idx) + 2)),16));
	mainWind->lineEdit_Press_P26_raw->setText(QString::number((*(packets+(17*packet2_idx) + 3)),16));
	mainWind->lineEdit_Press_T160_raw->setText(QString::number((*(packets+(17*packet2_idx) + 4)),16));
	mainWind->lineEdit_Press_T26_raw->setText(QString::number((*(packets+(17*packet2_idx) + 5)),16));
	mainWind->lineEdit_Press_ExcitR_raw->setText(QString::number((*(packets+(17*packet2_idx) + 6)),16));
	mainWind->lineEdit_Press_P160V0_raw->setText(QString::number((*(packets+(17*packet2_idx) + 7)),16));
	mainWind->lineEdit_Press_P26V0_raw->setText(QString::number((*(packets+(17*packet2_idx) + 8)),16));
	mainWind->lineEdit_Press_T160V0_raw->setText(QString::number((*(packets+(17*packet2_idx) + 9)),16));
	mainWind->lineEdit_Press_T26V0_raw->setText(QString::number((*(packets+(17*packet2_idx) + 10)),16));
	mainWind->lineEdit_Press_Excit0_raw->setText(QString::number((*(packets+(17*packet2_idx) + 11)),16));
	mainWind->lineEdit_Press_P160V5_raw->setText(QString::number((*(packets+(17*packet2_idx) + 12)),16));
	mainWind->lineEdit_Press_P26V5_raw->setText(QString::number((*(packets+(17*packet2_idx) + 13)),16));
	mainWind->lineEdit_Press_T160V5_raw->setText(QString::number((*(packets+(17*packet2_idx) + 14)),16));
	mainWind->lineEdit_Press_T26V5_raw->setText(QString::number((*(packets+(17*packet2_idx) + 15)),16));
	mainWind->lineEdit_Press_Excit5_raw->setText(QString::number((*(packets+(17*packet2_idx) + 16)),16));

}

void GuiController::processPacket3(unsigned int* packets)
{

	if(!streamingPaused){
		memset(packet3, 0, sizeof(unsigned int)*256);
		memcpy(packet3, (packets + 17*packet3_idx), sizeof(unsigned int)*256);
	}

	mainWind->lineEdit_Cal_Header_raw->setText(QString::number((*(packets+(17*packet3_idx))),16));
	mainWind->lineEdit_Cal_Status_raw->setText(QString::number((*(packets+(17*packet3_idx) + 1)),16));
	mainWind->lineEdit_Cal_N1CA_raw->setText(QString::number((*(packets+(17*packet3_idx) + 2)),16));
	mainWind->lineEdit_Cal_N2CA_raw->setText(QString::number((*(packets+(17*packet3_idx) + 3)),16));
	mainWind->lineEdit_Cal_N1CB_raw->setText(QString::number((*(packets+(17*packet3_idx) + 4)),16));
	mainWind->lineEdit_Cal_N1CB_raw_2->setText(QString::number((*(packets+(17*packet3_idx) + 5)),16));
	mainWind->lineEdit_Cal_N3A_raw->setText(QString::number((*(packets+(17*packet3_idx) + 6)),16));
	mainWind->lineEdit_Cal_N1TA_raw->setText(QString::number((*(packets+(17*packet3_idx) + 7)),16));
	mainWind->lineEdit_Cal_Wdt_raw->setText(QString::number((*(packets+(17*packet3_idx) + 8)),16));
	mainWind->lineEdit_Cal_Unused1_raw->setText(QString::number((*(packets+(17*packet3_idx) + 9)),16));
	mainWind->lineEdit_Cal_ADCSample_raw->setText(QString::number((*(packets+(17*packet3_idx) + 10)),16));
	mainWind->lineEdit_Cal_GearVib_raw->setText(QString::number((*(packets+(17*packet3_idx) + 11)),16));
	mainWind->lineEdit_Cal_EngVib1_raw->setText(QString::number((*(packets+(17*packet3_idx) + 12)),16));
	mainWind->lineEdit_Cal_EngVib2_raw->setText(QString::number((*(packets+(17*packet3_idx) + 13)),16));
	mainWind->lineEdit_Cal_OPRDet_raw->setText(QString::number((*(packets+(17*packet3_idx) + 14)),16));
	mainWind->lineEdit_Cal_CFPGANum_raw->setText(QString::number((*(packets+(17*packet3_idx) + 15)),16));
	mainWind->lineEdit_Cal_Unused2_raw->setText(QString::number((*(packets+(17*packet3_idx) + 16)),16));

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

	setFault(mainWind->lineEdit_cbit0, EXTRACT_BIT(cbitFaultWord, 0));
	setFault(mainWind->lineEdit_cbit1, EXTRACT_BIT(cbitFaultWord, 1));
	setFault(mainWind->lineEdit_cbit2, EXTRACT_BIT(cbitFaultWord, 2));
	setFault(mainWind->lineEdit_cbit3, EXTRACT_BIT(cbitFaultWord, 3));
	setFault(mainWind->lineEdit_cbit4, EXTRACT_BIT(cbitFaultWord, 4));
	setFault(mainWind->lineEdit_cbit5, EXTRACT_BIT(cbitFaultWord, 5));
	setFault(mainWind->lineEdit_cbit6, EXTRACT_BIT(cbitFaultWord, 6));
	setFault(mainWind->lineEdit_cbit7, EXTRACT_BIT(cbitFaultWord, 7));
	setFault(mainWind->lineEdit_cbit8, EXTRACT_BIT(cbitFaultWord, 8));
	setFault(mainWind->lineEdit_cbit9, EXTRACT_BIT(cbitFaultWord, 9));
	setFault(mainWind->lineEdit_cbit10, EXTRACT_BIT(cbitFaultWord, 10));
	setFault(mainWind->lineEdit_cbit11, EXTRACT_BIT(cbitFaultWord, 11));
	setFault(mainWind->lineEdit_cbit12, EXTRACT_BIT(cbitFaultWord, 12));
	setFault(mainWind->lineEdit_cbit13, EXTRACT_BIT(cbitFaultWord, 13));
	setFault(mainWind->lineEdit_cbit14, EXTRACT_BIT(cbitFaultWord, 14));
	setFault(mainWind->lineEdit_cbit15, EXTRACT_BIT(cbitFaultWord, 15));
	setFault(mainWind->lineEdit_cbit16, EXTRACT_BIT(cbitFaultWord, 16));
	setFault(mainWind->lineEdit_cbit17, EXTRACT_BIT(cbitFaultWord, 17));
	setFault(mainWind->lineEdit_cbit18, EXTRACT_BIT(cbitFaultWord, 18));
	setFault(mainWind->lineEdit_cbit19, EXTRACT_BIT(cbitFaultWord, 19));
	setFault(mainWind->lineEdit_cbit20, EXTRACT_BIT(cbitFaultWord, 20));
	setFault(mainWind->lineEdit_cbit21, EXTRACT_BIT(cbitFaultWord, 21));
	setFault(mainWind->lineEdit_cbit22, EXTRACT_BIT(cbitFaultWord, 22));
	setFault(mainWind->lineEdit_cbit23, EXTRACT_BIT(cbitFaultWord, 23));
	setFault(mainWind->lineEdit_cbit24, EXTRACT_BIT(cbitFaultWord, 24));
	setFault(mainWind->lineEdit_cbit25, EXTRACT_BIT(cbitFaultWord, 25));
	setFault(mainWind->lineEdit_cbit26, EXTRACT_BIT(cbitFaultWord, 26));
	setFault(mainWind->lineEdit_cbit27, EXTRACT_BIT(cbitFaultWord, 27));
	setFault(mainWind->lineEdit_cbit28, EXTRACT_BIT(cbitFaultWord, 28));
	setFault(mainWind->lineEdit_cbit29, EXTRACT_BIT(cbitFaultWord, 29));
	setFault(mainWind->lineEdit_cbit30, EXTRACT_BIT(cbitFaultWord, 30));
	setFault(mainWind->lineEdit_cbit31, EXTRACT_BIT(cbitFaultWord, 31));

	setFault(mainWind->lineEdit_pbit0, EXTRACT_BIT(pbitFaultWord, 0));
	setFault(mainWind->lineEdit_pbit1, EXTRACT_BIT(pbitFaultWord, 1));
	setFault(mainWind->lineEdit_pbit2, EXTRACT_BIT(pbitFaultWord, 2));
	setFault(mainWind->lineEdit_pbit3, EXTRACT_BIT(pbitFaultWord, 3));
	setFault(mainWind->lineEdit_pbit4, EXTRACT_BIT(pbitFaultWord, 4));
	setFault(mainWind->lineEdit_pbit5, EXTRACT_BIT(pbitFaultWord, 5));
	setFault(mainWind->lineEdit_pbit6, EXTRACT_BIT(pbitFaultWord, 6));
	setFault(mainWind->lineEdit_pbit7, EXTRACT_BIT(pbitFaultWord, 7));
	setFault(mainWind->lineEdit_pbit8, EXTRACT_BIT(pbitFaultWord, 8));
	setFault(mainWind->lineEdit_pbit9, EXTRACT_BIT(pbitFaultWord, 9));
	setFault(mainWind->lineEdit_pbit10, EXTRACT_BIT(pbitFaultWord, 10));
	setFault(mainWind->lineEdit_pbit11, EXTRACT_BIT(pbitFaultWord, 11));
	setFault(mainWind->lineEdit_pbit12, EXTRACT_BIT(pbitFaultWord, 12));
	setFault(mainWind->lineEdit_pbit13, EXTRACT_BIT(pbitFaultWord, 13));
	setFault(mainWind->lineEdit_pbit14, EXTRACT_BIT(pbitFaultWord, 14));
	setFault(mainWind->lineEdit_pbit15, EXTRACT_BIT(pbitFaultWord, 15));
	setFault(mainWind->lineEdit_pbit16, EXTRACT_BIT(pbitFaultWord, 16));
	setFault(mainWind->lineEdit_pbit17, EXTRACT_BIT(pbitFaultWord, 17));
	setFault(mainWind->lineEdit_pbit18, EXTRACT_BIT(pbitFaultWord, 18));
	setFault(mainWind->lineEdit_pbit19, EXTRACT_BIT(pbitFaultWord, 19));
	setFault(mainWind->lineEdit_pbit20, EXTRACT_BIT(pbitFaultWord, 20));
	setFault(mainWind->lineEdit_pbit21, EXTRACT_BIT(pbitFaultWord, 21));
	setFault(mainWind->lineEdit_pbit22, EXTRACT_BIT(pbitFaultWord, 22));
	setFault(mainWind->lineEdit_pbit23, EXTRACT_BIT(pbitFaultWord, 23));
	setFault(mainWind->lineEdit_pbit24, EXTRACT_BIT(pbitFaultWord, 24));
	setFault(mainWind->lineEdit_pbit25, EXTRACT_BIT(pbitFaultWord, 25));
	setFault(mainWind->lineEdit_pbit26, EXTRACT_BIT(pbitFaultWord, 26));
	setFault(mainWind->lineEdit_pbit27, EXTRACT_BIT(pbitFaultWord, 27));
	setFault(mainWind->lineEdit_pbit28, EXTRACT_BIT(pbitFaultWord, 28));
	setFault(mainWind->lineEdit_pbit29, EXTRACT_BIT(pbitFaultWord, 29));
	setFault(mainWind->lineEdit_pbit30, EXTRACT_BIT(pbitFaultWord, 30));
	setFault(mainWind->lineEdit_pbit31, EXTRACT_BIT(pbitFaultWord, 31));

	setFault(mainWind->lineEdit_nvmFault_0, EXTRACT_BIT(nvmFaultWord, 0));
	setFault(mainWind->lineEdit_nvmFault_1, EXTRACT_BIT(nvmFaultWord, 1));
	setFault(mainWind->lineEdit_nvmFault_2, EXTRACT_BIT(nvmFaultWord, 2));
	setFault(mainWind->lineEdit_nvmFault_3, EXTRACT_BIT(nvmFaultWord, 3));
	setFault(mainWind->lineEdit_nvmFault_4, EXTRACT_BIT(nvmFaultWord, 4));
	setFault(mainWind->lineEdit_nvmFault_5, EXTRACT_BIT(nvmFaultWord, 5));
	setFault(mainWind->lineEdit_nvmFault_6, EXTRACT_BIT(nvmFaultWord, 6));
	setFault(mainWind->lineEdit_nvmFault_7, EXTRACT_BIT(nvmFaultWord, 7));
	setFault(mainWind->lineEdit_nvmFault_8, EXTRACT_BIT(nvmFaultWord, 8));
	setFault(mainWind->lineEdit_nvmFault_9, EXTRACT_BIT(nvmFaultWord, 9));
	setFault(mainWind->lineEdit_nvmFault_10, EXTRACT_BIT(nvmFaultWord, 10));
	setFault(mainWind->lineEdit_nvmFault_11, EXTRACT_BIT(nvmFaultWord, 11));
	setFault(mainWind->lineEdit_nvmFault_12, EXTRACT_BIT(nvmFaultWord, 12));
	setFault(mainWind->lineEdit_nvmFault_13, EXTRACT_BIT(nvmFaultWord, 13));
	setFault(mainWind->lineEdit_nvmFault_14, EXTRACT_BIT(nvmFaultWord, 14));
	setFault(mainWind->lineEdit_nvmFault_15, EXTRACT_BIT(nvmFaultWord, 15));
	setFault(mainWind->lineEdit_nvmFault_16, EXTRACT_BIT(nvmFaultWord, 16));
	setFault(mainWind->lineEdit_nvmFault_17, EXTRACT_BIT(nvmFaultWord, 17));
	setFault(mainWind->lineEdit_nvmFault_18, EXTRACT_BIT(nvmFaultWord, 18));
	setFault(mainWind->lineEdit_nvmFault_19, EXTRACT_BIT(nvmFaultWord, 19));
	setFault(mainWind->lineEdit_nvmFault_20, EXTRACT_BIT(nvmFaultWord, 20));
	setFault(mainWind->lineEdit_nvmFault_21, EXTRACT_BIT(nvmFaultWord, 21));
	setFault(mainWind->lineEdit_nvmFault_22, EXTRACT_BIT(nvmFaultWord, 22));
	setFault(mainWind->lineEdit_nvmFault_23, EXTRACT_BIT(nvmFaultWord, 23));
	setFault(mainWind->lineEdit_nvmFault_24, EXTRACT_BIT(nvmFaultWord, 24));
	setFault(mainWind->lineEdit_nvmFault_25, EXTRACT_BIT(nvmFaultWord, 25));
	setFault(mainWind->lineEdit_nvmFault_26, EXTRACT_BIT(nvmFaultWord, 26));
	setFault(mainWind->lineEdit_nvmFault_27, EXTRACT_BIT(nvmFaultWord, 27));
	setFault(mainWind->lineEdit_nvmFault_28, EXTRACT_BIT(nvmFaultWord, 28));
	setFault(mainWind->lineEdit_nvmFault_29, EXTRACT_BIT(nvmFaultWord, 29));
	setFault(mainWind->lineEdit_nvmFault_30, EXTRACT_BIT(nvmFaultWord, 30));
	setFault(mainWind->lineEdit_nvmFault_31, EXTRACT_BIT(nvmFaultWord, 31));

	setFault(mainWind->lineEdit_bootFault_0, EXTRACT_BIT(bootFaultWord, 0));
	setFault(mainWind->lineEdit_bootFault_1, EXTRACT_BIT(bootFaultWord, 1));
	setFault(mainWind->lineEdit_bootFault_2, EXTRACT_BIT(bootFaultWord, 2));
	setFault(mainWind->lineEdit_bootFault_3, EXTRACT_BIT(bootFaultWord, 3));
	setFault(mainWind->lineEdit_bootFault_4, EXTRACT_BIT(bootFaultWord, 4));
	setFault(mainWind->lineEdit_bootFault_5, EXTRACT_BIT(bootFaultWord, 5));
	setFault(mainWind->lineEdit_bootFault_6, EXTRACT_BIT(bootFaultWord, 6));
	setFault(mainWind->lineEdit_bootFault_7, EXTRACT_BIT(bootFaultWord, 7));
	setFault(mainWind->lineEdit_bootFault_8, EXTRACT_BIT(bootFaultWord, 8));
	setFault(mainWind->lineEdit_bootFault_9, EXTRACT_BIT(bootFaultWord, 9));
	setFault(mainWind->lineEdit_bootFault_10, EXTRACT_BIT(bootFaultWord, 10));
	setFault(mainWind->lineEdit_bootFault_11, EXTRACT_BIT(bootFaultWord, 11));
	setFault(mainWind->lineEdit_bootFault_12, EXTRACT_BIT(bootFaultWord, 12));
	setFault(mainWind->lineEdit_bootFault_13, EXTRACT_BIT(bootFaultWord, 13));
	setFault(mainWind->lineEdit_bootFault_14, EXTRACT_BIT(bootFaultWord, 14));
	setFault(mainWind->lineEdit_bootFault_15, EXTRACT_BIT(bootFaultWord, 15));
	setFault(mainWind->lineEdit_bootFault_16, EXTRACT_BIT(bootFaultWord, 16));
	setFault(mainWind->lineEdit_bootFault_17, EXTRACT_BIT(bootFaultWord, 17));
	setFault(mainWind->lineEdit_bootFault_18, EXTRACT_BIT(bootFaultWord, 18));
	setFault(mainWind->lineEdit_bootFault_19, EXTRACT_BIT(bootFaultWord, 19));
	setFault(mainWind->lineEdit_bootFault_20, EXTRACT_BIT(bootFaultWord, 20));
	setFault(mainWind->lineEdit_bootFault_21, EXTRACT_BIT(bootFaultWord, 21));
	setFault(mainWind->lineEdit_bootFault_22, EXTRACT_BIT(bootFaultWord, 22));
	setFault(mainWind->lineEdit_bootFault_23, EXTRACT_BIT(bootFaultWord, 23));
	setFault(mainWind->lineEdit_bootFault_24, EXTRACT_BIT(bootFaultWord, 24));
	setFault(mainWind->lineEdit_bootFault_25, EXTRACT_BIT(bootFaultWord, 25));
	setFault(mainWind->lineEdit_bootFault_26, EXTRACT_BIT(bootFaultWord, 26));
	setFault(mainWind->lineEdit_bootFault_27, EXTRACT_BIT(bootFaultWord, 27));
	setFault(mainWind->lineEdit_bootFault_28, EXTRACT_BIT(bootFaultWord, 28));
	setFault(mainWind->lineEdit_bootFault_29, EXTRACT_BIT(bootFaultWord, 29));
	setFault(mainWind->lineEdit_bootFault_30, EXTRACT_BIT(bootFaultWord, 30));
	setFault(mainWind->lineEdit_bootFault_31, EXTRACT_BIT(bootFaultWord, 31));

	setFault(mainWind->lineEdit_McBSPFault_0, EXTRACT_BIT(mcbspFaultWord, 0));
	setFault(mainWind->lineEdit_McBSPFault_1, EXTRACT_BIT(mcbspFaultWord, 1));
	setFault(mainWind->lineEdit_McBSPFault_2, EXTRACT_BIT(mcbspFaultWord, 2));
	setFault(mainWind->lineEdit_McBSPFault_3, EXTRACT_BIT(mcbspFaultWord, 3));
	setFault(mainWind->lineEdit_McBSPFault_4, EXTRACT_BIT(mcbspFaultWord, 4));
	setFault(mainWind->lineEdit_McBSPFault_5, EXTRACT_BIT(mcbspFaultWord, 5));
	setFault(mainWind->lineEdit_McBSPFault_6, EXTRACT_BIT(mcbspFaultWord, 6));
	setFault(mainWind->lineEdit_McBSPFault_7, EXTRACT_BIT(mcbspFaultWord, 7));
	setFault(mainWind->lineEdit_McBSPFault_8, EXTRACT_BIT(mcbspFaultWord, 8));
	setFault(mainWind->lineEdit_McBSPFault_9, EXTRACT_BIT(mcbspFaultWord, 9));
	setFault(mainWind->lineEdit_McBSPFault_10, EXTRACT_BIT(mcbspFaultWord, 10));
	setFault(mainWind->lineEdit_McBSPFault_11, EXTRACT_BIT(mcbspFaultWord, 11));
	setFault(mainWind->lineEdit_McBSPFault_12, EXTRACT_BIT(mcbspFaultWord, 12));
	setFault(mainWind->lineEdit_McBSPFault_13, EXTRACT_BIT(mcbspFaultWord, 13));
	setFault(mainWind->lineEdit_McBSPFault_14, EXTRACT_BIT(mcbspFaultWord, 14));
	setFault(mainWind->lineEdit_McBSPFault_15, EXTRACT_BIT(mcbspFaultWord, 15));
	setFault(mainWind->lineEdit_McBSPFault_16, EXTRACT_BIT(mcbspFaultWord, 16));
	setFault(mainWind->lineEdit_McBSPFault_17, EXTRACT_BIT(mcbspFaultWord, 17));
	setFault(mainWind->lineEdit_McBSPFault_18, EXTRACT_BIT(mcbspFaultWord, 18));
	setFault(mainWind->lineEdit_McBSPFault_19, EXTRACT_BIT(mcbspFaultWord, 19));
	setFault(mainWind->lineEdit_McBSPFault_20, EXTRACT_BIT(mcbspFaultWord, 20));
	setFault(mainWind->lineEdit_McBSPFault_21, EXTRACT_BIT(mcbspFaultWord, 21));
	setFault(mainWind->lineEdit_McBSPFault_22, EXTRACT_BIT(mcbspFaultWord, 22));
	setFault(mainWind->lineEdit_McBSPFault_23, EXTRACT_BIT(mcbspFaultWord, 23));
	setFault(mainWind->lineEdit_McBSPFault_24, EXTRACT_BIT(mcbspFaultWord, 24));
	setFault(mainWind->lineEdit_McBSPFault_25, EXTRACT_BIT(mcbspFaultWord, 25));
	setFault(mainWind->lineEdit_McBSPFault_26, EXTRACT_BIT(mcbspFaultWord, 26));
	setFault(mainWind->lineEdit_McBSPFault_27, EXTRACT_BIT(mcbspFaultWord, 27));
	setFault(mainWind->lineEdit_McBSPFault_28, EXTRACT_BIT(mcbspFaultWord, 28));
	setFault(mainWind->lineEdit_McBSPFault_29, EXTRACT_BIT(mcbspFaultWord, 29));
	setFault(mainWind->lineEdit_McBSPFault_30, EXTRACT_BIT(mcbspFaultWord, 30));
	setFault(mainWind->lineEdit_McBSPFault_31, EXTRACT_BIT(mcbspFaultWord, 31));

	setFault(mainWind->lineEdit_RS422Fault_0, EXTRACT_BIT(rs422FaultWord, 0));
	setFault(mainWind->lineEdit_RS422Fault_1, EXTRACT_BIT(rs422FaultWord, 1));
	setFault(mainWind->lineEdit_RS422Fault_2, EXTRACT_BIT(rs422FaultWord, 2));
	setFault(mainWind->lineEdit_RS422Fault_3, EXTRACT_BIT(rs422FaultWord, 3));
	setFault(mainWind->lineEdit_RS422Fault_4, EXTRACT_BIT(rs422FaultWord, 4));
	setFault(mainWind->lineEdit_RS422Fault_5, EXTRACT_BIT(rs422FaultWord, 5));
	setFault(mainWind->lineEdit_RS422Fault_6, EXTRACT_BIT(rs422FaultWord, 6));
	setFault(mainWind->lineEdit_RS422Fault_7, EXTRACT_BIT(rs422FaultWord, 7));
	setFault(mainWind->lineEdit_RS422Fault_8, EXTRACT_BIT(rs422FaultWord, 8));
	setFault(mainWind->lineEdit_RS422Fault_9, EXTRACT_BIT(rs422FaultWord, 9));
	setFault(mainWind->lineEdit_RS422Fault_10, EXTRACT_BIT(rs422FaultWord, 10));
	setFault(mainWind->lineEdit_RS422Fault_11, EXTRACT_BIT(rs422FaultWord, 11));
	setFault(mainWind->lineEdit_RS422Fault_12, EXTRACT_BIT(rs422FaultWord, 12));
	setFault(mainWind->lineEdit_RS422Fault_13, EXTRACT_BIT(rs422FaultWord, 13));
	setFault(mainWind->lineEdit_RS422Fault_14, EXTRACT_BIT(rs422FaultWord, 14));
	setFault(mainWind->lineEdit_RS422Fault_15, EXTRACT_BIT(rs422FaultWord, 15));
	setFault(mainWind->lineEdit_RS422Fault_16, EXTRACT_BIT(rs422FaultWord, 16));
	setFault(mainWind->lineEdit_RS422Fault_17, EXTRACT_BIT(rs422FaultWord, 17));
	setFault(mainWind->lineEdit_RS422Fault_18, EXTRACT_BIT(rs422FaultWord, 18));
	setFault(mainWind->lineEdit_RS422Fault_19, EXTRACT_BIT(rs422FaultWord, 19));
	setFault(mainWind->lineEdit_RS422Fault_20, EXTRACT_BIT(rs422FaultWord, 20));
	setFault(mainWind->lineEdit_RS422Fault_21, EXTRACT_BIT(rs422FaultWord, 21));
	setFault(mainWind->lineEdit_RS422Fault_22, EXTRACT_BIT(rs422FaultWord, 22));
	setFault(mainWind->lineEdit_RS422Fault_23, EXTRACT_BIT(rs422FaultWord, 23));
	setFault(mainWind->lineEdit_RS422Fault_24, EXTRACT_BIT(rs422FaultWord, 24));
	setFault(mainWind->lineEdit_RS422Fault_25, EXTRACT_BIT(rs422FaultWord, 25));
	setFault(mainWind->lineEdit_RS422Fault_26, EXTRACT_BIT(rs422FaultWord, 26));
	setFault(mainWind->lineEdit_RS422Fault_27, EXTRACT_BIT(rs422FaultWord, 27));
	setFault(mainWind->lineEdit_RS422Fault_28, EXTRACT_BIT(rs422FaultWord, 28));
	setFault(mainWind->lineEdit_RS422Fault_29, EXTRACT_BIT(rs422FaultWord, 29));
	setFault(mainWind->lineEdit_RS422Fault_30, EXTRACT_BIT(rs422FaultWord, 30));
	setFault(mainWind->lineEdit_RS422Fault_31, EXTRACT_BIT(rs422FaultWord, 31));

	setFault(mainWind->lineEdit_ethStatus_0, EXTRACT_BIT(ethernetStatusWord, 0));
	setFault(mainWind->lineEdit_ethStatus_1, EXTRACT_BIT(ethernetStatusWord, 1));
	setFault(mainWind->lineEdit_ethStatus_2, EXTRACT_BIT(ethernetStatusWord, 2));
	setFault(mainWind->lineEdit_ethStatus_3, EXTRACT_BIT(ethernetStatusWord, 3));
	setFault(mainWind->lineEdit_ethStatus_4, EXTRACT_BIT(ethernetStatusWord, 4));
	setFault(mainWind->lineEdit_ethStatus_5, EXTRACT_BIT(ethernetStatusWord, 5));
	setFault(mainWind->lineEdit_ethStatus_6, EXTRACT_BIT(ethernetStatusWord, 6));
	setFault(mainWind->lineEdit_ethStatus_7, EXTRACT_BIT(ethernetStatusWord, 7));
	setFault(mainWind->lineEdit_ethStatus_8, EXTRACT_BIT(ethernetStatusWord, 8));
	setFault(mainWind->lineEdit_ethStatus_9, EXTRACT_BIT(ethernetStatusWord, 9));
	setFault(mainWind->lineEdit_ethStatus_10, EXTRACT_BIT(ethernetStatusWord, 10));
	setFault(mainWind->lineEdit_ethStatus_11, EXTRACT_BIT(ethernetStatusWord, 11));
	setFault(mainWind->lineEdit_ethStatus_12, EXTRACT_BIT(ethernetStatusWord, 12));
	setFault(mainWind->lineEdit_ethStatus_13, EXTRACT_BIT(ethernetStatusWord, 13));
	setFault(mainWind->lineEdit_ethStatus_14, EXTRACT_BIT(ethernetStatusWord, 14));
	setFault(mainWind->lineEdit_ethStatus_15, EXTRACT_BIT(ethernetStatusWord, 15));
	setFault(mainWind->lineEdit_ethStatus_16, EXTRACT_BIT(ethernetStatusWord, 16));
	setFault(mainWind->lineEdit_ethStatus_17, EXTRACT_BIT(ethernetStatusWord, 17));
	setFault(mainWind->lineEdit_ethStatus_18, EXTRACT_BIT(ethernetStatusWord, 18));
	setFault(mainWind->lineEdit_ethStatus_19, EXTRACT_BIT(ethernetStatusWord, 19));
	setFault(mainWind->lineEdit_ethStatus_20, EXTRACT_BIT(ethernetStatusWord, 20));
	setFault(mainWind->lineEdit_ethStatus_21, EXTRACT_BIT(ethernetStatusWord, 21));
	setFault(mainWind->lineEdit_ethStatus_22, EXTRACT_BIT(ethernetStatusWord, 22));
	setFault(mainWind->lineEdit_ethStatus_23, EXTRACT_BIT(ethernetStatusWord, 23));
	setFault(mainWind->lineEdit_ethStatus_24, EXTRACT_BIT(ethernetStatusWord, 24));
	setFault(mainWind->lineEdit_ethStatus_25, EXTRACT_BIT(ethernetStatusWord, 25));
	setFault(mainWind->lineEdit_ethStatus_26, EXTRACT_BIT(ethernetStatusWord, 26));
	setFault(mainWind->lineEdit_ethStatus_27, EXTRACT_BIT(ethernetStatusWord, 27));
	setFault(mainWind->lineEdit_ethStatus_28, EXTRACT_BIT(ethernetStatusWord, 28));
	setFault(mainWind->lineEdit_ethStatus_29, EXTRACT_BIT(ethernetStatusWord, 29));
	setFault(mainWind->lineEdit_ethStatus_30, EXTRACT_BIT(ethernetStatusWord, 30));
	setFault(mainWind->lineEdit_ethStatus_31, EXTRACT_BIT(ethernetStatusWord, 31));

	setFault(mainWind->lineEdit_ethFault_0, EXTRACT_BIT(ethernetFaultWord, 0));
	setFault(mainWind->lineEdit_ethFault_1, EXTRACT_BIT(ethernetFaultWord, 1));
	setFault(mainWind->lineEdit_ethFault_2, EXTRACT_BIT(ethernetFaultWord, 2));
	setFault(mainWind->lineEdit_ethFault_3, EXTRACT_BIT(ethernetFaultWord, 3));
	setFault(mainWind->lineEdit_ethFault_4, EXTRACT_BIT(ethernetFaultWord, 4));
	setFault(mainWind->lineEdit_ethFault_5, EXTRACT_BIT(ethernetFaultWord, 5));
	setFault(mainWind->lineEdit_ethFault_6, EXTRACT_BIT(ethernetFaultWord, 6));
	setFault(mainWind->lineEdit_ethFault_7, EXTRACT_BIT(ethernetFaultWord, 7));
	setFault(mainWind->lineEdit_ethFault_8, EXTRACT_BIT(ethernetFaultWord, 8));
	setFault(mainWind->lineEdit_ethFault_9, EXTRACT_BIT(ethernetFaultWord, 9));
	setFault(mainWind->lineEdit_ethFault_10, EXTRACT_BIT(ethernetFaultWord, 10));
	setFault(mainWind->lineEdit_ethFault_11, EXTRACT_BIT(ethernetFaultWord, 11));
	setFault(mainWind->lineEdit_ethFault_12, EXTRACT_BIT(ethernetFaultWord, 12));
	setFault(mainWind->lineEdit_ethFault_13, EXTRACT_BIT(ethernetFaultWord, 13));
	setFault(mainWind->lineEdit_ethFault_14, EXTRACT_BIT(ethernetFaultWord, 14));
	setFault(mainWind->lineEdit_ethFault_15, EXTRACT_BIT(ethernetFaultWord, 15));
	setFault(mainWind->lineEdit_ethFault_16, EXTRACT_BIT(ethernetFaultWord, 16));
	setFault(mainWind->lineEdit_ethFault_17, EXTRACT_BIT(ethernetFaultWord, 17));
	setFault(mainWind->lineEdit_ethFault_18, EXTRACT_BIT(ethernetFaultWord, 18));
	setFault(mainWind->lineEdit_ethFault_19, EXTRACT_BIT(ethernetFaultWord, 19));
	setFault(mainWind->lineEdit_ethFault_20, EXTRACT_BIT(ethernetFaultWord, 20));
	setFault(mainWind->lineEdit_ethFault_21, EXTRACT_BIT(ethernetFaultWord, 21));
	setFault(mainWind->lineEdit_ethFault_22, EXTRACT_BIT(ethernetFaultWord, 22));
	setFault(mainWind->lineEdit_ethFault_23, EXTRACT_BIT(ethernetFaultWord, 23));
	setFault(mainWind->lineEdit_ethFault_24, EXTRACT_BIT(ethernetFaultWord, 24));
	setFault(mainWind->lineEdit_ethFault_25, EXTRACT_BIT(ethernetFaultWord, 25));
	setFault(mainWind->lineEdit_ethFault_26, EXTRACT_BIT(ethernetFaultWord, 26));
	setFault(mainWind->lineEdit_ethFault_27, EXTRACT_BIT(ethernetFaultWord, 27));
	setFault(mainWind->lineEdit_ethFault_28, EXTRACT_BIT(ethernetFaultWord, 28));
	setFault(mainWind->lineEdit_ethFault_29, EXTRACT_BIT(ethernetFaultWord, 29));
	setFault(mainWind->lineEdit_ethFault_30, EXTRACT_BIT(ethernetFaultWord, 30));
	setFault(mainWind->lineEdit_ethFault_31, EXTRACT_BIT(ethernetFaultWord, 31));

	setFault(mainWind->lineEdit_ptm0, EXTRACT_BIT(ptmFaultWord, 0));
	setFault(mainWind->lineEdit_ptm1, EXTRACT_BIT(ptmFaultWord, 1));
	setFault(mainWind->lineEdit_ptm2, EXTRACT_BIT(ptmFaultWord, 2));
	setFault(mainWind->lineEdit_ptm3, EXTRACT_BIT(ptmFaultWord, 3));
	setFault(mainWind->lineEdit_ptm4, EXTRACT_BIT(ptmFaultWord, 4));
	setFault(mainWind->lineEdit_ptm5, EXTRACT_BIT(ptmFaultWord, 5));
	setFault(mainWind->lineEdit_ptm6, EXTRACT_BIT(ptmFaultWord, 6));
	setFault(mainWind->lineEdit_ptm7, EXTRACT_BIT(ptmFaultWord, 7));
	setFault(mainWind->lineEdit_ptm8, EXTRACT_BIT(ptmFaultWord, 8));
	setFault(mainWind->lineEdit_ptm9, EXTRACT_BIT(ptmFaultWord, 9));
	setFault(mainWind->lineEdit_ptm10, EXTRACT_BIT(ptmFaultWord, 10));
	setFault(mainWind->lineEdit_ptm11, EXTRACT_BIT(ptmFaultWord, 11));
	setFault(mainWind->lineEdit_ptm12, EXTRACT_BIT(ptmFaultWord, 12));
	setFault(mainWind->lineEdit_ptm13, EXTRACT_BIT(ptmFaultWord, 13));
	setFault(mainWind->lineEdit_ptm14, EXTRACT_BIT(ptmFaultWord, 14));
	setFault(mainWind->lineEdit_ptm15, EXTRACT_BIT(ptmFaultWord, 15));
	setFault(mainWind->lineEdit_ptm16, EXTRACT_BIT(ptmFaultWord, 16));
	setFault(mainWind->lineEdit_ptm17, EXTRACT_BIT(ptmFaultWord, 17));
	setFault(mainWind->lineEdit_ptm18, EXTRACT_BIT(ptmFaultWord, 18));
	setFault(mainWind->lineEdit_ptm19, EXTRACT_BIT(ptmFaultWord, 19));
	setFault(mainWind->lineEdit_ptm20, EXTRACT_BIT(ptmFaultWord, 20));
	setFault(mainWind->lineEdit_ptm21, EXTRACT_BIT(ptmFaultWord, 21));
	setFault(mainWind->lineEdit_ptm22, EXTRACT_BIT(ptmFaultWord, 22));
	setFault(mainWind->lineEdit_ptm23, EXTRACT_BIT(ptmFaultWord, 23));
	setFault(mainWind->lineEdit_ptm24, EXTRACT_BIT(ptmFaultWord, 24));
	setFault(mainWind->lineEdit_ptm25, EXTRACT_BIT(ptmFaultWord, 25));
	setFault(mainWind->lineEdit_ptm26, EXTRACT_BIT(ptmFaultWord, 26));
	setFault(mainWind->lineEdit_ptm27, EXTRACT_BIT(ptmFaultWord, 27));
	setFault(mainWind->lineEdit_ptm28, EXTRACT_BIT(ptmFaultWord, 28));
	setFault(mainWind->lineEdit_ptm29, EXTRACT_BIT(ptmFaultWord, 29));
	setFault(mainWind->lineEdit_ptm30, EXTRACT_BIT(ptmFaultWord, 30));
	setFault(mainWind->lineEdit_ptm31, EXTRACT_BIT(ptmFaultWord, 31));
}

void GuiController::setFault(QLineEdit* lineEdit, unsigned int fault){
	lineEdit->setText(QString::number(fault));
	fault ?  lineEdit->setStyleSheet( QString( "background-color: red")) : lineEdit->setStyleSheet( QString( "background-color: white"));

}
//TODO receive signal from EthernetTransmit and increment tx counter
//TODO increment rx counter when packet is received

void GuiController::processLoopback(unsigned int* rxBuffer)
{
	testWind->label_loopback_tx_packets->setText(QString::number(ewriter->getTxCount()));
	testWind->label_loopback_rx_packets->setText(QString::number(loopback_rx_count));
}

GuiController::~GuiController()
{
	free(packet1);
	free(packet2);
	free(packet3);
}
