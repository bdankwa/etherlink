/* ************************************************************************//**
 * US Export Controlled EAR - Not Protectively Marked
 * ****************************************************************************
 *
 * Copyright 2016 Rolls-Royce Controls and Data Services Inc.
 * 
 * @file OS_AS_API.h
 *
 * @author Randy Turner <randall.turner@controlsdata.com>
 *
 * @version 2016-05-23-105741
 * 
 * @addtogroup
 *
 * @brief OS API for the VisiumSence Platform OS (Level C):
 *
 * This file Contains most of the functions for the AS to access
 * OS provided functionality and resources on the level C portion of the
 * VisiumSense platform. API functions defined in other files are documented in other
 * sections of this document.
 */
// ****************************************************************************


/****************************************************************************
* @copyright { © 2015 Rolls-Royce Controls and Data Services Inc. The
* information in this document is the property of Rolls-Royce Controls and Data
* Services Inc. and may not be copied or communicated to a third party, or used
* for any purpose other than that for which it is supplied without the express
* written consent of Rolls-Royce Controls and Data Services Inc.
* Destination Control Statement for BR700 and BR7000
* US Export Controlled-EAR
* These commodities, technology or software are subject to the Export
* Administration Regulations. Diversion contrary to U.S. Law prohibited.
* NLR per 9E991 }
*/
// ****************************************************************************

/****************************************************************************
* @page Background
*
* # Introduction
*
* This document outlines the Application Programming Interface (API) for the
* Application Software (AS) to access Operating System (OS) provided
* functionality and resources on the Level C portion of the VisiumSense Platform.
*
* Since the AS will be developed using Simulink, all data transferred between OS
* and AS shall be contained in 32-bit data items (because this is the smallest
* granularity supported by Simulink).
*
* ## Functional Allocation to Operating Software
*
* For the purposes of the Level C portion of the VisiumSense Platform, the OS
* will provide basic access to the hardware and computing resources provided by
* the platform.  Where possible, this will be done in a hardware agnostic
* method to allow the hardware to be updated without affecting the API, and
* therefore the AS.
*
* Functionality required to provide the access listed in the paragraph above
* will be allocated to the OS.
*
* This allocation will be further defined in the software architecture,
* once it is completed.
*
* ## Functional Allocation to the Application System Software
*
* For the purposes of the Level C portion of the VisiumSense Platform, all
* functionality not required to be allocated to the OS (discussed in
* section 1.1) will be allocated to the AS.
*
* This allocation will be further defined in the software architecture,
* once it is completed.  However, additional decomposition of AS functionality
* will not be discussed in this document since that is the exclusive
* responsibility of the AS designers.
*
* ### Scheduling
*
* The Level C partition of the VisiumSense Platform shall employ simple software
* tasking to provide prioritized execution of High Priority, Main Priority and
* Background threads (the threads listed in decreasing order of priority).
* Execution of individual tasks within these threads will be controlled by their
* relative position within the call chain for their thread. Individual threads or
* tasks shall not be assigned adjustable priorities. The OS shall use timer and
* hardware interrupts to provide all required scheduling.
*
* #### Scheduling Description
*
* For the Level C partition of the VisiumSense Platform, a cooperative
* foreground-background scheduler with timer and hardware-based interrupts
* will be used.  All scheduling will be statically defined at compile-time and
* will not be modifiable at run-time.
*
* In this scheduling approach, there are not per-task priorities instead
* there is a set of classes of tasks with simple and pre-defined rules
* regarding which class can interrupt which others.  These classes of threads
* are defined in Section [Ref to Periodic Tasks].
*
* Task lockout where a task is constantly interrupted by another task and is
* unable to complete is managed by mandating a per-task budget, and
* verification that the task does not overrun its budgeted throughput
* allocation.  As an additional protection, a platform
* hardware watchdog will force a hardware reset of the system if it is not serviced
* within a defined time.
*
* Special care should be taken with data that is modified or accessed across
* multiple tasks, as expectations of data coherency cannot be assumed between
* them.  It is recommended that synchronization primitives (mutexes,
* semaphores, etc) or an IPO (Input-Process-Output) model is used in all software
* components to protect against these issues.
*
* #### Periodic Threads
*
* The majority of the processes in the VisiumSense system are periodic in
* nature; i.e. they run repeatedly at a given time interval or period.
*
* There are 3 classes of periodic tasks in the VisiumSense architecture:
*
*       o Background Thread
*           + Description
*               - Lowest priority thread.
*               - Background task to include BIT, internal monitoring
*                 (ie. checksums), etc.
*           + Can Be Interrupted
*               - Yes, by either the Main Periodic Thread (time interrupt) or
*                 the High Rate Thread(Interfacen FPGA interrupt).
*           + Iteration rate
*               - Not specified, runs whenever no other thread is running.
*                 running
*           + Throughput budget
*               - Not specified, will use up whatever remaining throughput is
*                 available after the other threads run
*           + Limitations and Notes
*               - No implied or assumed time of execution can be expected on
*                 the background thread, and nothing can be allocated to this
*                 task that requires specific timing.
*       o Main Periodic Thread
*           + Description
*               - Primary cyclic tasks
*               - Will toggle the analog input buffer indication and call the
*                 primary entry point for the AS.
*           + Can Be Interrupted
*               - Yes, but only by the High Rate Interrupt thread
*           + Iteration rate
*               - 200ms expected to be triggered on the analog input
*                 ping-pong buffer acquisition.
*           + Throughput budget
*               - 20% of Main Periodic Thread interrupt period. A 200ms period yields a
*                 40ms execution budget.
*           + Limitations and Notes
*               - The watchdog must be serviced at the beginning of this task.
*               - This provides a deterministic period for servicing the watchdog
*               - It assumes the overrun flag will catch the Application Software overruns
*               - and allows more flexibility in servicing any overruns
*       o High Rate Interrupt (HRI) Thread
*           + Description
*               - Fast rate, high priority task to handle time sensitive
*                 functionality.
*               - All functionality expected to run at a faster rate than the
*                 Main Periodic thread. It is required.  It is expected that a case statement or similar
*                 will provide the ability to schedule functionality on
*                 specific iterations of this task within the primary 200ms
*                 frame (ie, on every 3rd instance of this 50ms task, on the
*                 7th instance in the 200ms major frame, etc).
*           + Can Be Interrupted
*               - No, will disallow all but non-maskable interrupts
*           + Iteration rate
*               - 50ms
*               - An iteration rate of 50ms allows for all periodic ARINC transmissions
*               - and minimal delay in handling the received ARINC messages
*           + Throughput budget
*               - 20% of interrupt period. A 50ms period yields a budget of 12.5ms.
*                   * This is a hard not-to-exceed budget.
*           + Limitations and Notes
*               - The budget listed above is for the worst case execution
*                 within this task. For instance, if multiple functions could
*                 be called within an iteration of the task, the summation of
*                 their worst case execution time must be less than the budget
*                 listed above.
*               - Functionality allocated to this task should be as limited as
*                 feasible.
*
* \image rtf Scheduler.jpg
*
* Figure 1 shows how the various classes of tasks are able to interrupt each
* other.  In the image, all the threads are shown as differently colored boxes,
* with threads shown on top of any other threads that they would pre-empt.
* For example, when the 200ms thread and the 50ms thread are both scheduled at a
* specific time, the 50ms thread would preempt the 200ms thread.  Similarly, when
* there is no other thread scheduled, the background thread will run.
*
*
* #### APeriodic Tasks
*
* An aperiodic process is one whose execution is in response to an event
* (either internal or external to the system) that does not follow a periodic
* pattern.  Aperiodic processes are useful where a completely periodic system
* would have to use a polling approach to determine whether to execute a
* handler process.  Aperiodic processes can be used to handle external events
* such as receipt of a communication bus packets, etc.
*
* Aperiodic processes can be accommodated within a fixed-priority scheduling
* scheme, given that minimum inter-arrival time (i.e. the shortest time
* period between successive events) is known.  Aperiodic processes can then be
* modelled as periodic processes with a period less than the minimum
* inter-arrival time.
*
* The VisiumSense scheduler will use the High Rate Interrupt thread to
* execute any aperiodic tasks.
*
* #### Initialization Description
*
* On power-up, the OS perform its own initialization (including counters and
* interrupts) and shall then invoke AsInit to permit the AS to perform
* all initialization and power-up housekeeping. Upon return from AsInit(), the
* OS shall begin the Background Thread. During this thread, the OS invokes the
* OsBkGround task and the AsBkGroundTask in an infinite while-loop
* In this way, whenever the Main and High-Priority tasks
* are finished, the AS will consume all remaining time on background tasks.
*
* When the High-Priority interrupt is activated, the OS shall increment periodMSTime
* and, if the count exceeds 950 ms., shall wrap the value back to zero.  The OS
* then invokes AsHighRateTask. This task will employ the period of the High Rate
* Thread to potentially invoke different groupings
* of high priority tasks in different High-Priority periods within a single
* 200 ms interval.
*
* When the Main Priority Thread interrupt is activated, the OS shall perform the following actions:
*
* 1) Service the watchdog to avoid unnecessary reset.
* 2) Toggle the analog input ping-pong buffer pointer.
* 3) Detect and disposition Main Priority Thread overrun (if necessary).
* 4) Invokes AsMainPeriodicTask(void).
*
* ## Static Configuration
*
* This section contains all configuration items that will be statically defined
* at compile time along with any description needed for them.
*
* ### Tasks
*
* The Application Software shall provide the implementation of the
* following functions to be scheduled by the Operating System:
*
* ### Application Software Initialization
*
* The Operating System calls this function to provide an opportunity for the
* Application Software to perform any initializations required.
*
*       void AsInit(void)
*
* ### Application Background Task
*
* The Operating System calls this function to provide the means for the
* Application Software to execute any background tasks.
*
*       void AsBkGroundTask(void)
*
* ### Application Main Periodic Task
*
* The Operating System calls this function to provide the means for the
* Application Software to execute the main periodic task.
* This task processes the data from the Control FPGS (pressure, temperatrue, vibration, etc.)
*
*       void AsMainPeriodicTask(void)
*
* ### Application High Rate Task
*
* The Operating System calls this function to provide the means for the
* Application Software to execute a high rate periodic task (50ms task).
* This task handles the ARINC Tx/Rx messages
*
*       void AsHightRateTask(void)
*
*       periodMSTime : (from uint32_t GetHighRatePeriod(void))
*           Every 200 ms timer period this variable counts
*           from 0, 50, 100, 150,...,950 ms by 50 ms
*
*/
//***********************************************************************************

