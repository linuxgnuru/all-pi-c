from RPi import GPIO
from luma.core.interface.serial import i2c
from luma.core.render import canvas
from luma.oled.device import ssd1306, ssd1325, ssd1331, sh1106
from time import sleep
import socket

try:
	import psutil
except ImportError:
	print "the psutil library was not found"
	sys.exit()

dt = 13 #23 A
clk = 16 #27 B
sw = 19 #24
global menuindex
global insubmenu

GPIO.setmode(GPIO.BCM)
GPIO.setup(clk, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(dt, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(sw, GPIO.IN, pull_up_down=GPIO.PUD_UP)
serial = i2c(port=1, address=0x3C)
device = ssd1306(serial, rotate=0)

clkLastState = GPIO.input(clk)

from PIL import ImageFont, ImageDraw
from luma.core.interface.serial import i2c
from luma.core.render import canvas
from luma.oled.device import ssd1306, ssd1325, ssd1331, sh1106
import time

serial = i2c(port=1, address=0x3C)
device = ssd1306(serial, rotate=0)
global gdraw, gdevice

def ip_address():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = (s.getsockname()[0])
    s.close()
    return str(ip)

def invert(draw,x,y,text):
    font = ImageFont.load_default()
    draw.rectangle((x, y, x+120, y+10), outline=255, fill=255)
    draw.text((x, y), text, font=font, outline=0,fill="black")
	
# Box and text rendered in portrait mode
def menu(device, draw, menustr,index):
    global menuindex
    font = ImageFont.load_default()
    draw.rectangle(device.bounding_box, outline="white", fill="black")
    for i in range(len(menustr)):
        if( i == index):
            menuindex = i
            invert(draw, 2, i*10, menustr[i])
        else:
		    draw.text((2, i*10), menustr[i], font=font, fill=255)
		
		
names = ['Disk', 'Memory', 'Network', 'CPUUsage', 'IPAddress', 'CODELECTRON']
with canvas(device) as draw:
    menu(device, draw, names,1)
#	

def bytes2human(n):
    """
    >>> bytes2human(10000)
    '9K'
    >>> bytes2human(100001221)
    '95M'
    """
    symbols = ('K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y')
    prefix = {}
    for i, s in enumerate(symbols):
        prefix[s] = 1 << (i + 1) * 10
    for s in reversed(symbols):
        if n >= prefix[s]:
            value = int(float(n) / prefix[s])
            return '%s%s' % (value, s)
    return "%sB" % n

def disk_usage(dir):
    usage = psutil.disk_usage(dir)
    return "SD:  %s %.0f%%" \
        % (bytes2human(usage.used), usage.percent)

def menu_operation(strval):
    if ( strval == "CODELECTRON"):
        with canvas(device) as draw:
            draw.rectangle(device.bounding_box, outline="white", fill="black")
            draw.text((10, 20), "Thank you", fill="white")
            draw.text((10, 30), "Keep following", fill="white")
    if ( strval == "Disk"):
        with canvas(device) as draw:
            draw.text((0, 26), disk_usage('/'), fill="white")
    if ( strval == "Memory"):
        with canvas(device) as draw:
            draw.text((0, 26), sys_info.mem_usage(), fill="white")
    if ( strval == "Network"):
        with canvas(device) as draw:
            draw.text((0, 26), sys_info.network('wlan0'), fill="white")
    if ( strval == "CPUUsage"):
        with canvas(device) as draw:
            draw.text((0, 26), sys_info.cpu_usage(), fill="white")
    if ( strval == "IPAddress"):
        with canvas(device) as draw:
            draw.text((0, 26), ip_address(), fill="white")

def sw_callback(channel):  
    global menuindex
    global insubmenu

    strval = names[menuindex]
    menu_operation(strval)


def rotary_callback(channel):  
    global clkLastState
    global counter
    try:
                clkState = GPIO.input(clk)
                if clkState != clkLastState:
                        dtState = GPIO.input(dt)
                        if dtState != clkState:
                                counter += 1
                        else:
                                counter -= 1
                        print counter
                        with canvas(device) as draw:
                            menu(device, draw, names,counter%7)
                clkLastState = clkState
    finally:
                print "Ending"


counter = 0
insubmenu = 0
clkLastState = GPIO.input(clk)
GPIO.add_event_detect(clk, GPIO.FALLING , callback=rotary_callback, bouncetime=100)  
GPIO.add_event_detect(sw, GPIO.FALLING , callback=sw_callback, bouncetime=300)  
raw_input("Enter anything")
GPIO.cleanup()

