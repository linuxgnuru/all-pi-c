import time
import sys
import subprocess
import re
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

RST = None     # on the PiOLED this pin isnt used

disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST)

# Initialize library.
disp.begin()

# Clear display.
disp.clear()
disp.display()

# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
width = disp.width
height = disp.height
image = Image.new('1', (width, height))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)

# Draw a black filled box to clear the image.
draw.rectangle((0,0,width,height), outline=0, fill=0)

# Draw some shapes.
# First define some constants to allow easy resizing of shapes.
padding = -2
top = padding
bottom = height-padding
# Move left to right keeping track of the current x position for drawing shapes.
x = 0

# Load default font.
font = ImageFont.load_default()

# Alternatively load a TTF font.  Make sure the .ttf font file is in the same directory as the python script!
# Some other nice fonts to try: http://www.dafont.com/bitmap.php
# font = ImageFont.truetype('Minecraftia.ttf', 8)

def ResetView():
    draw.rectangle((0, 0, width, height), outline=0, fill=0)
    top = -2
    x = 0

b = False
while True:
    draw.text((x, top), str('> '),  font=font, fill=255)
    disp.image(image)
    disp.display()
    try:
        s = raw_input('> ')
    except:
        break

    top += 8
    if top >= 32:
        top = -2
        ResetView()
    draw.text((x, top + 8), str(s),  font=font, fill=255)
    disp.image(image)
    disp.display()
    if s.strip().lower() == 'clear':
        b = True
        top = -2
        ResetView()
    if s.strip().lower() == 'exit':
        break

    top += 8
    if top >= 32:
        ResetView()
    try:
        if b == False:
            cmd = subprocess.Popen(re.split(r'\s+', s), stdout=subprocess.PIPE)
            cmd_out = cmd.stdout.read()
            # print output
            draw.text((x, top), str(cmd_out),  font=font, fill=255)
            disp.image(image)
            disp.display()
        else:
            b = False
#print cmd_out
    except OSError:
#print 'invalid command'
        draw.text((x, top + 8), str('invalid command'), font=font, fill=255)
        disp.image(image)
        disp.display()
    top += 8
    if top >= 32:
        ResetView()
