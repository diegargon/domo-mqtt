/*
*	Diego Garcia (diegargon)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPiI2C.h>
#include <unistd.h> 
#include <time.h>

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
    PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
    PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
    PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64    \
    PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
    PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end macros --- */

#define DEBUG_CONFIG_BIN	(0)
#define ADC_CHANNELS		(1)
#define ADC_UPDATE_TIME 	(100000)
#define SET_RATE			(0)
#define SET_GAIN			(0)	
#define ALARM_TRIGGER_TIME	(1)

#define GAIN_VAL			(1)
#define RATE_VAL			(0)

#define I2CDEV 				"/dev/i2c-0"	
#define CONVERT_RATE 		(0.000125)

#define	CONFIG_DEFAULTS		(0x8583)

#define REG_CONVERSION (0x00)
#define REG_CONFIG (0x01)

#define AN1_SINGLE (0x4000) 
#define AN2_SINGLE (0x5000) 
#define AN3_SINGLE (0x6000) 
#define AN4_SINGLE (0x7000)


int ads_address = 0x48;
uint16_t val;

uint16_t writeBuf;
uint16_t readBuf;

typedef struct ADC {
	int fd;
	uint16_t config;
} ADC;

int setupADS1115(ADC *adc);


uint16_t readADC_SingleEnded(ADC adc, uint8_t channel) {
	
	int16_t result;
	

	switch (channel)
	{
		case (0): adc.config |= AN1_SINGLE; break;
		case (1): adc.config |= AN2_SINGLE; break;
		case (2): adc.config |= AN3_SINGLE;	break;
		case (3): adc.config |= AN4_SINGLE; break;
	}
	
	if (DEBUG_CONFIG_BIN) 
	{
		printf("Config "
           PRINTF_BINARY_PATTERN_INT16 "\n",		   
           PRINTF_BYTE_TO_BINARY_INT16(adc.config));
		
	
	}
	
	adc.config = __bswap_16 (adc.config);
	wiringPiI2CWriteReg16(adc.fd, REG_CONFIG, adc.config);
	
	for (;;)
	{
		result =  wiringPiI2CReadReg16 (adc.fd, REG_CONFIG) ;
		result = __bswap_16 (result) ;
		if ((result & 0x8000) != 0)
			break ;
		usleep(1000);
	}
	
	result = wiringPiI2CReadReg16(adc.fd, REG_CONVERSION);		
	result = __bswap_16 (result) ;
	
	return (int)result ;

}

int setupADS1115(ADC *adc) {
	
	/* bug in wirireop, not open i2c-0.
	* Using SetupInterface instead 
	if ( (fd = wiringPiI2CSetup(ads_address) < 0)  )
	{	  
		perror("Error: Couldn't open device!\n");
		exit (1);
	}
	*/
	
	if (( adc->fd = wiringPiI2CSetupInterface (I2CDEV, 0x48) ) < 0) 
	{
		perror("Error: Couldn't open device!\n");
		exit (1);		
	}

	adc->config = CONFIG_DEFAULTS;

	return 0;
}

int setGain(ADC *adc, int gain) {
	/*
		bits 11-9
		0 = 000 = ±6.144V
		1 = 001 = ±4.096V
		2 = 010 = ±2.048V
		3 = 011 = ±1.024V
		4 = 100 = ±0.512V
		5 = 101 = ±0.256V
	*/
	
	if (gain < 0 || gain > 5) 
	{
		return -1;
	}

	adc->config &= ~0x0E00;
	
		   
	switch (gain)
	{
		case (0): adc->config |= 0x0000 ; break;
		case (1): adc->config |= 0x0200; break;
		case (2): adc->config |= 0x0400; break;
		case (3): adc->config |= 0x0600; break;
		case (4): adc->config |= 0x0800; break;
		case (5): adc->config |= 0x0A00; break;

	}	

	return 0;
}

int setRate(ADC *adc, int rate) {
	/*
		bits 7-5
		0 = 8sps
		1 = 16sps
		2 = 32sps
		3 = 64sps
		4 = 128sps (default)
		5 = 250sps
		6 = 475sps
		7 = 860sps
	*/
	
	if (rate < 0 || rate > 7) 
	{
		return -1;
	}

	adc->config &= ~0x00E0; 

	switch (rate)
	{
		case (0): adc->config |= 0x0000; break;
		case (1): adc->config |= 0x0020; break;
		case (2): adc->config |= 0x0040; break;
		case (3): adc->config |= 0x0060; break;
		case (4): adc->config |= 0x0080; break;
		case (5): adc->config |= 0x00A0; break;
		case (6): adc->config |= 0x00C0; break;
		case (7): adc->config |= 0x00E0; break;
	}			

	return 0;
}