/* # Deferred into 2016
*
* The following functions have been identified as being required for the end 
* product but are not required for the initial product that will be delivered
* in 2015. 
* 
* ROMCheck    | ROM Check
* :---------- |:--------------------------------------------------------------|
* Function    | int32_t ROMCheck (uint32_t * checkAddress, uint32_t startAddress, uint32_t size);
* Argument    | checkAddress    address being checked
* Argument    | startAddress    start address
* Argument    | size    number of words to be checked
* Return Value| Returns 0 for success, Returns -1 for fail
* Description | Provides a rolling exlusive OR of memory, if error found then returns a fail.
* Comment     | Need to determine if the ROM check is supposed to only verify the image or NVM.
* Comment     | Also, need to look at the requirements to determine if this is a one time or continuous.
*
* RAMCheck    | RAM Check
* :---------- |:--------------------------------------------------------------|
* Function    | int32_t ROMCheck (uint32_t * checkAddress, uint32_t startAddress, uint32_t size);
* Argument    | checkAddress    address being checked
* Argument    | startAddress    start address
* Argument    | size    number of words to be checked
* Return Value| Returns 0 for success, Returns -1 for fail
* Description | Verifies each location in RAM can be written to and read from. 
* Comment     | 2 types of ramchecks. Power up, and then scu test for infrequently changing data.
* Comment     | This is to detect single event upsets that can occur to data that will not change
* Comment     | or change infrequently during power on. Need to think about this more and look at
* Comment     | the requirements. Also, need to determine if this should be a one time test or continuous test.
* 
*
// ****************************************************************************/

/* ************************************************************************//**
* @page Tasks
*
* # Application Software Tasks
* 
* The Application Software shall provide the implementation of the 
* following functions to be scheduled by the Operating System:
*
* ## Application Software Initialization
*
* The Operating System calls this function to provide an opportunity for the
* Application Software to perform any initializations required.
*
*       void AsInit(void)
*
* ## Application Background Task
*
* The Operating System calls this function to provide the means for the
* Application Software to execute any background tasks.
*
*       void AsBkGroundTask(void)
*
* ## Application Main Periodic Task
*
* The Operating System calls this function to provide the means for the
* Application Software to execute the main periodic task.
* This task processes the data from the FPGS (pressure, temperatrue, vibration, etc.)
*
*       void AsMainPeriodicTask(void)
*/
// ****************************************************************************

/* ************************************************************************//**
* @page Notes
*
*
* # Predefined Symbols
* Predefined symbols that are specified at compilation. Each of these symbols enables
* a specific feature within the OS.
*
* 		+ VS_EN_ICMP	Enable ICMP Echo Reply support within the stack
* 		+ VS_CDNFPGA	Use CDN FPGA with C6713 McBSP interface driver
* 		+ VS_LENDIAN	Set stack to function in little endian mode
*
* If VS_LENDIAN is not specified during compilation,then a pre-processor
* error is raised during compilation.
*
* # CDN FPGA Limitations
* The CDN FPGA driver is designed for the C6713 microprocessor target. It provides two network PHYs.
* The FPGA is a parallel interface, but must communicate via McBSP due to pin constraints.
*
* The McBSP interface runs at 37.5MHz, but requires one clock cycle (26ns) between messages
* to allow detection of synchronisation pulses. The CDN FPGA hardware imposes certain limitations
* on the Ethernet stack:
*
*		+ The source IP address for both network interfaces must be the same
*		+ Only the last two octals of the source MAC address can vary between the two interfaces
*		+ Only the last two octals of a destination MAC address can vary between routes
*		+ The socket port range is limited to 127 ports per interface (i.e. 32001 to 32128)
*		+ The Address Resolution Protocol is not supported by the CDN FPGA
*
*/
// ****************************************************************************

#ifndef PLA_OS_API_Headers_H
#define PLA_OS_API_Headers_H

/// @cond (!(API || BR700NG || T7000))
#include "Common.h"
#include "Arinc.h"
#include "Timer.h"
#include "ROMlnk.h"
#include "nvm.h"
#include "Reprogram.h"

// Constants
// NVM Size
#define NVM_SIZE            (1024)
// Maximum Ethernet Interfaces
#define MAX_ENET_ID         (3)         //jm I thought we had a 2 interfaces not 3
// Maximum Ethernet Frames
#define MAX_ENET_FRAMES     (30)
// Maximum Ethernet 
#define MAX_ENET_SIZE       (1522)
// Maximum Ethenet Payload
#define MAX_ENET_PAYLOAD    (1500)
// Maximum IP Payload
#define MAX_IP_PAYLOAD      (1480)
// TTL
#define TTL                 (128)
// Maxumum RS422 Buffer size
#define MAX_RS422_BUF       (262144)
// Received Parkets Per Fast ? 
#define RX_PKTS_PER_FAST    (5)

/// @endcond

/// @cond T7000
/* **************************************************************************
 * @brief Maximum Ethernet Receive Frames:
 *
 * This is the number of messages to transfer from the hardware receive
 * buffers into the NCB (Socket buffer). It's a means of controlling 
 * receive throughput of vs_networkProcessRX().
 *
 * Background:
 *
 * vs_networkProcessRX() is called periodically to process a single message from
 * the Ethernet hardware receive buffers. This function grabs a packet from the
 * physical Ethernet interface and processes it. It is a synchronous task and 
 * polled at a specified rate in an interrupt. 
 * 
 * The receive throughput can be increased by repeatedly calling the function a 
 * fixed number of times, or until the function returns
 * a value of 0. The value of MAX_ETHERNET_RX_FRAMES is TBD.
 */
// ****************************************************************************
// Maximum Ethernet frames to receive per cycle (TBD)
//#define MAX_ETHERNET_RX_FRAMES (5)
/// @endcond

/* ****************************************************************************
 * @brief Software Timer:
 *
 * Type structure to define a software timer.
 * All values are in the free run timer tick values.
 * Convert the tick value to milliseconds by dividing
 * the tick value by TICKSPERMS
 */
// ****************************************************************************

#ifdef _CAPTURE_EXE_TIMING
#define EXETIMEHIST  4  // depth of capture buffer
#endif

/* ************************************************************************//**
 * @brief Control FPGA Speed and Vibration Data:
 *
 * Type structure to define speed and vibration data in the ping-pong buffer.
 * Data format is defined as follows:
 *
 * Vibrations: 2's complement BNR, 24 significant bit signed extended
 * Speeds: 2's complement BNR.
 * Watchdog Timer: 2's complement BNR, 26 significant bits.
 *
 * Struct Member | Gain         | Units    | Min   		  | Max          |
 * :------------:|:------------:|:--------:|:------------:|:------------:|
 * pktHdr        | N/A			| N/A      |    N/A       |  N/A         |
 * sysStatus     | N/A			| Packed   |    N/A       |  N/A         |
 * freqN1Ca      |  40			| 1e-9 s   | -268,435,456 | 268,435,455  |
 * freqN2Ca      |  40			| 1e-9 s   | -268,435,456 | 268,435,455  |
 * freqN1Cb      |  40			| 1e-9 s   | -268,435,456 | 268,435,455  |
 * freqN2Cb      |  40			| 1e-9 s   | -268,435,456 | 268,435,455  |
 * freqN3a       |  40          | 1e-9 s   | -268,435,456 | 268,435,455  |
 * freqN1Ta      |  40          | 1e-9 s   | -268,435,456 | 268,435,455  |
 * wdt           |  40          | 1e-9 s   | -268,435,456 | 268,435,455  |
 * unused        | N/A          | N/A	   |    N/A       |  N/A         |
 * adcSampNum    |  25	        | 1e-6s    | -268,435,456 | 268,435,455  |
 * gearVib       | 6.87			| 1e-6 g   | -268,435,456 | 268,435,455  |
 * engVib1       | 6.87			| 1e-6 g   | -268,435,456 | 268,435,455  |
 * engVib2       | 6.87			| 1e-6 g   | -268,435,456 | 268,435,455  |
 * oprDetd       | N/A			| Packed   |    N/A       |  N/A         |
 * cfpgaPNum     | 1			| N/A	   |    N/A       |  N/A         |
 */
