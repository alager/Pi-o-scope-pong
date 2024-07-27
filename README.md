Fun little project to make pong using a Raspberry Pi and and oscilloscope set to XY mode.<br>
It uses an 8 bit MAX506 DAC that I had left over from college ( over 20 years ago, but it still works great! )<br>
https://www.maximintegrated.com/en/products/analog/data-converters/digital-to-analog-converters/MAX506.html<br>

The controls are from the two joystcks on an XBox One controller, connected via bluetooth to the Raspberry Pi.
At the time of development, it showed up as /dev/input/js0.  This can be modified in the header file to suit your own controller input.

Build:  gcc -Wall -pthread -o xy xy.c -lpigpio -lrt -lm <br>
Run:  sudo ./xy


There are lots of improvements that can be made:<br>
1. Finsh score keeping<br>
2. Change the angle of ball delfection based on where the ball hits the paddle<br>
3. Add sound for ball bouncing, etc.

![image of game](https://github.com/alager/Pi-o-scope-pong/blob/master/Screenshot_20201018-192314.png)
