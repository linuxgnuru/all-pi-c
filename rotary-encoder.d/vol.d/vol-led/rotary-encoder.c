#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <wiringPi.h>

#include "rotaryencoder.h"

int num_encoders = 0;

void updateEncoders()
{
	struct encoder *encoder = encoders;

	for (; encoder < encoders + num_encoders; encoder++)
	{
		int MSB = digitalRead(encoder->pin_a);
		int LSB = digitalRead(encoder->pin_b);
		int encoded = (MSB << 1) | LSB;
		int sum = (encoder->lastEncoded << 2) | encoded;

		if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoder->value++;
		if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoder->value--;
		/*
		if (sum == 0b0100 || sum == 0b1011) encoder->value++;
		if (sum == 0b0111 || sum == 0b1000) encoder->value--;
		*/
		encoder->lastEncoded = encoded;
	}
}

struct encoder *setupEncoder(int pin_a, int pin_b)
{
	if (num_encoders > max_encoders)
	{
		printf("Max number of encodered exceded: %i\n", max_encoders);
		return NULL;
	}
	struct encoder *newencoder = encoders + num_encoders++;
	newencoder->pin_a = pin_a;
	newencoder->pin_b = pin_b;
	newencoder->lastEncoded = 0;
	pinMode(pin_a, INPUT);
	pinMode(pin_b, INPUT);
	pullUpDnControl(pin_a, PUD_UP);
	pullUpDnControl(pin_b, PUD_UP);
	wiringPiISR(pin_a, INT_EDGE_BOTH, updateEncoders);
	wiringPiISR(pin_b, INT_EDGE_BOTH, updateEncoders);
	return newencoder;
}
