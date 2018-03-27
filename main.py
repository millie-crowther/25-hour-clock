import time
import sys
import itertools
from nanpy import SerialManager

# update rate for clock state
rate = 0.5  

connection = None

def setup():
    port = '/dev/serial0'
    global connection
    connection = SerialManager(device=port, baudrate=9600)

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
    h = str((t / 3600) % 24).rjust(2, '0')
    m = str((t / 60) % 60).rjust(2, '0')
    s = str(t % 60).rjust(2, '0')

    return [int(n) for n in h + m + s]

def get_led_map(digits):
    # creates a mapping from LEDs on the display to whether or not they are active
    l = lambda digit:  {
        'A' : digit in [0, 2, 3, 5, 6, 7, 8, 9],
        'B' : digit in [0, 1, 2, 3, 4, 7, 8, 9],
        'C' : digit in [0, 1, 3, 4, 5, 6, 7, 8, 9],
        'D' : digit in [0, 2, 3, 5, 6, 8, 9],
        'E' : digit in [0, 2, 6, 8],
        'F' : digit in [0, 4, 5, 6, 8, 9],
        'G' : digit in [2, 3, 4, 5, 6, 8, 9]
    }

    return [l(d) for d in digits]

def transmit_to_arduino(led_maps):
    for leds in led_maps:
        for (led, value) in leds.items():
            print('writing \'' + str(value) + '\' to connection')
            connection.write(value)
            print connection.readline()

def main():
    t0 = start_time('--load' in sys.argv)
 
    setup()

    while True:
        t = int(time.time()) - t0
        digits = get_digits(t)
        leds = get_led_map(digits)

        print digits
        print leds

        transmit_to_arduino(leds)
        


        time.sleep(rate)

if __name__ == "__main__":
    main()
