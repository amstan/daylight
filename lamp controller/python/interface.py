#!/usr/bin/env python

from time import sleep
import serial
import readline
import random
from colour import rgb, hsv, mix

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

s=serial.Serial("/dev/ttyUSB0",19200,timeout=0.1)
s.close()
s.open()

nowcolour=rgb.from_string("red")

def set_colour(colour):
	bytes="%.2x%.2x%.2x\n" % tuple(map(int,colour))
	print bytes
	s.write(bytes)
	s.flushOutput()

def fade(tocolour,time=1):
	tocolour=tocolour
	delay=0.01
	steps=time/delay
	global nowcolour
	for i in xrange(steps):
		set_colour(mix(nowcolour,tocolour,i/steps))
		sleep(delay)
	set_colour(tocolour)
	nowcolour=tocolour

while(1):
	try:
		fade(rgb.from_string(raw_input("Colour> ")),2)
	except Exception as e:
		print e