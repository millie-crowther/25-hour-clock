import time
import sys

# update rate for clock state
rate = 0.45

# pin mapping
#   + first index is digit number: hours = {0, 1}, minutes = {2, 3}, seconds = {4, 5}
#   + letters correspond to LEDs as defined in data sheet
#   + numbers correspond to GPIO pins on Pi
pin_map = {
    0 : { 'A' : 0, 'B' : 0, 'C' : 0, 'D' : 0, 'E' : 0, 'F' : 0, 'G' : 0 },
    1 : { 'A' : 0, 'B' : 0, 'C' : 0, 'D' : 0, 'E' : 0, 'F' : 0, 'G' : 0 },
    2 : { 'A' : 0, 'B' : 0, 'C' : 0, 'D' : 0, 'E' : 0, 'F' : 0, 'G' : 0 },
    3 : { 'A' : 0, 'B' : 0, 'C' : 0, 'D' : 0, 'E' : 0, 'F' : 0, 'G' : 0 },
    4 : { 'A' : 0, 'B' : 0, 'C' : 0, 'D' : 0, 'E' : 0, 'F' : 0, 'G' : 0 },
    5 : { 'A' : 0, 'B' : 0, 'C' : 0, 'D' : 0, 'E' : 0, 'F' : 0, 'G' : 0 }
}

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

def get_pin_map(led_map):
    # returns a full pin map for all digits this tick
    result = {}
    for digit in range(6):
        for led, value in led_map[digit].items():
            result[pin_map[digit][led]] = value
    return result

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

def main():
    t0 = start_time('--load' in sys.argv)

    while True:
        t = int(time.time()) - t0
        digits = get_digits(t)
    
        print digits
        print get_led_map(digits)

        time.sleep(rate)

if __name__ == "__main__":
    main()
