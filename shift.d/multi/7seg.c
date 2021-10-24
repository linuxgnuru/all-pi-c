#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for uid_t

#define DISPLAY_BRIGHTNESS  500

#define DIGIT_ON  HIGH
#define DIGIT_OFF  LOW

#define SEGMENT_ON  LOW
#define SEGMENT_OFF HIGH

int SEGMENT_1=7; 
int SEGMENT_2=11;
int SEGMENT_3=13;
int SEGMENT_4=15;

int SEGMENT_A=3; 
int SEGMENT_B=5; 
int SEGMENT_C=18; 
int SEGMENT_D=19; 
int SEGMENT_E=23; 
int SEGMENT_F=24; 
int SEGMENT_G=25; 

void print_number(int num);

void display_number(int num)
{
    long start;
    int i;
    pinMode(SEGMENT_1,OUTPUT);
    pinMode(SEGMENT_2,OUTPUT);
    pinMode(SEGMENT_3,OUTPUT);
    pinMode(SEGMENT_4,OUTPUT);
    start=millis();
    for(i=4;i>0;i--)
    {
        switch(i)
        {
            case 1:
                digitalWrite(SEGMENT_1,DIGIT_ON);
                break;
            case 2:
                digitalWrite(SEGMENT_2,DIGIT_ON);
                break;
            case 3:
                digitalWrite(SEGMENT_3,DIGIT_ON);
                break;
            case 4:
                digitalWrite(SEGMENT_4,DIGIT_ON);
                break;
        }
        print_number(num%10);
        num/=10;
        delayMicroseconds(DISPLAY_BRIGHTNESS); 
        print_number(10); 
        digitalWrite(SEGMENT_1,DIGIT_OFF);
        digitalWrite(SEGMENT_2,DIGIT_OFF);
        digitalWrite(SEGMENT_3,DIGIT_OFF);
        digitalWrite(SEGMENT_4,DIGIT_OFF);
    }
    while((millis()-start)<10)
        ;
}

int main(int argc, char **argv)
{
    int counter=0;

    printf("7 Segment Multiplexing using Raspberry Pi\n") ;
    if(getuid()!=0) //wiringPi requires root privileges  
    {  
        printf("Error:wiringPi must be run as root.\n");  
        return 1;  
    }  
    if(wiringPiSetup()==-1)  
    {  
        printf("Error:wiringPi setup failed!\n");  
        return 1;  
    }
    for(;;)
    {
        display_number(counter++);
        delay(1000);
        if(counter>9999)
            counter=0;
    }
    return 0;
}

void print_number(int num)
{
    pinMode(SEGMENT_A,OUTPUT);
    pinMode(SEGMENT_B,OUTPUT);
    pinMode(SEGMENT_C,OUTPUT);
    pinMode(SEGMENT_D,OUTPUT);
    pinMode(SEGMENT_E,OUTPUT);
    pinMode(SEGMENT_F,OUTPUT);
    pinMode(SEGMENT_G,OUTPUT);
    switch(num)
    {
        case 0:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_ON);
            digitalWrite(SEGMENT_E,SEGMENT_ON);
            digitalWrite(SEGMENT_F,SEGMENT_ON);
            digitalWrite(SEGMENT_G,SEGMENT_OFF);
            break;

        case 1:
            digitalWrite(SEGMENT_A,SEGMENT_OFF);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_OFF);
            digitalWrite(SEGMENT_E,SEGMENT_OFF);
            digitalWrite(SEGMENT_F,SEGMENT_OFF);
            digitalWrite(SEGMENT_G,SEGMENT_OFF);
            break;

        case 2:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_OFF);
            digitalWrite(SEGMENT_D,SEGMENT_ON);
            digitalWrite(SEGMENT_E,SEGMENT_ON);
            digitalWrite(SEGMENT_F,SEGMENT_OFF);
            digitalWrite(SEGMENT_G,SEGMENT_ON);
            break;

        case 3:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_ON);
            digitalWrite(SEGMENT_E,SEGMENT_OFF);
            digitalWrite(SEGMENT_F,SEGMENT_OFF);
            digitalWrite(SEGMENT_G,SEGMENT_ON);
            break;

        case 4:
            digitalWrite(SEGMENT_A,SEGMENT_OFF);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_OFF);
            digitalWrite(SEGMENT_E,SEGMENT_OFF);
            digitalWrite(SEGMENT_F,SEGMENT_ON);
            digitalWrite(SEGMENT_G,SEGMENT_ON);
            break;

        case 5:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_OFF);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_ON);
            digitalWrite(SEGMENT_E,SEGMENT_OFF);
            digitalWrite(SEGMENT_F,SEGMENT_ON);
            digitalWrite(SEGMENT_G,SEGMENT_ON);
            break;

        case 6:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_OFF);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_ON);
            digitalWrite(SEGMENT_E,SEGMENT_ON);
            digitalWrite(SEGMENT_F,SEGMENT_ON);
            digitalWrite(SEGMENT_G,SEGMENT_ON);
            break;

        case 7:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_OFF);
            digitalWrite(SEGMENT_E,SEGMENT_OFF);
            digitalWrite(SEGMENT_F,SEGMENT_OFF);
            digitalWrite(SEGMENT_G,SEGMENT_OFF);
            break;

        case 8:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_ON);
            digitalWrite(SEGMENT_E,SEGMENT_ON);
            digitalWrite(SEGMENT_F,SEGMENT_ON);
            digitalWrite(SEGMENT_G,SEGMENT_ON);
            break;

        case 9:
            digitalWrite(SEGMENT_A,SEGMENT_ON);
            digitalWrite(SEGMENT_B,SEGMENT_ON);
            digitalWrite(SEGMENT_C,SEGMENT_ON);
            digitalWrite(SEGMENT_D,SEGMENT_ON);
            digitalWrite(SEGMENT_E,SEGMENT_OFF);
            digitalWrite(SEGMENT_F,SEGMENT_ON);
            digitalWrite(SEGMENT_G,SEGMENT_ON);
            break;

        case 10:
            digitalWrite(SEGMENT_A,SEGMENT_OFF);
            digitalWrite(SEGMENT_B,SEGMENT_OFF);
            digitalWrite(SEGMENT_C,SEGMENT_OFF);
            digitalWrite(SEGMENT_D,SEGMENT_OFF);
            digitalWrite(SEGMENT_E,SEGMENT_OFF);
            digitalWrite(SEGMENT_F,SEGMENT_OFF);
            digitalWrite(SEGMENT_G,SEGMENT_OFF);
            break;
    }
}

