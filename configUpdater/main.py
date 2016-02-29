# -*- coding: utf-8 -*-
#Author : Rémi Fabre

import sys

from xml.dom.minidom import parse, parseString
import xml.dom.minidom
import json


def sign(x) :
    if (x > 0) :
        return 1
    if (x < 0) :
        return -1
    return 1

def isNumber(value):
    try:
        float(value)
        return True
    except:
        return False

class Servo(object):
    def __init__(self, idServo, name, zero, inverted, minAngle, maxAngle) :
        self.id = idServo
        self.name = name
        self.zero = zero
        self.inverted = inverted
        self.minAngle = minAngle
        self.maxAngle = maxAngle

    def __repr__(self):
        output = []
        for key in self.__dict__:
            output.append("{key}='{value}'\n".format(key=key, value=self.__dict__[key]))
 
        return ', '.join(output)
    def toMap(self):
        output = {
                  "id" : self.id,
                  "name" : self.name,
                  "parameters" : {
                                  "inverse" : self.inverted,
                                  "zero" : self.zero,
                                  "cwAngleLimit" : self.minAngle,
                                  "ccwAngleLimit" : self.maxAngle 
                                  }
                  }
        return output

def main(xmlFile):
    #Parsing the xml file
    dom = parse(xmlFile)
    listOfServos = []
    for node in dom.getElementsByTagName('ServoConfig'):  # visit every node <bar />
        idServo =  node.getElementsByTagName('Id')[0].childNodes[0].nodeValue
        name =  node.getElementsByTagName('Name')[0].childNodes[0].nodeValue
        zero = node.getElementsByTagName('ZeroAngle')[0].childNodes[0].nodeValue
        inverted = node.getElementsByTagName('Inverse')[0].childNodes[0].nodeValue
        minAngle = node.getElementsByTagName('MinAngle')[0].childNodes[0].nodeValue
        maxAngle = node.getElementsByTagName('MaxAngle')[0].childNodes[0].nodeValue
        invertedBool = ""
        if inverted.strip() == "1" :
            invertedBool = "true"
        else :
            invertedBool = "false"
        servo = Servo(idServo, name, zero, invertedBool, minAngle, maxAngle)
        listOfServos.append(servo)
        
    #Creating the JSON file
    header = {
             "Manager": {
                "baudrate": 1000000.0,
                "enableSyncRead": "true",
                "enableSyncWrite": "true",
                "port": "/dev/ttyACM0",
                "protocol": "DynamixelV1",
                "scheduleMode": "true",
                "throwErrorOnRead": "true",
                "throwErrorOnScan": "true"
            },
            "Pins": {
                "devices": [
                    {
                        "id": 242,
                        "name": "Pins_1",
                        "parameters": "null"
                    }
                ],
                "parameters": "null"
            },
            "Protocol": {
                "timeout": 0.01
            },
            "Dynaban64" : {
               "devices" : []
            },
             "RX" : {
               "devices" : []
            }
     }
    
    for servo in listOfServos :
        name = servo.name
        if ("ankle" in name) or ("hip" in name) or ("knee" in name) :
            #The servo will be considerer Dynaban64
            header["Dynaban64"]["devices"].append(servo.toMap())
        else :
            #The others are considered to be RX
            header["RX"]["devices"].append(servo.toMap())
    
    print json.dumps(header, sort_keys=True, indent=4, separators=(',', ': '))

if ( __name__ == "__main__"):
    print("A new day dawns")
    main(sys.argv[1])
    print("Done !")
