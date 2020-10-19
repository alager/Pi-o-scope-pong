
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// this gives us access to the xbox controller
#include <linux/joystick.h>

// this gives us access to the gpio on the raspberry pi
#include "pigpio.h"

// this is our local header
#include "xy.h"

// globals for the xbox controller
// will need to do something else if I add another controller
int *axis = NULL;
char *button = NULL;
int num_of_axis = 0;

int main()
{
	int	 serveCount;
	bool serve;
	unsigned  paddlePosition1, paddlePosition2;
	circleLocation_t circleLoc;
	time_t t;

	if ( gpioInitialise() < 0 )
	{
	   // pigpio initialisation failed.
		printf( "initialisation failed.\n" );
		exit( 1 );
	}
	else
	{
	   // pigpio initialised okay.
		printf( "initialisation passed.\n" );
	}

	// configure the IO outputs
	gpioSetMode( D0, PI_OUTPUT );
	gpioSetMode( D1, PI_OUTPUT );
	gpioSetMode( D2, PI_OUTPUT );
	gpioSetMode( D3, PI_OUTPUT );
	gpioSetMode( D4, PI_OUTPUT );
	gpioSetMode( D5, PI_OUTPUT );
	gpioSetMode( D6, PI_OUTPUT );
	gpioSetMode( D7, PI_OUTPUT );
	gpioSetMode( WR, PI_OUTPUT );
	gpioSetMode( A0, PI_OUTPUT );
	gpioSetMode( A1, PI_OUTPUT );
	
	// close the data latch
	gpioWrite( WR, 1 );
	

	/* Intializes random number generator */
	srand((unsigned) time(&t));

	
	// initialize the xbox one controller
	int joy_fd, num_of_buttons=0;
	char  name_of_joystick[80];

	if( ( joy_fd = open( JOY_DEV , O_RDONLY)) == -1 )
	{
		printf( "Couldn't open joystick\n" );
		return -1;
	}

	ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
	ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
	ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );

	axis = (int *) calloc( num_of_axis, sizeof( int ) );
	button = (char *) calloc( num_of_buttons, sizeof( char ) );

	printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
		, name_of_joystick
		, num_of_axis
		, num_of_buttons );

	// use non-blocking mode 
	fcntl( joy_fd, F_SETFL, O_NONBLOCK );	

	// end of xbox one controller init


	// show a splash screen for a few seconds
	serveCount = SRV_DLY * 10;
	while( serveCount-- )
	{
		spashScreen();
		gpioDelay( 1200 );
	}


	// start the game off by serving
	serveCount = SRV_DLY;

	// *** Game super loop ***
	while(1)
	{
		// draw game outline
		drawRect( 0, 0, 255, 255 );

		paddlePosition1 = getPaddle( 0, joy_fd );
		//	printf( "paddle: %i\n", paddlePosition1 );
		drawPaddle1( paddlePosition1 );

		if( serveCount-- )
		{
			serve = true;
		}
		else
		{
			serve = false;
			serveCount = 0;
		}

		circleLoc = getBallLoc( serve );
		drawBall( circleLoc );

		paddlePosition2 = getPaddle( 1, joy_fd );
		//	printf( "paddle: %i\n", paddlePosition2 );
		drawPaddle2( paddlePosition2 );

		// check for miss
		if( checkMiss( &circleLoc, paddlePosition1, paddlePosition2 ) )
		{
			serveCount = SRV_DLY ;
		}

		// animation delay
		// TODO: have button presses increase/decrease this value
		gpioDelay( 1200 );
	}
}

void spashScreen()
{
	// P
	drawLine( 0, 200, 0, 255 );
	drawLine( 0, 255, 15, 240 );
	drawLine( 15, 240, 0, 230 ); 

	// i
	drawLine( 30, 200, 30, 220 );
	drawLine( 30, 235, 30, 245 );


	// P
	drawLine( 75, 200, 75, 255 );
	drawLine( 75, 255, 90, 240 );
	drawLine( 90, 240, 75, 230 ); 

	// o
	drawCircle( 105, 210, 10 );

	// n
	drawLine( 135, 200, 135, 220 );
	drawLine( 135, 220, 150, 220 );
	drawLine( 150, 220, 150, 200 );
	
	// g
	drawCircle( 175, 210, 10 );
	drawLine( 185, 210, 185, 185 );
	drawLine( 185, 185, 165, 185 );
}


