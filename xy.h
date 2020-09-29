
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

#define JOY_DEV "/dev/input/js0"


typedef struct {
	unsigned x;
	unsigned y;
} circleLocation_t;



void setData( unsigned data, unsigned xy );

void drawLine( unsigned x1, unsigned y1, unsigned x2, unsigned y2 );

void drawRect( unsigned x1, unsigned y1, unsigned x2, unsigned y2 );

void drawCircle( unsigned h, unsigned k, unsigned r );

void drawPaddle1( unsigned y );
void drawPaddle2( unsigned y );
unsigned getPaddle( unsigned id, int joy_fd );

circleLocation_t getBallLoc( void );
void drawBall( circleLocation_t ball );


