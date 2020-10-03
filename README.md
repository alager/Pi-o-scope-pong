Fun little project to make pong using a Raspberry Pi and and oscilloscope set to XY mode.
It uses an 8 bit MAX506 DAC that I had left over from college ( over 20 years ago, but it still works great! )

Build:  gcc -Wall -pthread -o xy xy.c -lpigpio -lrt -lm <br>
Run:  sudo ./xy