// ****************************************************************************
typedef struct
{
    uint32_t    pktHdr;  	/**< Packet Header Word. See JEMU/0325 for Trent7000 definition*/
    uint32_t    sysStatus;	/**< System Status Word. - DSP FPGA configuration wrap around. See JEMU/0325 for Trent7000 definition */
    uint32_t    freqN1Ca;	/**< N1C Channel A (Value provided in Period. Frequency = 1/Period)*/
    uint32_t    freqN2Ca;	/**< N2C Channel A (Value provided in Period. Frequency = 1/Period)*/
    uint32_t    freqN1Cb;	/**< N1C Channel B (Value provided in Period. Frequency = 1/Period)*/
    uint32_t    freqN2Cb;	/**< N2C Channel B (Value provided in Period. Frequency = 1/Period)*/
    uint32_t    freqN3a;	/**< N3 Channel A (Value provided in Period. Frequency = 1/Period)*/
    uint32_t    freqN1Ta;	/**< N1T Channel A (Value provided in Period. Frequency = 1/Period)*/
    uint32_t    wdt;        /**< Watchdog Timer - DSP watchdog timer wrap around. See JEMU/0325 for Trent7000 definition*/
    uint32_t	unused1;	/**< Unused word. Set to 0 */
    uint32_t	adcSampNum; /**< ADC Sample Number - The number of 40kHz samples captured since power-up.*/
    uint32_t    gearVib;	/**< Gear Vibration Word */
    uint32_t    engVib1;	/**< Engine Vibration 1 Word */
    uint32_t    engVib2;	/**< Engine Vibration 2 Word*/
    uint32_t	oprDetd;	/**< Once Per Revolution Short Tooth Detected*/
    uint32_t	cfpgaPNum;  /**< Part Number Representing FPGA Code Internal to The Control FPGA */
    uint32_t	unused2;	/**< Unused word. Set to 0 */

}DataPkt1_t;

/* ************************************************************************//**
 * @brief Control FPGA Pressure Data:
 *
 * Type structure to define pressure data in the ping-pong buffer.  
 * Data format is defined as follows:
 *
 * Pressures: 2's complement BNR, 24 significant bit signed extended.
 * PS160 Sample Number: See JEMU/0325 for Trent7000 definition.
 *
 * Struct Member | Gain         | Units  |    Min       |    Max       |
 * :------------:|:------------:|:------:|:------------:|:------------:|
 * pktHdr        | N/A			| N/A    |    N/A       |     N/A      |
 * pressSel      | N/A			| Packed | N/A          |     N/A      |
 * ps160Sig      | 0.298   		| 1e-3V  | -268,435,456 | 268,435,455  |
 * ps26Sig	     | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps160Temp     | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps26Temp      | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps160v0Sig    | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps26v0Sig     | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps160v0Temp   | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps26v0Temp    | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps160v5Sig    | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps26v5Sig     | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps160v5Temp   | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * ps26v5Temp    | 0.298	    | 1e-3V	 | -268,435,456 | 268,435,455  |
 * psExcit  	 | 0.298	    | 1e-3V  | -268,435,456 | 268,435,455  |
 * unused[2]     | N/A			|  N/A   | N/A          | N/A          |
 *
 */
// ****************************************************************************
typedef struct
{
    uint32_t    pktHdr;			/**< Packet Header Word. See JEMU/0325 for Trent7000 definition*/
    uint32_t	pressSel;		/**< Selected Pressure Sensor Input*/
    uint32_t    ps160Sig;		/**< PS160 Signal Word (mV = BNR * Gain)*/
    uint32_t    ps26Sig;		/**< PS26 Signal Word (mV = BNR * Gain)*/
    uint32_t    ps160Temp;		/**< PS160 Temperature Word (mV = BNR * Gain)*/
    uint32_t    ps26Temp;		/**< PS26 Temperature Word (mV = BNR * Gain)*/
    uint32_t    ps160v0Sig;		/**< PS160 0v Signal Word (mV = BNR * Gain)*/
    uint32_t    ps26v0Sig;		/**< PS26 0v Signal Word (mV = BNR * Gain)*/
    uint32_t    ps160v0Temp;	/**< PS160 0v Temperature Word (mV = BNR * Gain)*/
    uint32_t    ps26v0Temp;		/**< PS26 0v Temperature Word (mV = BNR * Gain)*/
    uint32_t    ps160v5Sig;		/**< PS160 5v Signal Word (mV = BNR * Gain)*/
    uint32_t    ps26v5Sig;		/**< PS26 5v Signal Word (mV = BNR * Gain)*/
    uint32_t    ps160v5Temp;	/**< PS160 5v Temperature Word (mV = BNR * Gain)*/
    uint32_t    ps26v5Temp;		/**< PS26 5v Temperature Word (mV = BNR * Gain)*/
    uint32_t    psExcit;		/**< Excitation Word (mV = BNR * Gain)*/
    uint32_t    unused[2];		/**< Unused*/
}DataPkt2_t;

/* ****************************************************************************
 * @brief Control FPGA Data:
 *
 * Type structure to define the data in the ping-pong buffer
 * A data sample set (two data packets of 17 words-32 bits - speed and vibration
 * data in one packet, and pressure data in the other) arrives each 25 micro seconds.
 * 1024 data sample sets are accumulated in a 25.6 millisecond period
 *
 * Example of how to reference the data in the data buffer:
 *
 *		DataSampleSet_t cntrlFPGADataBuf[1024];
 *
 */
// ****************************************************************************
/// @cond DOC
//typedef struct
//{
//    DataPkt1_t  pkt1;       /**< Speed and vibration data packet */
//    DataPkt2_t  pkt2;       /**< Pressure data packet */
//}DataSampleSet_t;
///@endcond

/* ************************************************************************//**
 * @brief Pressure Sensor Info:
 *
 * This structure stores pressure sensor metadata copied from the sensor NVM.
 * sensor
 */
// ****************************************************************************
typedef struct
{
	uint8_t pressureType;		/**< Pressure Type: 0 = P160, 1 = P26 */
    uint16_t dataFormat;       	/**< Calibration Data Format Specifier */
    uint8_t deviceID[29];       /**< Device ID - ASCII Text*/
    uint8_t pressureStyle;		/**< Pressure Style ASCII Text: A = Absolute, G = Guage */
    float32_t presRange;		/**< Pressure Range */
    uint32_t serialNumber;		/**< Device Serial Number */
    uint8_t calDay;   			/**< Date of Calibration : 1 to 31 */
    uint8_t calMonth;   		/**< Date of Calibration : 1 to 12 */
    uint8_t calYear;   			/**< Date of Calibration : 1 to 99 */
}PressInfo_t;

/* ************************************************************************//**
 * @brief Select P160
 *
 * Select P160 Sensor
 */
// ****************************************************************************
#define PRESS_P26	0U

/* ************************************************************************//**
 * @brief Select P26
 *
 * Select P26 Sensor
 */
// **************************************************************************
#define PRESS_P160	1U

/* **********************************************************************//**
 * @brief Good Pressure Calibration Data
 *
 * Checksum passed for read calibration data for a given pressure sensor
 */
// ****************************************************************************
#define PRES_CAL_GOODDATA	0U

/* ***********************************************************************//**
 * @brief Bad Pressure Calibration Data
 *
 * Checksum failed for read calibration data for a given pressure sensor.
 */
// ****************************************************************************
#define PRES_CAL_BADDATA	1U

/* **********************************************************************//**
 * @brief No Pressure Calibration Data
 *
 * No calibration data read for a given pressure sensor.
 */
// **************************************************************************
#define PRES_CAL_NODATA		2U

/* **********************************************************************//**
 * @brief Invalid Pressure Type
 *
 * An invalid pressure type selected
 */
// ****************************************************************************
#define PRES_INVALID_PRES_TYPE	3U

/* **********************************************************************//**
 * @brief PTM Calibration Idle
 *
 * Pressure calibration is in idle state
 */
// ****************************************************************************
#define PRES_CAL_IDLE	0U

/* **********************************************************************//**
 * @brief PTM Calibiration Busy
 *
 * Pressure calibration is in busy state
 */
// ****************************************************************************
#define PRES_CAL_BUSY	1U

/* **********************************************************************//**
 * @brief PTM Calibration Complete
 *
 * Pressure calibration is complete
 */
// ****************************************************************************
#define PRES_CAL_COMPLETE	2U

 // ****************************************************************************
// Function Prototypes
 // ****************************************************************************


/* ************************************************************************//**
 * @brief Returns a pointer to a buffer containing speed and vibration samples.
 *
 * Each 25 micro seconds, the control FPGA processes speed and vibration
 * samples into a 17-word packet (Packet #1). Over 25.6 milliseconds, 1024 
 * packets are buffered and sent to the DSP via an interrupt. This API
 * returns a pointer to the buffer.
 * 
 * NOTE: The data in this buffer is only guaranteed for the current scheduling
 * period.
 *
 * @return  return the address of the data sample set available for processing
 * by the AS  Note: can return NULL (when data is not available for processing)
 *
 * The return value can be used as an address of an array of 1024 data sample sets
 *
 *		DataPkt1_t cntrlFPGADataBuf[1024];
 *
*/
// ****************************************************************************
DataPkt1_t * GetCurrentAdcSpdVibBuf (void);

