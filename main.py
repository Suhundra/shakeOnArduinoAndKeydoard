import keyboard
import serial

port = serial.Serial('COM4', 115200) #налаштування порту та швидкість
def sendData(data):
    strData = str(data)
    port.write(strData.encode())
    #print(port.read().decode().replace('/n', '').replace('/r', ''))

required_keys = ['up', 'down', 'right', 'left']

key_states = {key: False for key in required_keys}

while True:
    for key in required_keys:
        if keyboard.is_pressed(key) and not key_states[key]:
            key_states[key] = True
            print('Key pressed:', key)
            
            if key == "up":
                sendData(0)
            if key == "down":
                sendData(1)
            if key == "right":
                sendData(2)
            if key == "left":
                sendData(3)
        
        if not keyboard.is_pressed(key) and key_states[key]:
            key_states[key] = False

    if keyboard.is_pressed('esc'):
        break

port.close()
print("Port clossed!")


