##########################################################
######                                              ######
######      AWFUL STUPID FUCKING FOX DESK PEST      ######
######                                              ######
##########################################################

# this script generates the imagedata.h file with the pixels in one big array
# the generated file is pretty heavy but it gets compressed down in compilation

import webp
from PIL import Image, ImageSequence


anim = webp.load_images("animation.webp")

outstr = "unsigned char data[] = {"

for f in anim:
	outstr += "\n0x"

	width, height = f.size
	outbytes = bytes()
	for y in range(height-1, -1, -1):
		for x in range(width-1, -1, -1):
			outbytes += bytes(f.getpixel((x, y)))

	outstr += outbytes.hex("_", 1)
	outstr += ","

for f in ImageSequence.Iterator(Image.open("explosion.gif")):

	print("herre")
	outstr += "\n0x"

	width, height = f.size
	outbytes = bytes()
	f = f.convert("RGBA")
	for y in range(height-1, -1, -1):
		for x in range(width-1, -1, -1):
			outbytes += bytes(f.getpixel((x, y)))
	outstr += outbytes.hex("_", 1)
	outstr += ","
				

outstr = outstr.replace("_", ", 0x")
outstr += "};" 

with open("pixeldata.h", "w") as outf:
	outf.write(outstr)