/// @cond DOC
/*
typedef struct{
	uint32_t swStartAddress;
	uint32_t swNumBytes;
	uint8_t swProgImage[0x400000]; //4MB max size
} swImage;
*/

/// @endcond
/* ************************************************************************//**
 * @brief Reprogram
 *
 * This function forces the software to enter reprogramming mode by writing
 * a pattern to RAM and then forcing a WDT reset. When the Bootloader comes
 * up it will read the pattern in RAM and wait until the image is received
 * via RS422.
 *
 * 
 * @return none
 */
// ****************************************************************************
void Reprogram (void);

/* ************************************************************************//**
 * @brief Reset Processor
 *
 * Resets the processor
 *
 * Prior to calling this function it is expected that the AS verifies that
 * the system is in a proper state for resetting
 * The function will force the watchdog timeout to reset the system
 *
 * @return None ... this function never returns
 */
// ****************************************************************************
void AS_Reset (void);

/****************************************************************************
 * @brief NVM Write
 *
 * This function writes 32 bits of data to a specified NVM location.
 *
 * @param address NVM address to write to
 * @param data    Data to be written to NVM (aligned 4 bytes)
 * @param size    Size of data in bytes
 *
 * @return NVM_SUCCESS:               write succeeded
 * @return NVM_ADDRESS_OUT_OF_BOUNDS: address was outside the NVM area
 * @return NVM_PROGRAMMING_TIMED_OUT: write operation timed out
 *****************************************************************************/
//int32_t NVMWrite(uint32_t address, void *data, uint32_t size);

/****************************************************************************
 * @brief NVM Read:
 *
 * This function Reads NVM location and returns 32 bit value at the address given.
 *
 * @param address   NVM address to be read
 * @param readValue Value read from NVM (unaligned 32 bits)
 *
 * @return NVM_SUCCESS: success
 * @return NVM_ADDRESS_OUT_OF_BOUNDS: input address was out of bounds
 *****************************************************************************/
//int32_t NVMRead(uint32_t address, void *readValue);

/**************************************************************************//**
 * @brief NVMDump:
 *
 * This function dumps all persistent data from NVM.
 *
 * @param[out] data: A pointer to the pointer where the starting NVM address
 *                   will be placed.
 * @param[out] size: A pointer where the size of the NVM persistent data area
 *                   will be placed.
 *****************************************************************************/
void NVMDump(uint8_t **data, uint32_t *size);

/**************************************************************************//**
 * @brief LPTBWriteRecord:
 *
 * This function writes a fan trim balance record to the persistent data
 * section of NVM.
 *
 * @param record: The record data to write.
 * @param size:   The size of the data to write, in bytes.
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t LPTBWriteRecord(uint8_t *record, uint32_t size);

/****************************************************************************
 * @brief LPTBGetRecordIterator:
 *
 * This function initializes an iterator that will iterate over records stored
 * in the fan trim balance section.
 *
 * @param iter: A pointer to an iterator to initialize.
 *****************************************************************************/
//void LPTBGetRecordIterator(NVMIterator *iter);

/****************************************************************************
 * @brief LPTBRecordIteratorNext:
 *
 * This function increments the record pointed to by a fan trim balance record
 * iterator.
 *
 * @param iter: A pointer to an iterator whose record is to be incremented
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
//Boolean_t LPTBRecordIteratorNext(NVMIterator *iter);

/**************************************************************************//**
 * @brief LPTBDump:
 *
 * This function dumps all fan trim balance data.
 *
 * @param[out] data: A pointer to the pointer where the starting fan trim
 *                   balance data will be placed.
 * @param[out] size: A pointer where the size of the fan trim balance data will
 *                   be placed.
 *****************************************************************************/
void LPTBDump(uint8_t **data, uint32_t *size);

/* ************************************************************************//**
 * @brief LPTBBufferSize
 *
 * Returns the size of the LPTB buffer.
 *
 * @return the size of the LPTB buffer
 *****************************************************************************/
uint8_t LPTBBufferSize(void);

/**************************************************************************//**
 * @brief LPTBHalted:
 *
 * This function returns true if the LPTB record buffer has been halted due to
 * a fault.  When this occurs, the application may examine fault codes and take
 * appropriate action.  Afterwards, the application can restart the buffer by
 * calling LPTBResume.
 *
 * @return A boolean indicating whether or not the LPTB record buffer has
 *         halted.
 *****************************************************************************/
Boolean_t LPTBHalted(void);

/**************************************************************************//**
 * @brief LPTBErasing
 *
 * This function returns true if the LPTB record buffer is currently erasing a
 * NOR sector.
 *
 * @return A boolean indicating whether or not the LPTB record buffer is
 *         erasing.
 *****************************************************************************/
Boolean_t LPTBErasing(void);

/**************************************************************************//**
 * @brief LPTBResume
 *
 * This function resumes LPTB record buffer operations.
 *****************************************************************************/
void LPTBResume(void);

/**************************************************************************//**
 * @brief SLOGWriteRecord:
 *
 * This function writes a status log record to the persistent data section of
 * NVM.
 *
 * @param record: The record data to write.
 * @param size:   The size of the data to write, in bytes.
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t SLOGWriteRecord(uint8_t *record, uint32_t size);

/****************************************************************************
 * @brief SLOGGetRecordIterator:
 *
 * This function initializes an iterator that will iterate over records stored
 * in the status log section.
 *
 * @param iter: A pointer to an iterator to initialize.
 *****************************************************************************/
//void SLOGGetRecordIterator(NVMIterator *iter);

/****************************************************************************
 * @brief SLOGRecordIteratorNext:
 *
 * This function increments the record pointed to by a status log record
 * iterator.
 *
 * @param iter: A pointer to an iterator whose record is to be incremented
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
//Boolean_t SLOGRecordIteratorNext(NVMIterator *iter);

/**************************************************************************//**
 * @brief SLOGDump:
 *
 * This function dumps all status log data.
 *
 * @param[out] data: A pointer to the pointer where the starting status log
 *                   data will be placed.
 * @param[out] size: A pointer where the size of the status log data will be
 *                   placed.
 *****************************************************************************/
void SLOGDump(uint8_t **data, uint32_t *size);

/* ************************************************************************//**
 * @brief SLOGBufferSize
 *
 * Returns the size of the SLOG buffer.
 *
 * @return the size of the SLOG buffer
 *****************************************************************************/
uint8_t SLOGBufferSize(void);

/**************************************************************************//**
 * @brief SLOGHalted:
 *
 * This function returns true if the SLOG record buffer has been halted due to
 * a fault.  When this occurs, the application may examine fault codes and take
 * appropriate action.  Afterwards, the application can restart the buffer by
 * calling SLOGResume.
 *
 * @return A boolean indicating whether or not the SLOG record buffer has
 *         halted.
 *****************************************************************************/
Boolean_t SLOGHalted(void);

/**************************************************************************//**
 * @brief SLOGErasing
 *
 * This function returns true if the SLOG record buffer is currently erasing a
 * NOR sector.
 *
 * @return A boolean indicating whether or not the SLOG record buffer is
 *         erasing.
 *****************************************************************************/
Boolean_t SLOGErasing(void);

/**************************************************************************//**
 * @brief SLOGResume
 *
 * This function resumes SLOG record buffer operations.
 *****************************************************************************/
void SLOGResume(void);

/**************************************************************************//**
 * @brief FRECWriteRecord:
 *
 * This function writes a fault record to the persistent data section of NVM.
 *
 * @param record: The record data to write.
 * @param size:   The size of the data to write, in bytes.
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t FRECWriteRecord(uint8_t *record, uint32_t size);

/****************************************************************************
 * @brief FRECGetRecordIterator:
 *
 * This function initializes an iterator that will iterate over records stored
 * in the fault record section.
 *
 * @param iter: A pointer to an iterator to initialize.
 *****************************************************************************/
//void FRECGetRecordIterator(NVMIterator *iter);

/****************************************************************************
 * @brief FRECRecordIteratorNext:
 *
 * This function increments the record pointed to by a fault record iterator.
 *
 * @param iter: A pointer to an iterator whose record is to be incremented
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
//Boolean_t FRECRecordIteratorNext(NVMIterator *iter);

/**************************************************************************//**
 * @brief FRECDump:
 *
 * This function dumps all fault record data.
 *
 * @param[out] data: A pointer to the pointer where the starting fault record
 *                   data will be placed.
 * @param[out] size: A pointer where the size of the fault record data will be
 *                   placed.
 *****************************************************************************/
void FRECDump(uint8_t **data, uint32_t *size);

/* ************************************************************************//**
 * @brief FRECBufferSize
 *
 * Returns the size of the FREC buffer.
 *
 * @return the size of the FREC buffer
 *****************************************************************************/
uint8_t FRECBufferSize(void);

