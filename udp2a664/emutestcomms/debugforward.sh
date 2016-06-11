#!/bin/sh
# Forward EMU debug messages to mcmfarm3
socat UDP-LISTEN:20002,bind=192.168.10.3,reuseaddr,fork udp:192.168.35.150:20002
