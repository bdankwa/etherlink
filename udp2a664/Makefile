#/*
# ***************************************************************************************************
# ** 				Copyright 2016 Rolls-Royce Controls and Data Services Limited.
# ** 	The information in this document is the property of Rolls-Royce Controls and Data Services
# ** 		Limited and may not be copied or communicated to a third party, or used for
# ** 	any purpose other than for which it was supplied without the express written consent of
# ** 						Rolls-Royce Controls and Data Services Limited.
# ***************************************************************************************************/
 
all: udp2a664

clean:
	rm Debug/udp2a664

udp2a664: udp2a664.c
	gcc -g -o Debug/udp2a664 udp2a664.c

install: udp2a664
	sudo cp Debug/udp2a664 /usr/local/bin