// check to see if a paddle missed the ball
// returns true on miss
bool checkMiss( circleLocation_t * circleLoc, unsigned paddlePosition1, unsigned paddlePosition2 )
{
	if( circleLoc->x < ( PADDLE_1_X + BALL_RADIUS ) )
	{
		//printf( "(%d, %d)\t", circleLoc->x, PADDLE_1_X  );
		// check if the paddle is where the ball is
		unsigned paddleTop = paddlePosition1 + PADDLE_SIZE + BALL_RADIUS;
		if( paddleTop > 255 )
			paddleTop = 255;

		int paddleBottom = paddlePosition1 - BALL_RADIUS;
		if( paddleBottom < 0 )
			paddleBottom = 0;

		if( ( circleLoc->y > paddleTop ) || ( circleLoc->y <  paddleBottom ) ) 
		{
			//printf( "(%d, %d)\n", circleLoc->y, (paddlePosition1 + PADDLE_SIZE )  );
			circleLoc->player = PLAYER1;
			return true;
		}
		//printf("\n");
	}
	else
	if( circleLoc->x > ( PADDLE_2_X - BALL_RADIUS ) )
	{
		unsigned paddleTop = paddlePosition2 + PADDLE_SIZE + BALL_RADIUS;
		if( paddleTop > 255 )
			paddleTop = 255;

		int paddleBottom = paddlePosition2 - BALL_RADIUS;
		if( paddleBottom < 0 )
			paddleBottom = 0;

		if( ( circleLoc->y > paddleTop ) || ( circleLoc->y <  paddleBottom ) ) 
		{
			circleLoc->player = PLAYER2;
			return true;
		}
	}

	circleLoc->player = NONE;
	return false;
}


// returns the new xy location of the ball
circleLocation_t getBallLoc( bool serve )
{
	static unsigned directionX = 0;
	static unsigned directionY = 0;
	static int ballPositionX = 0;
	static int ballPositionY = 50;
	circleLocation_t ball;
	int tmp;
	
	if( serve )
	{
		// hold the ball in the middle of the screen
		ballPositionX = 128;
		ballPositionY = 128;

		// randomize the starting direction
		tmp = rand() % 2;
		directionX = tmp;
		tmp = rand() % 2;
		directionY = tmp;
	}
	else
	{
		tmp = rand() % 2;

		if( directionX )
		{
			ballPositionX += tmp;
			if( ballPositionX > BALL_MAX_X )
			{
				ballPositionX = BALL_MAX_X;
				directionX = 0;
			}
		}
		else
		{
			ballPositionX -= tmp;
			if( ballPositionX < BALL_MIN_X )
			{
				ballPositionX = BALL_MIN_X;
				directionX = 1;
			}
		}

		tmp = rand() % 2;

		if( directionY )
		{
			ballPositionY += tmp;
			if( ballPositionY > BALL_MAX_Y )
			{
				ballPositionY = BALL_MAX_Y;
				directionY = 0;
			}
		}
		else
		{
			ballPositionY -= tmp;
			if( ballPositionY < BALL_MIN_Y )
			{
				ballPositionY = BALL_MIN_Y;
				directionY = 1;
			}
		}
	}

	ball.x = ballPositionX;
	ball.y = ballPositionY;

	return ball;
}


void drawBall( circleLocation_t ball )
{
	drawCircle( ball.x, ball.y, BALL_RADIUS );
}


// returns the Y location of the id paddle
unsigned getPaddle( unsigned id, int joy_fd )
{
	static int paddlePosition1 = 0;
	static int paddlePosition2 = 0;
	int paddlePosition = 0;

	struct js_event js;

	// read xbox controller state
	read( joy_fd, &js, sizeof( struct js_event ) );

	switch (js.type & ~JS_EVENT_INIT)
	{
		case JS_EVENT_AXIS:
			axis   [ js.number ] = js.value;
			break;
		case JS_EVENT_BUTTON:
			button [ js.number ] = js.value;
			break;
	}

	// scale +/-32768 into +/- 2
	//	axis[0]
	int leftY = 0;// = ( axis[1] / ( 1024 * 8) );
	if( abs( axis[1] ) > 10000 )
		leftY = 2;

	// set the sign
	if( axis[1] < 0 )
		leftY *= -1;

	//printf( "axis[1]: %d\tleftY: %d\n", axis[1], leftY );

	//	axis[2]
	int rightY = 0;// = ( axis[3] / ( 1024 * 8) );
	if( abs( axis[3] ) > 10000 )
		rightY = 2;

	// set the sign
	if( axis[3] < 0 )
		rightY *= -1;

	//printf( "axis[3]: %d\trightY: %d\n", axis[3], rightY );

	// print the results 
	//printf( "X: %6d  Y: %6d  ", axis[0], leftY );
	//printf("Z: %6d  ", axis[2] );
	//printf("R: %6d  ", rightY );
		
	//	for( x=0 ; x<num_of_buttons ; ++x )
	//		printf("B%d: %d  ", x, button[x] );


	if( id == 0 )
	{
		paddlePosition = paddlePosition1;
		paddlePosition -= leftY;
	}
	else
	if( id == 1 )
	{
		paddlePosition = paddlePosition2;
		paddlePosition -= rightY;
	}

	//printf( "    pos: %d\r", paddlePosition );
	//fflush(stdout);

	// check for out of bounds and set the limit
	if( paddlePosition > 210 )
	{
		paddlePosition = 210;
	}

	if( paddlePosition < 1 )
	{
		paddlePosition = 1;
	}


/*
	// random paddle position
	tmp = rand() % 4;
	if( direction )
	{
		paddlePosition += tmp;
		if( paddlePosition > 210 )
		{
			paddlePosition = 210;
			direction = 0;
		}
	}
	else
	{
		paddlePosition -= tmp;
		if( paddlePosition < 1 )
		{
			paddlePosition = 1;
			direction = 1;
		}
	}
*/

	// put the paddle position into the static memory for next time
	if( id == 0 )
	{
		paddlePosition1 = paddlePosition;
	}
	else
	if( id == 1 )
	{
		paddlePosition2 = paddlePosition;
	}


	return paddlePosition;
}