/**************************************************************************//**
 * @brief FRECHalted:
 *
 * This function returns true if the FREC record buffer has been halted due to
 * a fault.  When this occurs, the application may examine fault codes and take
 * appropriate action.  Afterwards, the application can restart the buffer by
 * calling FRECResume.
 *
 * @return A boolean indicating whether or not the FREC record buffer has
 *         halted.
 *****************************************************************************/
Boolean_t FRECHalted(void);

/**************************************************************************//**
 * @brief FRECErasing
 *
 * This function returns true if the FREC record buffer is currently erasing a
 * NOR sector.
 *
 * @return A boolean indicating whether or not the FREC record buffer is
 *         erasing.
 *****************************************************************************/
Boolean_t FRECErasing(void);

/**************************************************************************//**
 * @brief FRECResume
 *
 * This function resumes FREC record buffer operations.
 *****************************************************************************/
void FRECResume(void);

#ifdef ARINC_HW_PRESENT
/// @cond BR700NG
/* ************************************************************************//**
 * @brief ARINC 429 Set Valid Reception Labels and SDI Values
 *
 * This function is called once by AS at start-up. 
 *
 * This function takes an array of ARINC 429 labels and SDI values (pu32_Labels)
 * with the number of labels given by u32_NumLabels.  The array contains pairs 
 * of labels and SDI values so the number of elements in the array is twice the
 * number of labels, as illustrated by the following table: 
 *
 * Array Element | Meaning
 * :------------:|:-------:
 * 1             | Label #1
 * 2             | SDI #1
 * 3             | Label #2
 * 4             | SDI #2
 * ...           | ...
 * N * 2 -1      | Label #N
 * N * 2         | SDI #N
 *
 * The function shall accept between 1 and 50 labels.
 * 
 * Each label is specified by its 8-bit transmision value. For example, 
 * label 307 base 8 would be represented in C as:
 *
 * 		#define LABEL_307 0xE3
 *
 * NOTE: Need further clarification from the AS team whether the OS should 
 * provide additional filtering.
 *
 * @param u32_NumLabels Number of labels in pu32_Labels array
 * @param pu32_Labels Array of ARINC 429 labels and associated SDI values
 *
 * @return true = Success, false = Fail
 */
// ****************************************************************************
Boolean_t
Arinc429SetRxLabels(uint32_t u32_NumLabels, const uint32_t * pu32_Labels);

/* ************************************************************************//**
 * @brief Read ARINC Word
 *
 * ReadArinc is used by the AS to read a 32 bit ARINC 429 word
 * from the ARINC software receive queue.
 *
 * @param p_arincWord: address where the function will store the
 * 32 bit ARINC 429 word, if the read is successful.
 *
 * @return true = Success, false = Fail;
 * p_arincWord will contain ARINC data on success.
 */
// ****************************************************************************
Boolean_t
ReadArinc (uint32_t * p_arincWord);

/* ************************************************************************//**
 * @brief   Write ARINC Word
 *
 * WriteArinc is used by the AS to tranmit a 32 bit ARINC 429 word.
 * This function plases the data into a software transmit queue
 * for later transmission by the polling function
 *
 * @param arincWord 32 bit ARINC 429 word to be transmitted
 *
 * @return true = Success, false = Fail
 */
// ****************************************************************************
Boolean_t
WriteArinc (uint32_t arincWord);
/// @endcond
#endif
/* ************************************************************************//**
 * @brief ReadRS422
 *
 * Read a number of RS422 bytes from the software receive queue
 *
 * @param u32_NumBytes Number of bytes to read
 * @param p_bytes Pointer to where to place the received data
 *
 * @return number of bytes read into the destination location
 */
// ****************************************************************************
uint32_t
ReadRS422 (uint32_t u32_NumBytes, uint8_t * p_bytes);

/* ************************************************************************//**
 * @brief WriteRS422
 *
 * Write a given number of bytes to the software transmit queue
 * for later transmission by the polling function
 *
 * @param numBytes Number of bytes to transmit
 * @param p_bytes pointer to bytes to transmit
 *
 * @return number of bytes written to software queue
 */
// ****************************************************************************
uint32_t
WriteRS422 (uint32_t numBytes, uint8_t * p_bytes);


/* ************************************************************************//**
 * @brief   Initialize Software Timer:
 *
 * This function initializes a specific Software Timer to zero values, and set
 * min to max and max to min so that first time through the min/max will be
 * set to real data.
 *
 * @param timer Timer ID
 *
 * @return (True = Timer ID is in range,
 *          False = Timer ID not in range)
 */
// ****************************************************************************
Boolean_t
InitSWTimer(uint32_t timer);

/* ************************************************************************//**
 * @brief   Start Software Timer:
 *
 * This function sets the start time of the selected timer to the current time.
 *
 * @param  timer Timer ID
 *
 * @return (True = Timer ID is in range,
 *          False = Timer ID not in range)
 */
// ****************************************************************************
Boolean_t
StartSWTimer(uint32_t timer);

/* ************************************************************************//**
 * @brief   End Software Timer:
 *
 * This function sets SW timer end-time to current time.
 * Computes all other values based on start and end times
 *
 * @param  timer Timer ID
 *
 * @return (True = Timer ID is in range,
 *          False = Timer ID not in range)
 */
// ****************************************************************************
Boolean_t
EndSWTimer(uint32_t timer);

/* ************************************************************************//**
 * @brief   Get Software Timer:
 *
 * This function returns the values of a specific software timer
 *
 * @param  timerId Timer ID.
 * @param  p_swTimer Structure to return the timer values.
 *
 * @return (True = Timer ID is in range,
 *          False = Timer ID not in range)
 */
// ****************************************************************************
Boolean_t
GetSWTimer(uint32_t timerId, SWTimer_t   *p_swTimer);

/* ************************************************************************//**
 * @brief   Start Power-up Built-In Test:
 *
 * This function commands the OS to carry out the remaining half of PBIT. This
 * should be called only if the AS has determined that the airplane is on the
 * ground and the engines are off.
 *
 * @return (True = BIT initiated successfully,
 *          False = BIT error)
 */
// ****************************************************************************
Boolean_t
InitiatePBIT(void);

/* ************************************************************************//**
 * @brief   Start Initiated Built-In Test:
 *
 * This function commands the OS to carry out an Initiated BIT. This should be
 * called only if the AS has determined that the airplane is on the ground and
 * the engines are off.
 *
 * @return (True = BIT initiated successfully,
 *          False = BIT error)
 */
// ****************************************************************************
Boolean_t
InitiateIBIT(void);

