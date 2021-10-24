import shiftpi.shiftpi as SP
import time
import RPi.GPIO as GPIO
import pygame
import sys
import os.path

#      :   E   D   C   F   G   B   A
seg = [
    [ -1,  1,  2,  3,  4, -1,  6,  7 ],  # 0
    [ -1, -1, -1,  3, -1, -1,  6, -1 ],  # 1
    [ -1,  1,  2,  3, -1,  5,  6,  7 ],  # 2
    [ -1, -1,  2,  3, -1,  5,  6,  7 ],  # 3
    [ -1, -1, -1,  3,  4,  5,  6, -1 ],  # 4
    [ -1, -1,  2,  3,  4,  5, -1,  7 ],  # 5
    [ -1,  1,  2,  3,  4,  5, -1,  7 ],  # 6
    [ -1, -1, -1,  3, -1, -1,  6,  7 ],  # 7
    [ -1,  1,  2,  3,  4,  5,  6,  7 ],  # 8
    [ -1, -1,  2,  3,  4,  5,  6,  7 ]]  # 9

SP.shiftRegisters(4)

digit = [1, 5, 0, 0]

butPin = 18

pressFlag = False
GPIO.setwarnings(False)
GPIO.setup(butPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# for audio

sound_path = "/home/pi/sounds/"

sound_files = [[]]
sound_files.append([])  # make it 2 dimensional

agv_num = len(sys.argv)

# add any user files
if agv_num >= 3:
    s_m, s_s = sys.argv[1].split(':', 1)  # first get the end time
    i_m = int(s_m)
    i_s = int(s_s)
    stop_time = i_m * 60 + i_s
    # python no-default-wav.py MM:SS min:sec:sound1.wav min:sec:sound2.wav etc
    for item in range(2, agv_num):
        s_user_m, s_user_s, s_file = sys.argv[item].split(':', 2)
        s_user = s_user_m + ":" + s_user_s
        sound_files[0].append(s_user)
        sound_files[1].append(sound_path + s_file)
#        print "user_m: %d user_s: %d sound: %s" % (user_m, user_s, s_file)
else:
    print "usage: no-default-wav.py MM:SS MM:SS:file1.wav [... MM:SS:fileX.wav]"
    sys.exit("requires time (min:sec) and at least 2 sound files")

# shorten the agv counter
agv_num = agv_num - 2

missingFile = []

# check to make sure the files exist.
allOk = True
for i in range(0, agv_num):
    tmp_file = sound_files[1][i]
    if not os.path.isfile(tmp_file):
        missingFile.append(tmp_file)
        allOk = False

if not allOk:
    if len(missingFile) > 1:
        st = "files are"
    else:
        st = "file is"
    for i in range(0, len(missingFile)):
        print "%s" % (missingFile[i])
    sys.exit("the above " + st + " missing")

# setup the mixer
freq = 44100   # audio CD quality
bitsize = -16  # unsigned 16 bit
channels = 2   # 1 is mono, 2 is stero
buffer = 1024  # number of samples (experiment to get right sound)
pygame.mixer.init(freq, bitsize, channels, buffer)


def buzzer(index):
    pygame.mixer.music.load(sound_files[1][index])
    pygame.mixer.music.play(1)
    pygame.mixer.music.set_volume(0.8)


# debounce button
def wait_for_keydown(pin):
    while GPIO.input(pin):
        time.sleep(0.01)


def wait_for_keyup(pin):
    while not GPIO.input(pin):
        time.sleep(0.01)


def ledFunc(digit):
    ledClear()
    for col in range(0, 4):
        for i in range(0, 8):
            if seg[digit[col]][i] != -1:
                SP.digitalWrite((31 - seg[digit[col]][i]) - (col * 8), SP.HIGH)


def ledClear():
    SP.digitalWrite(SP.ALL, SP.LOW)


def resetDigit():
    m = i_m
    s = i_s
    digit[0], digit[1] = divmod(m, 10)
    digit[2], digit[3] = divmod(s, 10)


try:
    while True:
        resetDigit()
        ledFunc(digit)
        wait_for_keydown(butPin)
        wait_for_keyup(butPin)
        pressFlag = True
        if pressFlag:
            when_to_stop = stop_time
            while when_to_stop > -1:
                m, s = divmod(when_to_stop, 60)
                digit[0], digit[1] = divmod(m, 10)
                digit[2], digit[3] = divmod(s, 10)
                ledFunc(digit)
                for i in range(0, agv_num):
                    # FIXME we shouldn't have to do this a second time...
                    st_m, st_s = sound_files[0][i].split(':', 1)
                    user_m = int(st_m)
                    user_s = int(st_s)
                    if m == user_m and s == user_s:
                        buzzer(i)
                time.sleep(1)
                when_to_stop -= 1
            pressFlag = False
            ledClear()
except KeyboardInterrupt:
    ledClear()
    pass
ledClear()
