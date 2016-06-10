#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H
#include <QObject>
#include <stdlib.h>
#include "clinkMainWindow.h"
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
	void streamPacket(unsigned int* packet, unsigned int num);
	void readPacketNumber(QString);
    void startStream();
    //void pauseStream();
    //void stopStream();

private:
    Ui_MainWindow *mainWind;
    UIThreadArgs_t* args;
    EthernetReader* ereader;
};

#endif // GUICONTROLLER_H
