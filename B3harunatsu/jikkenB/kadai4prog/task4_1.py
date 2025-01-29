import mouse 
import serial
import time

mouse.FAILSAFE = False
ArduinoSerial = serial.Serial('COM3',9600)
time.sleep(1)

while 1:
    data = ArduinoSerial.readline().decode('utf-8').rstrip()
    x_vector, y_vector = map(int , data.split(","))
    (x,y) = mouse.get_position()
    mouse.move(x+x_vector,y-y_vector)