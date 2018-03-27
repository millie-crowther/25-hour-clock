# 25-hour-clock

* install script 'arduino_install.sh' provided needs to be run on the arduino 
  * may need to edit 'nanpy-firmware/Nanpy/cfg.h' but should work fine 
* install script 'pi_install.sh' needs to be run on pi
* will then need to correctly set the USB port that the arduino is connected to
  * currently set to default value of '/dev/ttyUSB0' which *should* work fine
  * if it doesn't, run 'ls /dev/tty*' on command line to get the list of ports
  * choose correct port (should have USB in the name)
  * ctrl-f 'usb_port' in main.py to change the value of the port
* run 'python main.py'
* hopefully it doesnt blow up
* if the clock is interrupted and the program halts for any reason, run 'python main.py --load' to resume the clock from where you left off