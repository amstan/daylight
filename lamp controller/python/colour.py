#!/usr/bin/env python

from colourtable import colourtable
import colorsys

class rgb(tuple):
	def __new__(cls,r,g,b):
		if r<0:
			r=0
		if r>255:
			r=255
		if g<0:
			g=0
		if g>255:
			g=255
		if b<0:
			b=0
		if b>255:
			b=255
		return tuple.__new__(cls,(r,g,b))
	
	@classmethod
	def from_string(cls,string):
		colour=None
		
		if string[0]=="#":
			#hex
			string=string[1:]
			if len(string)==3:
				colour=map(lambda x: int(x,16)*0x11,string)
			
			if len(string)==6:
				colour=map(lambda x: int(x,16),(string[0:2],string[2:4],string[4:6]))
		else:
			try:
				#colour name
				return cls.from_string(colourtable[string])
			except KeyError:
				#comma separated bytes
				parts=string.split(",")
				if len(parts)==3:
					colour=map(int,parts)
		if colour==None:
			raise ValueError("Could not determine colour from '%s'." % string)
		return cls(*colour)
	
	def __str__(self):
		return "#%.2X%.2X%.2X" % self
	
	def __repr__(self):
		return type(self).__name__ + tuple.__repr__(self)
	
	def to_hsv(self):
		h,s,v=colorsys.rgb_to_hsv(*map(lambda byte: byte/255.0,self))
		return hsv(h*360,s*100,v*100)

class hsv(tuple):
	def __new__(cls,h,s,v):
		h%=360
		for byte in (s,v):
			if byte<0:
				byte=0
			if byte>100:
				byte=100
		return tuple.__new__(cls,(h,s,v))
	
	def __repr__(self):
		return type(self).__name__ + tuple.__repr__(self)
	
	def to_rgb(self):
		h,s,v=self
		return rgb(*map(lambda byte: byte*255,colorsys.hsv_to_rgb(h/360.0,v/100.0,s/100.0)))

def mix(colour1, colour2, value=0.5):
	if type(colour1)!=type(colour2):
		return ValueError("Cannot mix %s with %s" % (type(colour1),type(colour2)))
	return colour1.__class__(*(byte1*(1-value)+byte2*value for byte1, byte2 in zip(colour1, colour2)))