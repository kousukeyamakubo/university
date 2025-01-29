import mouse 
import serial
import time
import numpy as np
import pyautogui 

mouse.FAILSAFE = False
ArduinoSerial = serial.Serial('COM3',9600)
time.sleep(1)

back_interval = 1
last_back_time = 0
forward_interval = 1
last_forward_time = 0
previous_right_button = 1
previous_left_button = 1

while 1:
    data = ArduinoSerial.readline().decode('utf-8').rstrip()
    values = data.split(",")
    if len(values) != 6:
        print(f"Unexpected data format: {data}")
        continue
    left_x_vector, left_y_vector ,right_x_vector ,right_y_vector ,left_button ,right_button = map(int , data.split(","))
    
    (mouse_x,mouse_y) = mouse.get_position()
    mouse.move(mouse_x+left_x_vector,mouse_y-left_y_vector)

    if right_button == 0 and previous_right_button == 1:
        mouse.click('left')
        
    
    if left_button == 0 and previous_left_button == 1:
        mouse.click('right')

    if right_y_vector != 0:
        wheel_speed = np.interp(right_y_vector,(-10,10),(-1,1))
        mouse.wheel(wheel_speed)

    if right_x_vector < -5:
        current_back_time = time.time()
        if current_back_time - last_back_time >= back_interval:
            pyautogui.hotkey('alt', 'left')
            last_back_time = current_back_time

    if right_x_vector > 5:
        current_forward_time = time.time()
        if current_forward_time - last_forward_time >= forward_interval:
            pyautogui.hotkey('alt', 'right')
            last_forward_time = current_forward_time
    
    
    previous_right_button = right_button
    previous_left_button = left_button