/* ************************************************************************//**
 * @brief   Get System Faults:
 *
 * This function stores all collected system faults to the appropriate location in
 * RAM for reference by the AS. The table below describes the faults in the buffer
 * returned by this API.

 * | Offset | Fault Word Description                      | Bit    | Bit Description                                                   | AS Response                                                                  |
 * |:------:|:-------------------------------------------:|:------:|:-----------------------------------------------------------------:|:---------------------------------------------------------:|
 * | 0      | CBIT Fault Word                             | 0      | GPIO configuration does not match expected value                  | OS to correct; if fault occurs repeatedly, GPIO failure                      |
 * | 0      | CBIT Fault Word                             | 1      | I2C bus was enabled when it was expected to be disabled           | OS to correct; if fault occurs repeatedly, I2C failure                       |
 * | 0      | CBIT Fault Word                             | 2      | Stack pointer not properly reset after function return            | AS Reset                                                                     |
 * | 0      | CBIT Fault Word                             | 3      | Stack has overrun its bounds                                      | AS Reset                                                                     |
 * | 0      | CBIT Fault Word                             | 4      | Main Task has exceeded its maximum allowed execution time         |                                                                              |
 * | 0      | CBIT Fault Word                             | 5      | Watchdog timer reset detected                                     |                                                               |
 * | 0      | CBIT Fault Word                             | 6      | High Rate Task execution time has exceeded a Minor Frame          |                                                                              |
 * | 0      | CBIT Fault Word                             | 7      | OS Background Task has exceeded its maximum allowed execution time|                                                                              |
 * | 0      | CBIT Fault Word                             | 8      | AS Background Task has exceeded its maximum allowed execution time|                                                                              |
 * | 0      | CBIT Fault Word                             | 9      |                                                                   |                                                               |
 * | 0      | CBIT Fault Word                             | 10     | RAM CRC does not match                                            | AS Reset                                                                     |
 * | 0      | CBIT Fault Word                             | 11     | McASP bus was enabled when it was expected to be disabled         | OS to correct; if fault occurs repeatedly, McASP failure                     |
 * | 0      | CBIT Fault Word                             | 12     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 13     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 14     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 15     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 16     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 17     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 18     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 19     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 20     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 21     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 22     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 23     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 24     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 25     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 26     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 27     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 28     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 29     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 30     | Unused                                                            |                                                                              |
 * | 0      | CBIT Fault Word                             | 31     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 0      | N1C Channel A Fault                                               |                                                                              |
 * | 1      | PBIT Fault Word                             | 1      | N2C Channel A Fault                                               |                                                                              |
 * | 1      | PBIT Fault Word                             | 2      | N1C Channel B Fault                                               |                                                                              |
 * | 1      | PBIT Fault Word                             | 3      | N2C Channel B Fault                                               |                                                                              |
 * | 1      | PBIT Fault Word                             | 4      | N3 Channel A Fault                                                |                                                                              |
 * | 1      | PBIT Fault Word                             | 5      | N1T Channel A Fault                                               |                                                                              |
 * | 1      | PBIT Fault Word                             | 6      | GearBox Vibration Channel Fault                                   |                                                                              |
 * | 1      | PBIT Fault Word                             | 7      | Engine Vibration Channel 1 Fault                                  |                                                                              |
 * | 1      | PBIT Fault Word                             | 8      | Engine Vibration Channel 2 Fault                                  |                                                                              |
 * | 1      | PBIT Fault Word                             | 9      | Main Image CRC does not match                                     |                                                                              |
 * | 1      | PBIT Fault Word                             | 10     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 11     | DSP internal RAM pattern test failed                              |                                                                              |
 * | 1      | PBIT Fault Word                             | 12     | External RAM pattern test failed                                  |                                                                              |
 * | 1      | PBIT Fault Word                             | 13     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 14     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 15     | McBSP0 fault thrown while trying to send CFPGA configuration      | Report EMU failure                                                           |
 * | 1      | PBIT Fault Word                             | 16     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 17     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 18     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 19     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 20     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 21     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 22     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 23     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 24     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 25     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 26     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 27     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 28     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 29     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 30     | Unused                                                            |                                                                              |
 * | 1      | PBIT Fault Word                             | 31     | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word  						      | 0      | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word  						      | 1      | Size of passed record exceeds the maximum record size             |                                                                              |
 * | 2      | NVM Status Word                             | 2      | Attempted to write outside of the defined NVM address space       |                                                                              |
 * | 2      | NVM Status Word                             | 3      | Erasing a sector timed out                                        |                                                                              |
 * | 2      | NVM Status Word                             | 4      | Size of passed record exceeds the maximum record size             |                                                                              |
 * | 2      | NVM Status Word                             | 5      | Writing the record failed                                         |                                                                              |
 * | 2      | NVM Status Word                             | 6      | The record has already been written to                            |                                                                              |
 * | 2      | NVM Status Word                             | 7      | Attempted to write a record, but did not finish                   |                                                                              |
 * | 2      | NVM Status Word                             | 8      | The record is marked bad                                          |                                                                              |
 * | 2      | NVM Status Word                             | 9      | Checking the status flags of a record failed                      |                                                                              |
 * | 2      | NVM Status Word                             | 10     | The CRC of stored data does not match the stored CRC              |                                                                              |
 * | 2      | NVM Status Word                             | 11     | A sector is empty                                                 |                                                                              |
 * | 2      | NVM Status Word                             | 12     | A sector is full                                                  |                                                                              |
 * | 2      | NVM Status Word                             | 13     | The newest sector for a section is empty                          |                                                                              |
 * | 2      | NVM Status Word                             | 14     | More than one sector in a section has the same index              |                                                                              |
 * | 2      | NVM Status Word                             | 15     | Marking a record bad during erase failed                          |                                                                              |
 * | 2      | NVM Status Word                             | 16     | The section is uninitialized                                      |                                                                              |
 * | 2      | NVM Status Word                             | 17     | A record is blank                                                 |                                                                              |
 * | 2      | NVM Status Word                             | 18     | The newest sector in a section is blank                           |                                                                              |
 * | 2      | NVM Status Word                             | 19     | Unable to locate the previous sector                              |                                                                              |
 * | 2      | NVM Status Word                             | 20     | Unable to locate the current sector                               |                                                                              |
 * | 2      | NVM Status Word                             | 21     | The passed buffer is invalid                                      |                                                                              |
 * | 2      | NVM Status Word                             | 22     | The buffer is halted                                              | Clear HALT status using API; if status reoccurs repeatedly, NVM has degraded |
 * | 2      | NVM Status Word                             | 23     | The buffer is full                                                | Resend data next cycle                                                       |
 * | 2      | NVM Status Word                             | 24     | The section's record cursor is at the end of the sector           |                                                                              |
 * | 2      | NVM Status Word                             | 25     | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word                             | 26     | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word                             | 27     | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word                             | 28     | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word                             | 29     | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word                             | 30     | Unused                                                            |                                                                              |
 * | 2      | NVM Status Word                             | 31     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 0      | McBSP 0 Transmit Timeout                                          | Transmit failed; data dropped (Can occur during ctrnl FPGA init & PTM calibration) no AS action required   |
 * | 3      | McBSP Fault Word                            | 1      | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 2      | McBSP 1 Transmit Timeout                                          | Data dropped  (API vs_sendto returns 0 bytes)                                |
 * | 3      | McBSP Fault Word                            | 3      | McBSP 1 Receive Timeout                                           | No data received (API vs_recvfrom returns 0 bytes)                           |
 * | 3      | McBSP Fault Word                            | 4      | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 5      | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 6      | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 7      | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 8      | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 9      | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 10     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 11     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 12     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 13     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 14     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 15     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 16     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 17     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 18     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 19     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 20     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 21     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 22     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 23     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 24     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 25     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 26     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 27     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 28     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 29     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 30     | Unused                                                            |                                                                              |
 * | 3      | McBSP Fault Word                            | 31     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 0      | RS422 failed to initialize properly                               | OS to correct; if fault occurs repeatedly, RS422 failure                     |
 * | 4      | RS422 Fault Word                            | 1      | I2C bus fault                                                     | OS to correct; if fault occurs repeatedly, I2C failure                       |
 * | 4      | RS422 Fault Word                            | 2      | RS422 software transmit buffer has overrrun                       | Data dropped                                                                 |
 * | 4      | RS422 Fault Word                            | 3      | RS422 software receive buffer has overrun                         | Data dropped                                                                 |
 * | 4      | RS422 Fault Word                            | 4      | Excessive NACKs from I2C-RS422 bridge                             | Data dropped                                                                 |
 * | 4      | RS422 Fault Word                            | 5      | Excessive failure of RS422 transmission                           | Data dropped; if fault occurs repeatedly, RS422 failure                      |
 * | 4      | RS422 Fault Word                            | 6      | Excessive failure of RS422 reception                              | Data dropped; if fault occurs repeatedly, RS422 failure                      |
 * | 4      | RS422 Fault Word                            | 7      | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 8      | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 9      | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 10     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 11     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 12     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 13     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 14     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 15     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 16     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 17     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 18     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 19     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 20     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 21     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 22     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 23     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 24     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 25     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 26     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 27     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 28     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 29     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 30     | Unused                                                            |                                                                              |
 * | 4      | RS422 Fault Word                            | 31     | Unused                                                            |                                                                              |
 * | 5      | Ethernet Status Word                        | 0      | Latched A Pmjab                                                   | TODO Revisit                                                                 |
 * | 5      | Ethernet Status Word                        | 1      | Latched B Pmjab                                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 2      | Latched A Pmpol                                              	   |                                                                              |
 * | 5      | Ethernet Status Word                        | 3      | Latched B Pmpol          		                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 4      | Rx A Link Test Fail        	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 5      | Rx B Link Test Fail       		                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 6      | Unused                     	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 7      | Unused                     	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 8      | Transmit Buffer Overrun       	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 9      | Transmit Buffer is Empty                                          |                                                                              |
 * | 5      | Ethernet Status Word                        | 10     | Unused                     	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 11     | Unused                     	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 12     | Unused                     	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 13     | Unused                    		                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 14     | Unused                    		                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 15     | Unused                     	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 16     | Latched Tx Buffer Parity Fail                                     |                                                                              |
 * | 5      | Ethernet Status Word                        | 17     | Latched Rx A Buffer Parity Fail                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 18     | Latched Rx B Buffer Parity Fail                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 19     | Unused                     	                                   |                                                                              |
 * | 5      | Ethernet Status Word                        | 20     | Latched Look-up Parity Fail                                       |                                                                              |
 * | 5      | Ethernet Status Word                        | 21     | Latched Rx EDE CRC X Parity Fail                                  |                                                                              |
 * | 5      | Ethernet Status Word                        | 22     | Latched Rx EDE CRC Y Parity Fail                                  |                                                                              |
 * | 5      | Ethernet Status Word                        | 23     | Latched Rx A IP Frag Parity Fail                                  |                                                                              |
 * | 5      | Ethernet Status Word                        | 24     | Latched Rx B IP Frag Parity Fail                                  |                                                                              |
 * | 5      | Ethernet Status Word                        | 25     | Tx Length Fail                                                    |                                                                              |
 * | 5      | Ethernet Status Word                        | 26     | Latched Rx B Overrun                                              |                                                                              |
 * | 5      | Ethernet Status Word                        | 27     | Latched Rx A Overrun                                              |                                                                              |
 * | 5      | Ethernet Status Word                        | 28     | Unused                                                            |                                                                              |
 * | 5      | Ethernet Status Word                        | 29     | Unused                                                            |                                                                              |
 * | 5      | Ethernet Status Word                        | 30     | Unused                                                            |                                                                              |
 * | 5      | Ethernet Status Word                        | 31     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 0      | Look-up Fail                                                      |                                                                              |
 * | 6      | Ethernet Fault Word                         | 1      | MAC Destination Address Fail                                      |                                                                              |
 * | 6      | Ethernet Fault Word                         | 2      | MAC Type Fail                                                     |                                                                              |
 * | 6      | Ethernet Fault Word                         | 3      | MAC FCS Fail                                                      |                                                                              |
 * | 6      | Ethernet Fault Word                         | 4      | Octet Fail                                                        |                                                                              |
 * | 6      | Ethernet Fault Word                         | 5      | MAC Frame Length Fail                                             |                                                                              |
 * | 6      | Ethernet Fault Word                         | 6      | VLID Fail                                                         |                                                                              |
 * | 6      | Ethernet Fault Word                         | 7      | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 8      | IP Version Fail                                                   |                                                                              |
 * | 6      | Ethernet Fault Word                         | 9      | IP IHL Fail                                                       |                                                                              |
 * | 6      | Ethernet Fault Word                         | 10     | IP Type of Service Fail                                           |                                                                              |
 * | 6      | Ethernet Fault Word                         | 11     | IP Length Fail                                                    |                                                                              |
 * | 6      | Ethernet Fault Word                         | 12     | IP Control Fail                                                   |                                                                              |
 * | 6      | Ethernet Fault Word                         | 13     | IP TTL Fail                                                       |                                                                              |
 * | 6      | Ethernet Fault Word                         | 14     | IP Protocol Fail                                                  |                                                                              |
 * | 6      | Ethernet Fault Word                         | 15     | IP Checksum Fail                                                  |                                                                              |
 * | 6      | Ethernet Fault Word                         | 16     | IP Destination Address Fail                                       |                                                                              |
 * | 6      | Ethernet Fault Word                         | 17     | Slot Overflow                                                     |                                                                              |
 * | 6      | Ethernet Fault Word                         | 18     | Out of Order Fragment                                             |                                                                              |
 * | 6      | Ethernet Fault Word                         | 19     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 20     | UDP Checksum Fail                                                 |                                                                              |
 * | 6      | Ethernet Fault Word                         | 21     | EDE CRC X Fail                                                    |                                                                              |
 * | 6      | Ethernet Fault Word                         | 22     | EDE CRC S Fail                                                    |                                                                              |
 * | 6      | Ethernet Fault Word                         | 23     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 24     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 25     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 26     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 27     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 28     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 29     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 30     | Unused                                                            |                                                                              |
 * | 6      | Ethernet Fault Word                         | 31     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 0      | P160 NVM Checksum Fail                                            | OS Rerequest when failure                                                    |
 * | 7      | PTM Fault Word                              | 1      | P26 NVM Checksum Fail                                             | OS Rerequest on failure                                                      |
 * | 7      | PTM Fault Word                              | 2      | Packet 2 P160 Refresh Fault                                       | AS Rerequest calibration                                                     |
 * | 7      | PTM Fault Word                              | 3      | Packet 2 P26 Refresh Fault                                        | AS Rerequest calibration                                                     |
 * | 7      | PTM Fault Word                              | 4      | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 5      | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 6      | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 7      | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 8      | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 9      | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 10     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 11     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 12     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 13     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 14     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 15     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 16     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 17     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 18     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 19     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 20     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 21     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 22     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 23     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 24     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 25     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 26     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 27     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 28     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 29     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 30     | Unused                                                            |                                                                              |
 * | 7      | PTM Fault Word                              | 31     | Unused                                                            |                                                                              |
 *
 * @param  currentFaults Pointer to current fault word array
 * @param  latchedFaults Pointer to latched fault word array
 *
 * @return (True = No BIT faults detected,
 *          False = Faults detected)
 */
