import time
import sys
import eyed3
import os.path
import pygame
import lirc
import threading
import random

import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

import subprocess

song_list = []

curSongIndex = 0

exitFlag = 0
RST = None     # on the PiOLED this pin isnt used
disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST)
disp.begin()
width = disp.width
height = disp.height
disp.clear()
disp.display()
image = Image.new('1', (width, height))
font = ImageFont.load_default()
draw = ImageDraw.Draw(image)
draw.rectangle((0, 0, width, height), outline=0, fill=0)
padding = -2
top = padding
bottom = height-padding
x = 0
isPaused = False
NewSong = False

def main(song_list):
    thread1.start()

#def initMixer():
#    pygame.mixer.init()
#	BUFFER = 3072  # audio buffer size, number of samples since pygame 1.8.
#    FREQ, SIZE, CHAN = pygame.mixer.get_init()
#	pygame.mixer.init(FREQ, SIZE, CHAN, BUFFER)

class myThread (threading.Thread):
    def __init__(self, threadID, name, counter):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name
        self.counter = counter

    def run(self):
        player(song_list, 0.1, len(song_list), self.name)

#class pauseMusic(object):
#    def __init__(self):
#        self.paused = pygame.mixer.music.get_busy()
#
#    def toggle(self):
#        if self.paused:
#            pygame.mixer.music.unpause()
#        if not self.paused:
#            pygame.mixer.music.pause()
#        self.paused = not self.paused

#PAUSE = pauseMusic()

def pauseMusic():
    global isPaused
    if isPaused == True:
        print "unpaused"
        pygame.mixer.music.unpause()
    else:
        print "paused"
        pygame.mixer.music.pause()
    isPaused = not ispaused

def ReadFiles():
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

def prevSong():
    global curSongIndex
    global NewSong
    NewSong = True
    if curSongIndex > 1:
        curSongIndex = curSongIndex - 1
    st = "prev song"
    draw.text((x, top+50), str(st), font=font, fill=255)
    print "prev song"

def nextSong():
    global curSongIndex
    global NewSong
    NewSong = True
    curSongIndex = curSongIndex + 1
    st = "next song"
    draw.text((x, top+50), str(st), font=font, fill=255)
    print "next song"

def volUp():
    v = pygame.mixer.music.get_volume()
    if v < 1:
        pygame.mixer.music.set_volume(v + 0.1)
    subprocess.Popen(['/usr/local/bin/f2', (str(v))]).wait()
    draw.text((x, top+40), str(v), font=font, fill=255)
    print "vol up"

def volDown():
    v = pygame.mixer.music.get_volume()
    if v > 0:
        pygame.mixer.music.set_volume(v - 0.1)
    subprocess.Popen(['/usr/local/bin/f2', (str(v))]).wait()
    draw.text((x, top+40), str(v), font=font, fill=255)
    print "vol dn"

def stopMusic():
    pygame.mixer.music.stop()

def player(song_list, delay, counter, threadName):
    global curSongIndex
    global NewSong
    while counter:
        if exitFlag:
            threadName.exit()
        time.sleep(delay)
        if pygame.mixer.music.get_busy() == False:
            GetTag(song_list[curSongIndex])
            pygame.mixer.music.load(song_list[curSongIndex])
            pygame.mixer.music.play()
            counter = counter - 1
            curSongIndex = curSongIndex + 1
        while pygame.mixer.music.get_busy() == True:
            try:
                received = lirc.nextcode()
            except Exception as e:
                time.sleep(3)
            else:
                if received:
                    key_pressed(received[0])
                    if NewSong == True:
                        stopMusic()
                        NewSong = False
            time.sleep(delay)
            continue

#def playMusic(soundfile):
#    clock = pygame.time.Clock()
#    pygame.mixer.music.load(soundfile)
#    pygame.mixer.music.play()
#    while pygame.mixer.music.get_busy():
#        clock.tick(1000)

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

    draw.text((x, top), str(title), font=font, fill=255)
    draw.text((x, top+10), str(artist), font=font, fill=255)
    draw.text((x, top+20), str(album), font=font, fill=255)
    draw.text((x, top+30), str(track_num), font=font, fill=255)
    disp.image(image)
    disp.display()
    disp.scrollHorizontal(0x00, 0x02)

def key_pressed(key):
    global NewSong
    if key == 'KEY_PLAYPAUSE':
        pauseMusic()
#        PAUSE.toggle()
    elif key == 'KEY_VOLUMEUP':
        volUp()
    elif key == 'KEY_VOLUMEDOWN':
        volDown()
    elif key == 'KEY_PREVIOUS':
        prevSong()
    elif key == 'KEY_NEXT':
        nextSong()
#elif key == 'KEY_F1':
#    elif key == 'KEY_F2':
    elif key == 'KEY_CHANNEL':
        NewSong = True
        random.shuffle(song_list)
    elif key == 'KEY_MODE':
#execute(['sudo', 'shutdown', '-h', 'now'])
        subprocess.Popen(['sudo halt']).wait()
        lirc.deinit()
        sys.exit(0)
#    elif key == 'KEY_CHANNELDOWN':
#do something
#    elif key == 'KEY_CHANNELUP':
#do something
#    elif key in ('KEY_0', 'KEY_1', 'KEY_2', 'KEY_3', 'KEY_4', 'KEY_5', 'KEY_6', 'KEY_7', 'KEY_8', 'KEY_9'):
#do something
#    else:
#do something

agv_num = len(sys.argv)

if agv_num == 1:
    sys.exit("usage: " + sys.argv[0] + " songs")

ReadFiles()

pygame.init()
pygame.mixer.init()
#initMixer()

thread1 = myThread(1, "PlayerThread", 1)
lirc.init('mp3_lirc', os.path.join('/etc/', 'play-mp3-lirc.conf'))

# set volume LEDs
pygame.mixer.music.set_volume(0.1)
v = pygame.mixer.music.get_volume()
subprocess.Popen(['/usr/local/bin/f2', (str(v))]).wait()

main(song_list)

#try:
#except KeyboardInterrupt:	# to stop playing, press "ctrl-c"
#    stopMusic()

#    for i in range(0, len(song_list)):
#        song = song_list[i]
#        GetTag(song)
