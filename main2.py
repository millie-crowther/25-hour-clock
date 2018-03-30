import time
import os
import serial
import sys

# update rate for clock state
rate = 10 

cmd = '~/arduino-1.8.5/arduino --upload --port /dev/ttyACM0 ~/25-hour-clock/arduino_code2/arduino_code2.ino'
os.system(cmd)

connection = serial.Serial('/dev/ttyACM0', 9600, timeout=0.050)

current_state = ['NULL'] * 256

def start_time(from_disk):
    # returns the time the clock started

    # try loading from disk
    if from_disk:
        try:
            f = open('start_time.txt', 'r') 
            result = int(f.readlines()[0].strip())
            f.close()
            return result
        except:
            pass

    # start clock from now as fallback
    result = int(time.time())
    f = open('start_time.txt', 'w')
    f.write(str(result))    
    f.close()
    return result

def main():
    t0 = start_time('--load' in sys.argv)
 
    while True:
        t = int(time.time()) - t0
        connection.write(str(t) + '.')
       
        for i in range(10):
            time.sleep(rate / 10) 
            while connection.inWaiting() > 0:
                print connection.readline()
        
        time.sleep(rate)

if __name__ == "__main__":
    main()
