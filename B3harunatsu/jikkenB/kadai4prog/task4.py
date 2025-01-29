import serial
import time
import pyautogui
import math
import mouse

def calculatespeed(x_vector, y_vector):
    min_speed = 0
    count = 0
    length = math.sqrt(x_vector**2 + y_vector**2)
    while 1:
        length = length - 50
        count = count + 1
        if length < 0:
            count = count - 1
            break
    speed = min_speed + 5 * count
    return speed

def getmouseposition():
    x, y = pyautogui.position()
    return x, y

# Arduinoとのシリアル通信設定
ser = serial.Serial('COM3', 9600)  # COMポートとボーレートをArduinoに合わせて設定
time.sleep(2)
current_x, current_y = mouse.get_position()
screen_width, screen_height = pyautogui.size()
while True:
    if ser.in_waiting > 0:
        try:
            raw_data = ser.readline()  # 生データを読み取る
            data = raw_data.decode('utf-8').rstrip()  # 改行を除去してデコード
            x_vector, y_vector = map(int, data.split(","))
            

            # 画面端に到達している場合は動作を停止
            if (current_x <= 0 and x_vector < 0) or (current_x >= screen_width and x_vector > 0):
                continue
            if (current_y <= 0 and y_vector < 0) or (current_y >= screen_height and y_vector > 0):
                continue

            # マウスを移動させる
            mouse.move(current_x+x_vector,current_y+y_vector)

        except UnicodeDecodeError as e:
            print(f"Decoding error: {e}")
        except Exception as e:
            print(f"Unexpected error: {e}")