void drawPaddle1( unsigned y )
{
	drawRect( PADDLE_1_X, y, PADDLE_1_X, ( y + PADDLE_SIZE ) );
}


void drawPaddle2( unsigned y )
{
	drawRect( PADDLE_2_X, y, PADDLE_2_X, ( y + PADDLE_SIZE ) );
}


// writes the 8bit value to the DAC on the selected channel.
void setData( unsigned data, unsigned xy )
{
	if( xy == X )
	{
		// select output C
		gpioWrite( A0, 0 );
		gpioWrite( A1, 1 );
	}
	else
	if( xy == Y )
	{
		// select output B
		gpioWrite( A0, 1 );
		gpioWrite( A1, 0 );
	}

	// clear the bits, then set the ones that need to be high
	gpioWrite( D0, 0 );
	gpioWrite( D1, 0 );
	gpioWrite( D2, 0 );
	gpioWrite( D3, 0 );
	gpioWrite( D4, 0 );
	gpioWrite( D5, 0 );
	gpioWrite( D6, 0 );
	gpioWrite( D7, 0 );
	
	// set the bits on the DAC
	if( data & 0x01 )
		gpioWrite( D0, 1 );
	if( data & 0x02 )
		gpioWrite( D1, 1 );
	if( data & 0x04 )
		gpioWrite( D2, 1 );
	if( data & 0x08 )
		gpioWrite( D3, 1 );
	if( data & 0x10 )
		gpioWrite( D4, 1 );
	if( data & 0x20 )
		gpioWrite( D5, 1 );
	if( data & 0x40 )
		gpioWrite( D6, 1 );
	if( data & 0x80 )
		gpioWrite( D7, 1 );
	
	
	// toggle the WR pin to latch the data to the DAC output
	gpioWrite( WR, 0 );
	asm("nop");
	gpioWrite( WR, 1 );
}


void drawLine( unsigned x1, unsigned y1, unsigned x2, unsigned y2 )
{

	setData( x1, X );
	setData( y1, Y );

//	printf( "( %i, %i )  ( %i, %i )\n", x1, y1, x2, y2 );

	do
	{
//		gpioSleep( PI_TIME_RELATIVE, 0, 10 );

		if( x1 < x2 )
			x1++;
		else
		if( x1 > x2 )
			x1--;

		setData( x1, X );

		if( y1 < y2 )
			y1++;
		else
		if( y1 > y2 )
			y1--;

		// write the data to the DAC
		setData( y1, Y );
	}
	while( ( x1 != x2 ) || ( y1 != y2 ) );
}


void drawRect( unsigned x1, unsigned y1, unsigned x2, unsigned y2 )
{
	drawLine( x1, y1, x2, y1 );
	drawLine( x2, y1, x2, y2 );
	drawLine( x2, y2, x1, y2 );
	drawLine( x1, y2, x1, y1 );
}


// trig functions are in radians
// input X, Y location and a radius
void drawCircle( unsigned h, unsigned k, unsigned r )
{
	float x,y;
	//	x  =  h + r cosθ
	//	y  =  k + r sinθ

	for( double idx = 0; idx <= 2*M_PI; idx += 0.05 )
	{
		// calculate the points along the circumference
		x = h + r * cos( idx );
		y = k + r * sin( idx );

		// write the point
		setData( (unsigned)x, X );
		setData( (unsigned)y, Y );
	}
}
