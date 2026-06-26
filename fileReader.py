import serial
import time

ser = serial.Serial("COM5", 115200)

time.sleep(2)

## reads the lines in the file and sends it to the arduino serial

with open("paths/circle.txt", "r") as file:
    for line in file:
        ser.write(line.encode())   
        time.sleep(0.05)        
              
ser.close()