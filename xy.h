
#define SRV_DLY ( 100 )

#define BALL_MAX_Y	( 240 )
#define BALL_MAX_X	( 240 )
#define BALL_MIN_Y	( 10 )
#define BALL_MIN_X	( 10 )
#define BALL_DIAMETER ( 10 )

#define PADDLE_1_X	( 6 )
#define PADDLE_2_X	( 249 )
#define PADDLE_SIZE ( 40 )

// configure our IO mappings for the DAC
#define D0	(4)
#define D1	(5)
#define D2	(6)
#define D3	(12)
#define D4	(13)
#define D5	(16)
#define D6	(17)
#define D7	(18)
#define WR	(19)
#define A0	(20)
#define A1	(21)

#define X	( 0 )
#define Y	( 1 )

#define NONE	( 1 )
#define PLAYER1	( 1 )
#define PLAYER2	( 2 )


#define JOY_DEV "/dev/input/js0"


typedef struct {
	// X location
	unsigned x;

	// Y location
	unsigned y;

	// miss (0=none, 1=player 1, 2 = player 2)
	unsigned player;
} circleLocation_t;



void setData( unsigned data, unsigned xy );

void drawLine( unsigned x1, unsigned y1, unsigned x2, unsigned y2 );

void drawRect( unsigned x1, unsigned y1, unsigned x2, unsigned y2 );

void drawCircle( unsigned h, unsigned k, unsigned r );

void drawPaddle1( unsigned y );
void drawPaddle2( unsigned y );
unsigned getPaddle( unsigned id, int joy_fd );

circleLocation_t getBallLoc( bool serve );
void drawBall( circleLocation_t ball );

bool checkMiss( circleLocation_t * circleLoc, unsigned paddlePosition1, unsigned paddlePosition2 );
