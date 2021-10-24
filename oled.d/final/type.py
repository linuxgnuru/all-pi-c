import time
import sys
import subprocess
import re
import termios
import tty
import time
import os
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

RST = None

disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST)

disp.begin()

disp.clear()
disp.display()

width = disp.width
height = disp.height
image = Image.new('1', (width, height))

draw = ImageDraw.Draw(image)

draw.rectangle((0,0,width,height), outline=0, fill=0)

global padding
global top
global bottom
global x
global b
padding = -2
top = padding
bottom = height-padding
x = 12

font = ImageFont.load_default()

def ResetView():
    disp.clear()
    disp.display()
    top = -2
    prompt()

def getch():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

def prompt():
    draw.text((0, top), str('> '),  font=font, fill=255)
    disp.image(image)
    disp.display()

def WriteText(mx, my, txt):
    draw.text((mx, my), str(txt),  font=font, fill=255)
    disp.image(image)
    disp.display()

def WrapText(txt):
    mx = x
    my = top
    mtxt = txt
    while len(mtxt >= 20):
        if len(mtxt) >= 20:
            a = mtxt[0:20]
            WriteText(mx, my, a)
            b = mtxt[21:len(mtxt)]
            mtxt = b
            mx = 0
            my += 8
            if my >= 64:
                my = -2

button_delay = 0.1

prompt()

br = False
line = ""

while br == False:
    char = getch()
    if char != '\r':
        if char != '\b':
            line += char
            draw.text((x, top), str(char),  font=font, fill=255)
            disp.image(image)
            disp.display()
#print "x top", x, top
            x += 6
            if x >= 120:
                x = 0
                top += 8
                if top >= 64:
                    top = -2
            time.sleep(button_delay)
    else:
        if line.strip().lower() == 'clear':
            ResetView()
        if line.strip().lower() == 'exit':
            br = True
            break
        top += 8
        if top >= 64:
            top = -2
        x = 12
        line = ""
        prompt()
