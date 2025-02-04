

import serial

ser = serial.Serial()
ser.baudrate = 115200
ser.port = 'COM3'
print(ser)
ser.open()
