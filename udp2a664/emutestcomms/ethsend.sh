#!/bin/bash
# This script emulates roughly the rate and type of EMU messages


function 50ms {
while :
do
	# 50ms
	xxd -r -p < EMU_TO_EEC_ACMS_P160_32024.txt > /dev/udp/127.0.0.1/22024
	xxd -r -p < EMU_TO_EEC_ACMS_P160_32024.txt > /dev/udp/127.0.0.1/32024
	sleep 0.05 
done
}

function 120ms {
while :
do
	# 120ms
	xxd -r -p < EMU_TO_EEC_MSG_6_32029.txt > /dev/udp/127.0.0.1/22029
	xxd -r -p < EMU_TO_EEC_MSG_6_32029.txt > /dev/udp/127.0.0.1/32029
	sleep 0.12
done
}


function 200ms {
while :
do
    # 200ms
	xxd -r -p < EMU_TO_EEC_MSG_1_32021.txt > /dev/udp/127.0.0.1/22021 
	xxd -r -p < EMU_TO_EEC_MSG_1_32021.txt > /dev/udp/127.0.0.1/32021 
	# 200ms
	xxd -r -p < EMU_TO_EEC_MSG_4_32027.txt > /dev/udp/127.0.0.1/22027
	xxd -r -p < EMU_TO_EEC_MSG_4_32027.txt > /dev/udp/127.0.0.1/32027
	# 200ms
	xxd -r -p < EMU_TO_EEC_MSG_5_32028.txt > /dev/udp/127.0.0.1/22028
	xxd -r -p < EMU_TO_EEC_MSG_5_32028.txt > /dev/udp/127.0.0.1/32028
	sleep 0.2
done
}

function 1000ms {
while :
do
	# 1000ms
	xxd -r -p < EMU_TO_EEC_ACMS_PRESSURES_AND_TEMPERATURES_32025.txt > /dev/udp/127.0.0.1/22025
	xxd -r -p < EMU_TO_EEC_ACMS_PRESSURES_AND_TEMPERATURES_32025.txt > /dev/udp/127.0.0.1/32025
	# 1000ms
	xxd -r -p < EMU_TO_EEC_MSG_3_32026.txt > /dev/udp/127.0.0.1/22026
	xxd -r -p < EMU_TO_EEC_MSG_3_32026.txt > /dev/udp/127.0.0.1/32026
	sleep 1
done
}


function 740ms {
while :
do
	# 740ms
	xxd -r -p < EMU_TO_EEC_ACMS_BROADBAND_32023.txt > /dev/udp/127.0.0.1/22023
	xxd -r -p < EMU_TO_EEC_ACMS_BROADBAND_32023.txt > /dev/udp/127.0.0.1/32023
	sleep 0.740
done
}


function 420ms {
while :
do

	# 420ms 
	xxd -r -p < EMU_TO_EEC_ACMS_VIBRATION_32022.txt > /dev/udp/127.0.0.1/22022 
	xxd -r -p < EMU_TO_EEC_ACMS_VIBRATION_32022.txt > /dev/udp/127.0.0.1/32022
	sleep 0.420
done
}





   1000ms &
   420ms &
   50ms &
   200ms &
   120ms &
   740ms &
   
   wait

