
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "pigpio.h"

#include "xy.h"

int main()
{
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
	
	unsigned  paddlePosition;
	circleLocation_t circleLoc;

	/* Intializes random number generator */
	time_t t;
	srand((unsigned) time(&t));

	while(1)
	{



		paddlePosition = getPaddle( 0 );
//		printf( "paddle: %i\n", paddlePosition );
		drawPaddle1( paddlePosition );

		circleLoc = getBallLoc();
		drawBall( circleLoc );

		paddlePosition = getPaddle( 1 );
//		printf( "paddle: %i\n", paddlePosition );
		drawPaddle2( paddlePosition );


		gpioDelay( 1500 );
	}
}


circleLocation_t getBallLoc( void )
{
	static unsigned directionX = 0;
	static unsigned directionY = 0;
	static int ballPositionX = 0;
	static int ballPositionY = 50;
	circleLocation_t ball;
	int tmp;
	
	tmp = rand() % 2;

	if( directionX )
	{
		ballPositionX += tmp;
		if( ballPositionX > 240 )
		{
			ballPositionX = 240;
			directionX = 0;
		}
	}
	else
	{
		ballPositionX -= tmp;
		if( ballPositionX < 10 )
		{
			ballPositionX = 10;
			directionX = 1;
		}
	}

	tmp = rand() % 2;

	if( directionY )
	{
		ballPositionY += tmp;
		if( ballPositionY > 240 )
		{
			ballPositionY = 240;
			directionY = 0;
		}
	}
	else
	{
		ballPositionY -= tmp;
		if( ballPositionY < 10 )
		{
			ballPositionY = 10;
			directionY = 1;
		}
	}

	ball.x = ballPositionX;
	ball.y = ballPositionY;

	return ball;
}


void drawBall( circleLocation_t ball )
{
	drawCircle( ball.x, ball.y, 10 );
}


unsigned getPaddle( unsigned id )
{
	static unsigned direction1=0;
	static unsigned direction2=0;
	static int paddlePosition1 = 0;
	static int paddlePosition2 = 0;
	int paddlePosition = 0;
	unsigned direction;
	int tmp;

	if( id == 0 )
	{
		direction = direction1;
		paddlePosition = paddlePosition1;
	}
	else
	if( id == 1 )
	{
		direction = direction2;
		paddlePosition = paddlePosition2;
	}


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

	if( id == 0 )
	{
		direction1 = direction;
		paddlePosition1 = paddlePosition;
	}
	else
	if( id == 1 )
	{
		direction2 = direction;
		paddlePosition2 = paddlePosition;
	}

	return paddlePosition;
}


#define PADDLE_SIZE (40)
void drawPaddle1( unsigned y )
{
	drawRect( 0, y, 1, ( y + PADDLE_SIZE ) );
}


void drawPaddle2( unsigned y )
{
	drawRect( 250, y, 250, ( y + PADDLE_SIZE ) );
}


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
	
	
	gpioWrite( WR, 0 );

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

		setData( y1, Y );
//		printf( "X: %i, Y: %i\n", x1, y1 );


	}
	while( ( x1 != x2 ) || ( y1 != y2 ) );
//	gpioSleep( PI_TIME_RELATIVE, 0, tdelay );
}


void drawRect( unsigned x1, unsigned y1, unsigned x2, unsigned y2 )
{
	drawLine( x1, y1, x2, y1 );
	drawLine( x2, y1, x2, y2 );
	drawLine( x2, y2, x1, y2 );
	drawLine( x1, y2, x1, y1 );
}


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
