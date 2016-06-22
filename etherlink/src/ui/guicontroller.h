#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H
#include <QObject>
#include <QMessageBox>
#include <stdlib.h>
#include "etherlinkMainWindow.h"
#include "testWindow.h"
#include "EthernetReader.h"
#include "EthernetTransmit.h"
#include "A664Filter.h"
#include "global.h"

#define NUM_BUFFS	(7)
#define NUM_PORTS	(NUM_BUFFS)

class GuiController : public QObject
{
	Q_OBJECT
public:
    GuiController(UIThreadArgs_t* threadParams);
    ~GuiController();
    void showGui();

signals:
	//void streamPacket1();
private slots:
	void streamPacket(unsigned int* packet, unsigned int num, int portIdx);
	void readPacket1Index(QString);
	void readPacket2Index(QString);
	void readPacket3Index(QString);
	void readPacket1RefreshRate(QString);
	void readPacket2RefreshRate(QString);
	void readPacket3RefreshRate(QString);
	void decrementPacket1Idx();
	void decrementPacket2Idx();
	void decrementPacket3Idx();
	void incrementPacket1Idx();
	void incrementPacket2Idx();
	void incrementPacket3Idx();
    void startStream();
    void pauseStream();
    void stopStream();
    void showTestWindow();
    void showStreamWindow();
    void startTesting();

private:
    Ui_MainWindow *mainWind;
    QMainWindow* main;
    Ui_Test *testWind;
    QMainWindow* test;
    UIThreadArgs_t* args;
    EthernetReader* ereader;
    EthernetTransmit* ewriter;
    A664Filter* a664Filter;
    QLabel* connectionLabel;
    QPalette* redPalette;
    QPalette* greenPalette;
    int connectionStatus;
    bool streamingPaused;
    bool testing;
    unsigned int* packet1;
    unsigned int* packet2;
    unsigned int* packet3;
    u_int32_t packet1_idx;
    u_int32_t packet2_idx;
    u_int32_t packet3_idx;
    u_int32_t packet1_refreshRate;
    u_int32_t packet2_refreshRate;
    u_int32_t packet3_refreshRate;

    QMessageBox* messageBox;

    void processPacket1(unsigned int* packets);
    void processPacket2(unsigned int* packets);
    void processPacket3(unsigned int* packets);
    void processPacket4(unsigned int* packets);
    void processPacket5(unsigned int* packets);
    void processFaults(unsigned int* faults);
    void processLoopback(unsigned int* buffer);
};

#endif // GUICONTROLLER_H
