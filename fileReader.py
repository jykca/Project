import serial
import time

ser = serial.Serial("COM5", 9600)

time.sleep(2)

## reads the lines in the file and sends it to the arduino serial

with open("paths/circle.txt", "r") as file:
    for line in file:
        print("SEND:", line.strip())
        ser.write(line.encode())   

        time.sleep(0.05)        

        start = time.time()
        while time.time() - start < 0.3:
            msg = ser.readline().decode(errors="ignore").strip()
            if msg:
                print("ARDUINO:", msg)
              
ser.close()