// ****************************************************************************
Boolean_t
GetSystemFaults(uint32_t **currentFaults, uint32_t **latchedFaults);

/**************************************************************************//**
 * @brief NVMDump:
 *
 * This function dumps all persistent data from NVM.
 *
 * @param[out] data: A pointer to the pointer where the starting NVM address
 *                   will be placed.
 * @param[out] size: A pointer where the size of the NVM persistent data area
 *                   will be placed.
 *****************************************************************************/
void NVMDump(uint8_t **data, uint32_t *size);

/**************************************************************************//**
 * @brief PartNumberRead:
 *
 * This function fetches the part number from NVM.
 *
 * @param[out] data: A pointer to the pointer where the part number will be
 *                   placed.
 * @return Boolean indicating whether or not CRC check passed.
 *****************************************************************************/
Boolean_t PartNumberRead(uint8_t **data);

/**************************************************************************//**
 * @brief SerialNumberRead:
 *
 * This funvoidction fetches the serial number from NVM.
 *
 * @param[out] data: A pointer to the pointer where the serial number will be
 *                   placed.
 * @return Boolean indicating whether or not CRC check passed.
 *****************************************************************************/
Boolean_t SerialNumberRead(uint8_t **data);

/**************************************************************************//**
 * @brief LPTBWriteRecord:
 *
 * This function writes a fan trim balance record to the persistent data
 * section of NVM.
 *
 * @param record: The record data to write.
 * @param size:   The size of the data to write, in bytes.
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t LPTBWriteRecord(uint8_t *record, uint32_t size);

/**************************************************************************//**
 * @brief LPTBGetRecordIterator:
 *
 * This function initializes an iterator that will iterate over records stored
 * in the fan trim balance section.
 *
 * @param iter: A pointer to an iterator to initialize.
 *****************************************************************************/
void LPTBGetRecordIterator(NVMIterator *iter);

/**************************************************************************//**
 * @brief LPTBRecordIteratorNext:
 *
 * This function increments the record pointed to by a fan trim balance record
 * iterator.
 *
 * @param iter: A pointer to an iterator whose record is to be incremented
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t LPTBRecordIteratorNext(NVMIterator *iter);

/**************************************************************************//**
 * @brief LPTBDump:
 *
 * This function dumps all fan trim balance data.
 *
 * @param[out] data: A pointer to the pointer where the starting fan trim
 *                   balance data will be placed.
 * @param[out] size: A pointer where the size of the fan trim balance data will
 *                   be placed.
 *****************************************************************************/
void LPTBDump(uint8_t **data, uint32_t *size);

/* ************************************************************************//**
 * @brief LPTBBufferSize
 *
 * Returns the size of the LPTB buffer.
 *
 * @return the size of the LPTB buffer
 *****************************************************************************/
uint8_t LPTBBufferSize(void);

/**************************************************************************//**
 * @brief LPTBHalted:
 *
 * This function returns true if the LPTB record buffer has been halted due to
 * a fault.  When this occurs, the application may examine fault codes and take
 * appropriate action.  Afterwards, the application can restart the buffer by
 * calling LPTBResume.
 *
 * @return A boolean indicating whether or not the LPTB record buffer has
 *         halted.
 *****************************************************************************/
Boolean_t LPTBHalted(void);

/**************************************************************************//**
 * @brief LPTBErasing
 *
 * This function returns true if the LPTB record buffer is currently erasing a
 * NOR sector.
 *
 * @return A boolean indicating whether or not the LPTB record buffer is
 *         erasing.
 *****************************************************************************/
Boolean_t LPTBErasing(void);

/**************************************************************************//**
 * @brief LPTBResume
 *
 * This function resumes LPTB record buffer operations.
 *****************************************************************************/
void LPTBResume(void);

/**************************************************************************//**
 * @brief SLOGWriteRecord:
 *
 * This function writes a status log record to the persistent data section of
 * NVM.
 *
 * @param record: The record data to write.
 * @param size:   The size of the data to write, in bytes.
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t SLOGWriteRecord(uint8_t *record, uint32_t size);

/**************************************************************************//**
 * @brief SLOGGetRecordIterator:
 *
 * This function initializes an iterator that will iterate over records stored
 * in the status log section.
 *
 * @param iter: A pointer to an iterator to initialize.
 *****************************************************************************/
void SLOGGetRecordIterator(NVMIterator *iter);

/**************************************************************************//**
 * @brief SLOGRecordIteratorNext:
 *
 * This function increments the record pointed to by a status log record
 * iterator.
 *
 * @param iter: A pointer to an iterator whose record is to be incremented
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t SLOGRecordIteratorNext(NVMIterator *iter);

/**************************************************************************//**
 * @brief SLOGDump:
 *
 * This function dumps all status log data.
 *
 * @param data: A pointer to the pointer where the starting status log
 *                   data will be placed.
 * @param size: A pointer where the size of the status log data will be
 *                   placed.
 *****************************************************************************/
void SLOGDump(uint8_t **data, uint32_t *size);

/* ************************************************************************//**
 * @brief SLOGBufferSize
 *
 * Returns the size of the SLOG buffer.
 *
 * @return the size of the SLOG buffer
 *****************************************************************************/
uint8_t SLOGBufferSize(void);

/**************************************************************************//**
 * @brief SLOGHalted:
 *
 * This function returns true if the SLOG record buffer has been halted due to
 * a fault.  When this occurs, the application may examine fault codes and take
 * appropriate action.  Afterwards, the application can restart the buffer by
 * calling SLOGResume.
 *
 * @return A boolean indicating whether or not the SLOG record buffer has
 *         halted.
 *****************************************************************************/
Boolean_t SLOGHalted(void);

/**************************************************************************//**
 * @brief SLOGErasing
 *
 * This function returns true if the SLOG record buffer is currently erasing a
 * NOR sector.
 *
 * @return A boolean indicating whether or not the SLOG record buffer is
 *         erasing.
 *****************************************************************************/
