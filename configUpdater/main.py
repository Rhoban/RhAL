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
                                  "angleLimitCWParameter" : self.minAngle,
                                  "angleLimitCCWParameter" : self.maxAngle 
                                  }
                  }
        return output

def main(xmlFile):
    index = xmlFile.rfind(".")
    if (index < 0) :
        fileName = "output"
    else :
        fileName = xmlFile[:index]
        
    #Parsing the xml file
    dom = parse(xmlFile)
    listOfServos = []
    for node in dom.getElementsByTagName('ServoConfig'):  # visit every node <bar />
        idServo =  int(node.getElementsByTagName('Id')[0].childNodes[0].nodeValue)
        name =  node.getElementsByTagName('Name')[0].childNodes[0].nodeValue
        zero = float(node.getElementsByTagName('ZeroAngle')[0].childNodes[0].nodeValue)
        inverted = node.getElementsByTagName('Inverse')[0].childNodes[0].nodeValue
        minAngle = float(node.getElementsByTagName('MinAngle')[0].childNodes[0].nodeValue)
        maxAngle = float(node.getElementsByTagName('MaxAngle')[0].childNodes[0].nodeValue)
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
            "IMU": {
                "devices": [
                    {
                        "id": 241,
                        "name": "IMU_1",
                        "parameters": "null"
                    }
            ],
            "parameters": "null"
    },
            "Protocol": {
                "timeout": 0.01
            },
            "Dynaban64" : {
               "devices" : [],
               "parameters" : "null"
            },
             "RX28" : {
               "devices" : [],
               "parameters" : "null"
            },
              "RX24" : {
                "devices" : [],
                "parameters" : "null"
            }
     }
    
    for servo in listOfServos :
        name = servo.name
        if ("ankle" in name) or ("hip" in name) or ("knee" in name) :
            #The servo will be considered Dynaban64
            header["Dynaban64"]["devices"].append(servo.toMap())
        elif ("head" in name) :
            #The servo will be considered RX24
            header["RX24"]["devices"].append(servo.toMap())
        else :
            #The others are considered to be RX28
            header["RX28"]["devices"].append(servo.toMap())
    
    result = json.dumps(header, sort_keys=True, indent=4, separators=(',', ': '))
    #Replacing "true" with true and co
    result = result.replace("\"true\"", "true")
    result = result.replace("\"false\"", "false")
    result = result.replace("\"null\"", "null")
    print result

    outputFile = fileName + '.json'
    print "Writing at ", outputFile
    f = open(outputFile,'w')
    f.write(result)
    f.close()

if ( __name__ == "__main__"):
    print("A new day dawns")
    main(sys.argv[1])
    print("Done !")
