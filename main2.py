import time
import os
import serial
import sys

hours_doc = 24
minutes_doc = 0
seconds_doc = 0

rate = 60 
cmd1 = '~/arduino-1.8.5/arduino --upload --port /dev/ttyACM0 ~/25-hour-clock/arduino_code2/arduino_code2.ino'
cmd2 = '~/arduino-1.8.5/arduino --upload --port /dev/ttyACM0 ~/25-hour-clock/arduino_doc/arduino_doc.ino'
cmd = cmd2 if ('--doc' in sys.argv) else cmd1
os.system(cmd)

connection = serial.Serial('/dev/ttyACM0', 9600, timeout=0.050)
current_state = ['NULL'] * 256

def t2():
    return hours_doc*60*60 + minutes_doc*60 + seconds_doc

def start_time(live_doc):
    if live_doc:
        return int(time.time()) - t2()
    else: 
        return 1522886400 -3600

def main():
    t0 = start_time('--live-doc' in sys.argv)
    s = time.time()
    isDoc = '--doc' in sys.argv

    while True:
        #t = (int(time.time()) - t0)       
        t = t2() if isDoc else (int(time.time()) - t0)
        connection.write(str(t) + '.')
        print "sending " + str(t) + ". to arduino"

        if time.time() - s < 15:
            print "short gap to improve sync"
            time.sleep(2)
        else:
            print "normal gap"
            time.sleep(rate)

        while connection.inWaiting() > 0:
            print connection.readline()

if __name__ == "__main__":
    main()
