import time
import sys
import eyed3
import os.path

import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

import subprocess

def play_mp3(path):
    subprocess.Popen(['mpg123', '-q', path]).wait()

def ResetView():
    global x
    global top
    x = 0
    top = -2
    disp.clear()
    disp.display()
    draw.rectangle((0, 0, width, height), outline=0, fill=0)
    disp.image(image)
    disp.display()

def GetTag(songfile):
    global top
    audiofile = eyed3.load(songfile)
    artist = audiofile.tag.artist + "] ["
    album = audiofile.tag.album + "] ["
    title = audiofile.tag.title + "] ["
    tmptrack_num = audiofile.tag.track_num
    track_num_a = tmptrack_num[0]
    track_num_b = tmptrack_num[1]
    if track_num_a:
        if track_num_b:
            track_num = str(track_num_a) + '/' + str(track_num_b)
        else:
            track_num = str(track_num_a)
    else:
        track_num = ''
    ResetView()

#    text = 'abcdefghijklmnopqrstuvwxyz'
#    maxwidth, unused = draw.textsize(text, font=font)

#    WrapText(0, top, str(title))
#    top += 8
#    WrapText(0, top, str(artist))
#    top += 8
#    WrapText(0, top, str(album))
#    top += 8
#    WrapText(0, top, str(track_num))
    draw.text((x, top), str(title), font=font, fill=255)
    draw.text((x, top+9), str(artist), font=font, fill=255)
    draw.text((x, top+17), str(album), font=font, fill=255)
    draw.text((x, top+27), str(track_num), font=font, fill=255)
    disp.image(image)
    disp.display()
    disp.scrollHorizontal(0x00, 0x02)
    play_mp3(songfile)

RST = None     # on the PiOLED this pin isnt used
disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST)
disp.begin()

# Get display width and height.
width = disp.width
height = disp.height

# Clear display.
disp.clear()
disp.display()

# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
image = Image.new('1', (width, height))

# Load default font.
font = ImageFont.load_default()

# Alternatively load a TTF font.  Make sure the .ttf font file is in the same directory as this python script!
# Some nice fonts to try: http://www.dafont.com/bitmap.php
# font = ImageFont.truetype('Minecraftia.ttf', 8)

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)

# Draw a black filled box to clear the image.
draw.rectangle((0, 0, width, height), outline=0, fill=0)

# Draw some shapes.
# First define some constants to allow easy resizing of shapes.
padding = -2
top = padding
bottom = height-padding
# Move left to right keeping track of the current x position for drawing shapes.
x = 0

agv_num = len(sys.argv)

if agv_num == 1:
    sys.exit("usage: " + sys.argv[0] + " songs")
# Draw a black filled box to clear the image.
draw.rectangle((0, 0, width, height), outline=0, fill=0)

song_list = []

for i in range(1, agv_num):
    tmpthing = sys.argv[i]
    if os.path.isfile(tmpthing):
        song_list.append(tmpthing)
    else:
        if os.path.isdir(tmpthing):
            for subdir, dirs, files in os.walk(tmpthing):
                for file in files:
                    tmptwo = os.path.join(subdir, file)
                    song_list.append(tmptwo)
for i in range(0, len(song_list)):
    song = song_list[i]
    GetTag(song)
#    pygame.mixer.music.load(song)
#    pygame.mixer.music.play()
#play_mp3(song)
#draw.text((x, top),       "IP: " + str(IP),  font=font, fill=255)
#draw.text((x, top+8),     str(CPU), font=font, fill=255)
#draw.text((x, top+16),    str(MemUsage),  font=font, fill=255)
#draw.text((x, top+25),    str(Disk),  font=font, fill=255)

# Display image.
#time.sleep(.1)