Boolean_t SLOGErasing(void);

/**************************************************************************//**
 * @brief SLOGResume
 *
 * This function resumes SLOG record buffer operations.
 *****************************************************************************/
void SLOGResume(void);

/**************************************************************************//**
 * @brief FRECWriteRecord:
 *
 * This function writes a fault record to the persistent data section of NVM.
 *
 * @param record: The record data to write.
 * @param size:   The size of the data to write, in bytes.
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t FRECWriteRecord(uint8_t *record, uint32_t size);

/**************************************************************************//**
 * @brief FRECGetRecordIterator:
 *
 * This function initializes an iterator that will iterate over records stored
 * in the fault record section.
 *
 * @param iter: A pointer to an iterator to initialize.
 *****************************************************************************/
void FRECGetRecordIterator(NVMIterator *iter);

/**************************************************************************//**
 * @brief FRECRecordIteratorNext:
 *
 * This function increments the record pointed to by a fault record iterator.
 *
 * @param iter: A pointer to an iterator whose record is to be incremented
 *
 * @return true if no faults occurred during the operation, false otherwise.
 *****************************************************************************/
Boolean_t FRECRecordIteratorNext(NVMIterator *iter);

/**************************************************************************//**
 * @brief FRECDump:
 *
 * This function dumps all fault record data.
 *
 * @param[out] data: A pointer to the pointer where the starting fault record
 *                   data will be placed.
 * @param[out] size: A pointer where the size of the fault record data will be
 *                   placed.
 *****************************************************************************/
void FRECDump(uint8_t **data, uint32_t *size);

/* ************************************************************************//**
 * @brief FRECBufferSize
 *
 * Returns the size of the FREC buffer.
 *
 * @return the size of the FREC buffer
 *****************************************************************************/
uint8_t FRECBufferSize(void);

/**************************************************************************//**
 * @brief FRECHalted:
 *
 * This function returns true if the FREC record buffer has been halted due to
 * a fault.  When this occurs, the application may examine fault codes and take
 * appropriate action.  Afterwards, the application can restart the buffer by
 * calling FRECResume.
 *
 * @return A boolean indicating whether or not the FREC record buffer has
 *         halted.
 *****************************************************************************/
Boolean_t FRECHalted(void);

/**************************************************************************//**
 * @brief FRECErasing
 *
 * This function returns true if the FREC record buffer is currently erasing a
 * NOR sector.
 *
 * @return A boolean indicating whether or not the FREC record buffer is
 *         erasing.
 *****************************************************************************/
Boolean_t FRECErasing(void);

/**************************************************************************//**
 * @brief FRECResume
 *
 * This function resumes FREC record buffer operations.
 *****************************************************************************/
void FRECResume(void);

/* ************************************************************************//**
 * @brief Calibrate Pressure Transducer Module:
 *
 * This function initiates the calibration process for the pressure sensors. The
 * OS initializes the FPGA to select real physical pressure on startup, therefore the zero
 * volt calibration input and the half volt calibration input signals in the received
 * buffer will remain stale until this function is called.
 *
 * When this function is called, the OS instructs the FPGA to first select the
 * zero volt calibration input, followed by the half volt calibration input and
 * then back to the real sensor input. It takes a couple of FPGA cycles for the ADC
 * output to settle after each transition, therefore the first 20 packets in the buffer
 * following a transition may exhibit fluctuations. The entire process is expected to 
 * take 6 OS cycles (This may be tightened up depending on test results).
 *
 * During the calibration process, the AS should inspect the pressure buffer obtained
 * via the GetCurrentADCPtmBuf APT to identify the transitions in the data in order 
 * to use settled data for computation of the compensation parameters.
 * See (JEMU/0325) for the compensation algorithm.
 *
 * Below is an example usage of this API for a 1 second calibration rate:
 *
 *		#define PRES_CALIBRATION_RATE  (39)
 *
 * 		static uint32_t cycleCount = 0;
 *
 * 		cycleCount++;
 * 		if(cycleCount > PRESS_CALIBRATION_RATE){
 * 			if(CalibratePTM() == PRES_CAL_COMPLETE){
 * 				cycleCount = 0;
 * 			}
 * 			//AS updates compensation parameters here
 * 		}
 *
 * 		//AS uses updated compensation parameters to correct real pressure input
 *
 * PRES_CALIBRATION_RATE must be equivalent to AT LEAST 7 OS cycles.
 * @return PRES_CAL_IDLE = Calibration Process Idle
 * @return PRES_CAL_BUSY = Calibration Process Running
 * @return PRES_CAL_COMPLETE = Calibration Process Completed
 */
// ****************************************************************************
uint32_t CalibratePTM(void);

/* ************************************************************************//**
 * @brief Get Pressure Calibration Table:
 *
 * This function copies calibration data stored in non-volatile memory of a
 * given pressure sensor into RAM and returns a pointer to the data.
 * The coefficients are stored contiguously in RAM and should be
 * accessed as a 2-D array. The values are store in row-major format in memory
 * and should be accessed as in the follow example:
 *
 *		float32_t* p_k; // coefficients
 *		uint32_t coeffElements;
 *		PressInfo_t P160Metadata;
 *		uint32_t* p_rawNVMData;
 *		uint32_t nvmRawSizeBytes;
 *
 *		float32_t p160psi = 0.0;
 *
 *		uint32_t status = getPTMCoeff(PRES_160, p_k,
 *					coeffElements, P160Metadata, p_rawNVMData, nvmRawSizeBytes);
 *
 *		if(status == PRES_CAL_GOODDATA){
 *			for i = 0 to 3 {
 *				for j = 0 to 4 {
 *					p160psi = p160psi + p_k[i,j] * p160mV^i * t160mV^j
 *				}
 *			} 	
 *		}
 *
 *		p160mv & t160mv are compensated pressure and temperature signals. see (JEMU/0325)
 *		for the compensation algorithm.
 *
 * The OS requests the NVM data during initialization but the FPGA begins
 * transmission when interrupts are enabled, therefore this API may return
 * PRES_CAL_NODATA for the first few cycles after initialization.
 *
 * @param  pressureType Selected Pressure Sensor (PRESS_P26, PRESS_P160)
 * @param  pp_pressureCoeff Pointer to Sensor Calibration Coefficient Buffer
 * @param  p_coeffLenElements Number of Elements in the Coefficient Buffer
 * @param  p_sensorInfo Pointer to Pressure Sensor Information
 * @param  pp_rawNVMData Pointer to The Raw NVM Data
 * @param  p_nvmDataLenBytes Size of The RAW NVM Data in Bytes
 *
 * @return PRES_CAL_GOODDATA = Good Data Read(Checksum passed)
 * @return PRES_CAL_BADDATA = Bad Data Read(Checksum failed)
 * @return PRES_CAL_NODATA = No Data Read
 * @return PRES_CAL_INVALID_PRES_TYPE = Invalid Pressure Type
 */
// ****************************************************************************
uint32_t
GetPTMCoeff(uint32_t pressureType, float32_t** pp_pressureCoeff, uint32_t* p_coeffLenElements,
		PressInfo_t* p_sensorInfo, uint32_t** pp_rawNVMData, uint32_t* p_nvmDataLenBytes);

/* ************************************************************************//**
 * @brief Returns a pointer to a buffer containing pressure and temperature samples.
 *
 * Each 100 micro seconds, the control FPGA samples data from two pressure transducer
 * sensors (P26 and P160) and creates a 17-word packet (Packet # 2) containing real physical sensor 
 * and simulated pressure and temperature samples. Over 25.6 milliseconds, 256 packets
 * are buffered and sent to the DSP via and interrupt. This API returns a pointer to 
 * the buffer. 
 *
 * The age of the samples in the packets depends on whether the OS is performing 
 * sensor calibration sequence or in normal operation mode. During normal operation,
 * real sensor data will be refreshed each 25.6 milliseconds, however, the zero volt 
 * calibration input and the half volt calibration input signals will be stale.
 * During the calibration sequence, the zero volt calibration input and the half volt
 * calibration input signals (0v first then 0.5v) will refresh, while the real sensor 
 * data samples remain stale. The OS calibration takes at least 6 cycles to complete.
 *
 * NOTE: The data in this buffer is only guaranteed for the current scheduling
 * period.
 *
 * Detailed packet information is documented in CIM Control FPGA Hardware Software 
 * Interface Control Document (JEMU/0325).
 *
 * @return Pointer to 256 Pressure and Temperature Samples from Pressure Sensors.
 * Note: can return NULL (when data is not available for processing)
 */
// ****************************************************************************
DataPkt2_t*
GetCurrentADCPtmBuf(void);

/* ************************************************************************//**
 * @brief   Set Once-Per-Revolution Threshold:
 *
 * If necessary, the AS may adjust the OPR threshold from its default value
 * using this function. The threshold as a percent of amplitude is calculated
 * using the formula (OPRValue + 1)/32
 *
 * @param  OPRValue - 5 bit integer
 *
 * @return (True = Threshold set,
 *          False = Error)
 */
// ****************************************************************************
Boolean_t
SetOPRThreshold(uint32_t OPRValue);

#endif //PLA_OS_API_Headers_H
