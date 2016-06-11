#!/bin/sh
socat UDP-LISTEN:20002,bind=192.168.10.3 - | od -x
