import time
import sys

rate = 0.4

def start_time(from_disk):
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

def get_uptime(start_time):
    # returns time since startup - possibly use more accurate internet clock?
    result = time.time()
    return int(result) - start_time

def get_digits(start_time):
    # converts uptime to an array of digits
    t = get_uptime(start_time)
    h = str((t / 3600) % 24)
    m = str((t / 60) % 60)
    s = str(t % 60)
    
    h = h.rjust(2, '0') 
    m = m.rjust(2, '0') 
    s = s.rjust(2, '0')

    result = [h[0], h[1], m[0], m[1], s[0], s[1]] 
    return [int(n) for n in result]

def bit_map(digits):
    # transforms full state of clock into nineteen-element binary vector
    bits_req = [2, 3, 3, 4, 3, 4]
    
    result = []
    for (i, digit) in enumerate(digits):
        result += [digit & (1 << shift) != 0 for shift in range(bits_req[i])]
    return result 

def led_map(digits):
    # creates a mapping from LEDs on the display to whether or not they are active
    l = lambda digit:  {
        'A' : digit in [0, 2, 3, 5, 6, 8, 9],
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
        digits = get_digits(t0)
        print digits
        print bit_map(digits)
        print led_map(digits)
        time.sleep(rate)

if __name__ == "__main__":
    main()
