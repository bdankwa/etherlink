#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H
#include <QObject>
#include <stdlib.h>
#include "etherlinkMainWindow.h"
#include "EthernetReader.h"
#include "global.h"

#define NUM_BUFFS	(18)
#define NUM_PORTS	(NUM_BUFFS)

class GuiController : public QObject
{
	Q_OBJECT
public:
    GuiController(UIThreadArgs_t* threadParams);
    void showGui();

signals:
	//void streamPacket1();
private slots:
	void streamPacket(unsigned int* packet, unsigned int num, int portIdx);
	void readPacketNumber(QString);
    void startStream();
    void pauseStream();
    void stopStream();

private:
    Ui_MainWindow *mainWind;
    UIThreadArgs_t* args;
    EthernetReader* ereader;
    void processPacket1(unsigned int* packets);
    void processPacket2(unsigned int* packets);
    void processPacket3(unsigned int* packets);
    void processPacket4(unsigned int* packets);
    void processPacket5(unsigned int* packets);
    void processFaults(unsigned int* faults);
};

#endif // GUICONTROLLER_H
