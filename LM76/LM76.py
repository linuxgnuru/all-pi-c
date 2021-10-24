#!/usr/bin/python
import smbus

class LM76():
    bus = smbus.SMBus(1)
    address = 0x48 
    def __init__(self,add):
        self.address = add
        self.setConf(0x01)
        self.setConf(0x00)
        return
    def getTemp(self):
        temp = self.bus.read_word_data(self.address,0x00)
        tmpsign = (temp&0x0080)>>7
        print tmpsign
        temp = temp&0xFF7F
        print temp
        temp =((-1)**tmpsign)*(((((temp&0x007F)<<8)|((temp&0xFF00)>>8))>>3)*0.0625)
        return temp
    def getThys(self):
        thys = self.bus.read_word_data(self.address,0x02)             
        tmpsign = (thys&0x0080)>>7
        thys = thys&0xFF7F
        thys =((-1)**tmpsign)*(((((thys&0x007F)<<8)|((thys&0xFF00)>>8))>>3)*0.0625)
        return thys
    def getTcrit(self):
        tcrit = self.bus.read_word_data(self.address,0x03)
        tmpsign = (tcrit&0x0080)>>7
        tcrit = tcrit&0xFF7F
        tcrit =((-1)**tmpsign)*(((((tcrit&0x007F)<<8)|((tcrit&0xFF00)>>8))>>3)*0.0625)
        return tcrit
    def getTlow(self):
        tlow = self.bus.read_word_data(self.address,0x04)
        tmpsign = (tlow&0x0080)>>7
        tlow = tlow&0xFF7F
        tlow =((-1)**tmpsign)*(((((tlow&0x007F)<<8)|((tlow&0xFF00)>>8))>>3)*0.0625)
        return tlow
    def getThigh(self):
        thigh = self.bus.read_word_data(self.address,0x05)
        tmpsign = (thigh&0x0080)>>7
        thigh = thigh&0xFF7F
        thigh =((-1)**tmpsign)*(((((thigh&0x007F)<<8)|((thigh&0xFF00)>>8))>>3)*0.0625)
        return thigh
    def getConf(self):
        conf  = self.bus.read_byte_data(self.address,0x01)
        return bin(conf)
    def setConf(self,conf):
        self.bus.write_byte_data(self.address,0x01,conf)
        return
    def setThys(self,thys):
        self.bus.write_word_data(self.address,0x02,thys)
        return
    def setTcrit(self,tcrit):
        self.bus.write_word_data(self.address,0x03,tcrit)
        return
    def setTlow(self,tlow):
        self.bus.write_bus_data(self.address,0x04,tlow)
        return
    def setThigh(self,thigh):
        self.bus.write_bus_data(self.address,0x05,thigh)
        return

def testSensor():
    sensor = LM76(0x48)
    print "Accessing LM76 at address 0x48"
    print "Temperature: " + `sensor.getTemp()`
    print "Thyst: " + `sensor.getThys()`
    print "Tcrit: " + `sensor.getTcrit()`
    print "Tlow: " + `sensor.getTlow()`
    print "Thigh: " + `sensor.getThigh()`
testSensor()
