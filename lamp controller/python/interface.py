#!/usr/bin/env python

from time import sleep
import serial
import readline
import random

def mousepos():
	import Xlib.display # python-xlib
	data = Xlib.display.Display().screen().root.query_pointer()._data
	return data["root_x"], data["root_y"]

def get_pixel_colour(i_x, i_y):
	import PIL.Image # python-imaging
	import PIL.ImageStat # python-imaging
	import Xlib.display # python-xlib
	o_x_root = Xlib.display.Display().screen().root
	o_x_image = o_x_root.get_image(i_x, i_y, 1, 1, Xlib.X.ZPixmap, 0xffffffff)
	o_pil_image_rgb = PIL.Image.fromstring("RGB", (1, 1), o_x_image.data, "raw", "BGRX")
	lf_colour = PIL.ImageStat.Stat(o_pil_image_rgb).mean
	return tuple(map(int, lf_colour))

def parse_colour(string):
	if string[0]=="#":
		#hex style
		string=string[1:]
		if len(string)==3:
			colour=map(lambda x: int(x,16)*0x11,string)
		if len(string)==6:
			colour=map(lambda x: int(x,16),(string[0:2],string[2:4],string[4:6]))
	else:
		string=string.split(",")
		colour=map(int,string)
	return colour

s=serial.Serial("/dev/ttyUSB0",19200,timeout=0.1)
s.close()
s.open()

nowcolour=(0,0,0)
steps=16

def set_colour(colour):
	bytes="%.2x%.2x%.2x\n" % tuple(map(int,colour))
	print bytes
	s.write(bytes)
	s.flushOutput()

def fade(tocolour):
	global nowcolour
	for i in xrange(steps):
		colour=[]
		for c in xrange(3):
			colour.append(((i)/float(steps))*tocolour[c]+((steps-i)/float(steps))*nowcolour[c])
		set_colour(colour)
		sleep(0.5)
	set_colour(tocolour)
	nowcolour=tocolour

colours="""#DCDCDC
#E49B0F
#F8F8FF
#6082B6
#D4AF37
#FFD700
#996515
#FCC200
#FFDF00
#DAA520
#A8E4A0
#808080
#465945
#00FF00
#008000
#00A550
#66B032
#ADFF2F
#A99A86
#00FF7F""".split("\n")

while(1):
	for colour in colours:
		set_colour(parse_colour(raw_input("Colour? ")))
		#raw_input()
		#sleep(0.5)