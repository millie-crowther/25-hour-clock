import time
import os
import serial
import sys

# update rate for clock state
rate = 0.5  

cmd1 = '~/arduino-1.8.5/arduino --upload --port /dev/ttyACM0 ~/25-hour-clock/arduino_code/arduino_code.ino'
cmd2 = '~/arduino-1.8.5/arduino --upload --port /dev/ttyACM1 ~/25-hour-clock/arduino_code/arduino_code.ino'
cmd3 = '~/arduino-1.8.5/arduino --upload --port /dev/ttyACM2 ~/25-hour-clock/arduino_code/arduino_code.ino'
os.system(cmd1)
os.system(cmd2)
os.system(cmd3)

connections = [
    serial.Serial('/dev/ttyACM0', 9600, timeout=0.050),
    serial.Serial('/dev/ttyACM1', 9600, timeout=0.050), # TODO: check ports
    serial.Serial('/dev/ttyACM2', 9600, timeout=0.050)
]

for i in range(3):
    connections[i].write(chr(((i * 2) << 5) | 2))

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

def get_digits(t):
    # converts uptime to an array of digits
    h = str((t / 3600) % 25).rjust(2, '0')
    m = str((t / 60) % 60).rjust(2, '0')
    s = str(t % 60).rjust(2, '0')

    return [int(n) for n in h + m + s]

def get_led_map(digits):
    # creates a mapping from LEDs on the display to whether or not they are active
    l = lambda digit:  {
        0 : digit in [0, 2, 3, 5, 6, 7, 8, 9],
        1 : digit in [0, 1, 2, 3, 4, 7, 8, 9],
        2 : digit in [0, 1, 3, 4, 5, 6, 7, 8, 9],
        3 : digit in [0, 2, 3, 5, 6, 8, 9],
        4 : digit in [0, 2, 6, 8],
        5 : digit in [0, 4, 5, 6, 8, 9],
        6 : digit in [2, 3, 4, 5, 6, 8, 9]
    }

    return [l(d) for d in digits]

def transmit_to_arduino(led_maps):
    for (digit, leds) in enumerate(led_maps):
        for (led, value) in leds.items():
            if not (digit == 0 and led == 1 or led == 3 and digit in [0, 2, 4]):
                byte = int(value)
                byte |= digit << 5
                byte |= led << 2

                connections[digit / 2].write(chr(byte))

def main():
    t0 = start_time('--load' in sys.argv)
 
    while True:
        t = int(time.time()) - t0
        digits = get_digits(t)
        leds = get_led_map(digits)

        print digits
        print leds

        transmit_to_arduino(leds)
        
        while connection.inWaiting() > 0:
            print connection.readline()
        
        time.sleep(rate)

if __name__ == "__main__":
    main()
