#define max_encoders 8

struct encoder
{
	int pin_a;
	int pin_b;
	volatile long value;
	volatile int lastEncoded;
};

struct encoder encoders[max_encoders];

struct encoder *setupEncoder(int pin_a, int pin_b);
