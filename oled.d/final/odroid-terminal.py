import time
import sys
import subprocess
import re
import termios
import tty
import time
import os

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
    sys.stdout.write('> ')
    sys.stdout.flush()

button_delay = 0.1

prompt()

br = False
line = ""

while br == False:
    char = getch()
    if char != '\r':
        if char != '\b':
            line += char
            sys.stdout.write(char)
            sys.stdout.flush()
    else:
        print ''
#        print "> [%s]" % (line)
        if line.strip().lower() == 'exit':
            br = True
        else:
            try:
                cmd = subprocess.Popen(re.split(r'\s+', line), stdout=subprocess.PIPE)
                cmd_out = cmd.stdout.read()
                print cmd_out
            except OSError:
                print 'invalid command'
            line = ""
            prompt()
