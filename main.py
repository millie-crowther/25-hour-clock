import time
import sys
import itertools
from nanpy import ArduinoApi
from nanpy import SerialManager
import RPi.GPIO as GPIO

# update rate for clock state
rate = 0.5  

# pin mapping
#   + first index is digit number: hours = {0, 1}, minutes = {2, 3}, seconds = {4, 5}
#   + letters correspond to LEDs as defined in data sheet
#   + numbers correspond to pin outputs
#   + 'B' LED omitted on digit 0 since it is always on
#   + 'D' LED omitted on digits 0, 2 and 4 since it is equivalent to 'A' LED
pin_map = {
    # arduino pins
    0 : { 'A' : 4, 'C' : 5, 'E' : 6, 'F' : 7, 'G' : 8 },
    1 : { 'A' : 9, 'B' : 10, 'C' : 11, 'D' : 12, 'E' : 13, 'F' : 14, 'G' : 15 },

    # pi pins
    2 : { 'A' : 0, 'B' : 1, 'C' : 2, 'E' : 3, 'F' : 4, 'G' : 5 },
    3 : { 'A' : 6, 'B' : 7, 'C' : 8, 'D' : 9, 'E' : 10, 'F' : 11, 'G' : 12 },
    4 : { 'A' : 13, 'B' : 14, 'C' : 15, 'E' : 16, 'F' : 17, 'G' : 18 },
    5 : { 'A' : 19, 'B' : 20, 'C' : 21, 'D' : 22, 'E' : 23, 'F' : 24, 'G' : 25 }
}

arduino = None

def setup():
    # set up arduino
    usb_port = '/dev/ttyUSB0'
    c = SerialManager(usb_port)
    global arduino 
    arduino = ArduinoApi(connection = c)

    for digit in [0, 1]:
        for (led, pin) in pin_map[digit].items():
            arduino.pinMode(pin, arduino.OUTPUT)
   
    # set up raspberry pi
    GPIO.setmode(GPIO.BCM)

    for digit in [2, 3, 4, 5]:
        for (led, pin) in pin_map[digit].items():
            GPIO.setup(pin, GPIO.OUT)

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

def output_to_pins(led_maps):
    for (digit, leds) in pin_map.items():
        for (led, pin) in leds.items():
            value = led_maps[digit][led]
            
            if digit in [0, 1]:
                # output to arduino
                v = arduino.HIGH if value else arduino.LOW
                arduino.digitalWrite(pin, v)
            else:
                # output to raspberry pi 
                GPIO.output(pin, value)

def main():
    t0 = start_time('--load' in sys.argv)
 
    setup()

    while True:
        t = int(time.time()) - t0
        digits = get_digits(t)
        leds = get_led_map(digits)

        print digits
        print leds

        output_to_pins(leds)

        time.sleep(rate)

if __name__ == "__main__":
    main()
