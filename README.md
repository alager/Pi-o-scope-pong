Fun little project to make pong using a Raspberry Pi and and oscilloscope set to XY mode.
It uses an 8 bit MAX506 DAC that I had left over from college ( over 20 years ago, but it still works great! )

Build:  gcc -Wall -pthread -o xy xy.c -lpigpio -lrt -lm <br>
Run:  sudo ./xy

There are lots of improvements that can be made:<br>
1. Finsh score keeping<br>
2. Change the angle of ball delfection based on where the ball hits the paddle<br>
3. Add sound for ball bouncing, etc.

![image of game](https://raw.github.com/alager/Pi-o-scope-pong/blob/master/Screenshot_20201018-192314.png")