int write_counters(int counter, int alarm_counter) {
	FILE *f = fopen("track.txt", "a+");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	
//	fprintf(f, "Counter: %d Alarm Counter: %d", counter, *alarm_counter);
	fclose(f);
	
	return 0;
	
}

int write_time_lasted(double lasted) {
	FILE *f = fopen("track.txt", "a+");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	
	fprintf(f, " -> Lasted : %f\n", lasted);
	fprintf(f, "---------------------------------------------------------------------------\n");
	fclose(f);
	
	return 0;
	
}

int write_time_now()
{
	FILE *f = fopen("track.txt", "a+");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	fprintf (f, "Trigger Time and date: %s\n", asctime (timeinfo) );
	fclose(f);
	
	return 0;	
}

int alarm_trigger(int alarm_counter, float vol) {
	FILE *f = fopen("track.txt", "a+");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	fprintf (f, "ALARM TRIGGED!!!!\n");
	fprintf (f, "Volt: %3.2f \n", vol);
	fclose(f);

	
	return 0;
}

int main()
{
	//int fd;
	time_t start_time, duration;
	
	//double time_taken = -1.0;

	int16_t adc0, adc1, adc2, adc3;
	int counter = 0;
	int alarm_counter = 0;
	int alarm_triggered = 0;
	ADC adc;
	
	setupADS1115(&adc);

	if ( SET_GAIN && setGain(&adc, GAIN_VAL) == -1) 
	{
		printf("Error Gain\n");
	}
	if ( SET_RATE && setRate(&adc, RATE_VAL) == -1)
	{
		printf("Error setRate");
	}
	
	int change_flag = 0;
	
	for (;;) {
		printf("\033[2J"); 
		printf("Counter: %d , Alarm Counter: %d, flag %d\n", counter, alarm_counter, change_flag);		
		printf("Last Trigger lasted (time) %d\n",  (int) duration);
		if ( ADC_CHANNELS >= 1 )
		{
			adc0 = readADC_SingleEnded(adc, 0);
			printf("AIN0: %d in volts %3.2f\n" ,adc0, adc0 * CONVERT_RATE);
			if ( ((adc0 * CONVERT_RATE) >= 4.0) && change_flag == 0) {
				change_flag = 1;	
				//time_taken = 0;
				start_time = time(0);
				counter++;	
				write_time_now();
				write_counters(counter, alarm_counter);
				
			} 
			//Trigger and alarm ( > duration)
			if ( 
				((adc0 * CONVERT_RATE) >= 4.0)
				&& ((time(0) - start_time) >= ALARM_TRIGGER_TIME)
				&& (alarm_triggered != 1)
			) {
				alarm_counter++;
				alarm_triggered = 1;
				alarm_trigger(alarm_counter, adc0 * CONVERT_RATE);
			}
			//Trigger off
			if ( ( (adc0 * CONVERT_RATE) < 4.0 ) && change_flag == 1) {
				
				duration = time(0) - start_time;	
				write_time_lasted(duration);
			 	change_flag = 0;
				alarm_triggered = 0;
			} 
			
		}
		
		if ( ADC_CHANNELS >= 2 )
		{			
				adc1 = readADC_SingleEnded(adc,1);
				printf("AIN1: %d in volts %3.2f\n",adc1, adc1 * CONVERT_RATE);
		}
		if ( ADC_CHANNELS >= 3 )
		{
				adc2 = readADC_SingleEnded(adc,2);
				printf("AIN2: %d in volts %3.2f\n",adc2, adc2 * CONVERT_RATE);
		}
		if ( ADC_CHANNELS == 4 ) 
		{			
				adc3 = readADC_SingleEnded(adc,3);
				printf("AIN3: %d in volts %3.2f\n",adc3, adc3 * CONVERT_RATE);	
		}
		
		usleep(ADC_UPDATE_TIME);
	}
	
   close(adc.fd);
   
}
