import serial

ser = serial.Serial()
ser.baudrate = 19200
ser.port = 'COM3'
print(ser)
ser.open()
print(ser.is_open)
ser.close()
print(ser.is_open)