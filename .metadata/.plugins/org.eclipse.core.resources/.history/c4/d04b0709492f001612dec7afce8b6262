#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>

#include "buffer.h"
#include "utils.h"
#include "process_ethernet.h"
#include "global.h"
#include "guicontroller.h"

#define UI_THREADS	(1)
#define INFR_THREADS	(1)
#define NUM_BUFFS	(18)
#define NUM_PORTS	(NUM_BUFFS)

struct global ethernetGlobalData;

ringBuffer_t Ethernet_Buffs[NUM_BUFFS];
ringBuffer_t RS422_Buffs[NUM_BUFFS];
ringBuffer_t ARINC_Buffs[NUM_BUFFS];


u_int32_t signal[3] = {0xDEADBEAF, 0xBAADF00D, 0xDADABACA};

//using namespace Ui;

void* ui_thread(void* args);
void delay(int count);

int main( int argc, char* argv[] )
{
	int i;
	int* threadRetVal = NULL;
	u_int32_t ports[18] = {
			22024,
			32024,
			22029,
			32029,
			22021,
			32021,
			22027,
			32027,
			22028,
			32028,
			22025,
			32025,
			22026,
			32026,
			22023,
			32023,
			22022,
			32022};


	for(i=0; i<NUM_BUFFS; i++){
		initBuffer(&Ethernet_Buffs[i]);
	}

	pthread_t threads[UI_THREADS + INFR_THREADS];
	pthread_attr_t attrs[UI_THREADS + INFR_THREADS];
	void* ret_status;

	ethernetGlobalData.rxBuff = Ethernet_Buffs;
	ethernetGlobalData.txBuff = NULL;
	ethernetGlobalData.exitStatus = (void*)threadRetVal;

	/***********************************
	 * Start GUI thread
	 ************************************/

	UIThreadArgs_t UIThreadData;
	UIThreadData.argc = argc;
	UIThreadData.argv = argv;
	UIThreadData.global_data = &ethernetGlobalData;

	if(pthread_attr_init(attrs)){
		print_error("attr_init()");
	}
	if(pthread_create(threads, attrs, ui_thread, &UIThreadData)){
		print_error("pthread_create");
		exit(EXIT_FAILURE);
	}

	/***********************************
	 * Start Ethernet thread
	 ************************************/
	/*ethernetGlobalData.rxBuff = Ethernet_Buffs;
	ethernetGlobalData.txBuff = NULL;
	ethernetGlobalData.exitStatus = (void*)threadRetVal;*/

	EthernetThreadArgs_t ethernetThreadData;

	ethernetThreadData.global_data = &ethernetGlobalData;
	ethernetThreadData.ports = ports;
	ethernetThreadData.numPorts = NUM_PORTS;


	if(pthread_attr_init(attrs+1)){
		print_error("attr_init()");
	}
	if(pthread_create(threads+1, attrs+1, ethernet_thread, &ethernetThreadData)){
		print_error("pthread_create");
		exit(EXIT_FAILURE);
	}

	int j;
	for(j=0; j < 2; j++){
		if(pthread_join(threads[j], &ret_status)){
			fprintf(stderr, "pthread_join: unable to join threads");
		}

		printf("some thread joined \n");

	}

	/***********************************
	 * Start RS422 thread
	 ************************************/

	/***********************************
	 * Start ARINC thread
	 ************************************/


	return 0;
}

void* ui_thread(void* params){

	UIThreadArgs_t* args = (UIThreadArgs_t*)params;


	/*QApplication app( args->argc, args->argv );

	QMainWindow* main = new QMainWindow();

	Ui_MainWindow *mainWind = new Ui_MainWindow;
	app.setActiveWindow( main );
	mainWind->setupUi(main);
	main->show();
	app.exec();*/
	GuiController* guiCntrl = new GuiController(args);
	guiCntrl->showGui();

	return NULL;

}

void delay(int count){
	int i = count;

	while(i > 0)
		i--;

}
