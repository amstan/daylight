#!/usr/bin/env python

import serial
import readline

def parse_colour(string):
	if string[0]=="#":
		#hex style
		string=string[1:]
		if len(string)==3:
			colour=map(lambda x: int(x,16)*0x11,string)
		if len(string)==6:
			colour=map(lambda x: int(x,16)*0x11,(string[0:2],string[2:4],string[4:6]))
	else:
		string=string.split(",")
		colour=map(int,string)
	return colour

s=serial.Serial("/dev/ttyUSB0",9600,timeout=0.1)
s.close()
s.open()

def set_colour(colour):
	bytes=''.join(map(chr,colour))
	print "sending colour:%s bytes:%dx %s" % (colour,len(bytes),bytes)
	s.write(bytes)
	s.flushOutput()

while(1):
	colour=parse_colour(raw_input("colour? "))
	errored=True
	while errored:
		try:
			set_colour(colour)
			errored=False
		except Exception:
			pass