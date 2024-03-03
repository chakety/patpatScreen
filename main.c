#include <stdio.h>
#include <stdlib.h>
#include <signal.h>     
#include <pthread.h>
#include "func/weather.h"
#include "DEV_Config.h"
#include "EPD_2in9_V2.h"
#include "ICNT86X.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"

//External Identifier 
extern ICNT86_Dev ICNT86_Dev_Now, ICNT86_Dev_Old;
extern int IIC_Address;
static pthread_t t1, t2;
UBYTE flag_2in9=1, dis_lock=1, dis_flag=1;	
UBYTE *BlackImage, *BlackImage_ASYNC;

//thread
void Handler_2in9(int signo)
{
    //System Exit
    printf("\r\nHandler_2in9:exit\r\n");
	EPD_2IN9_V2_Sleep();
	DEV_Delay_ms(1000);
	flag_2in9 = 0;
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
    DEV_ModuleExit();
    exit(0);
}

void *pthread_irq_2in9(void *arg)
{
	while(flag_2in9) {
		if(DEV_Digital_Read(INT) == 0) {
			ICNT86_Dev_Now.Touch = 1;
			// printf("!");
		}
		else {
			ICNT86_Dev_Now.Touch = 0;
		}
		DEV_Delay_ms(0.01);
	}
	printf("thread1:exit\r\n");
	pthread_exit(NULL);
}

void *pthread_dis_2in9(void *arg)
{
	while(flag_2in9) {
		if(dis_flag) {
			dis_lock = 1;
			EPD_2IN9_V2_Display_Partial(BlackImage_ASYNC);
			dis_flag = 0;
			dis_lock = 0;
			printf("ASYNC display over, unlock \r\n");
		}
		else {
			dis_lock = 0;
		}
		// DEV_Delay_ms(0.01);
	}
	
	printf("thread2:exit\r\n");
	pthread_exit(NULL);
}

//Display
char* photo_board[2] = {"./pic/whiteboard_1.png","./pic/whiteboard_2.png"}; 




// Fucntions 
int get_weather_teminaltest() {
    weatherData out_data;
    const char* city = "Boston";

    // Read API key from environment variable
    const char* api_key = getenv("WEATHER_API_KEY");
    if (!api_key) {
        fprintf(stderr, "Error: WEATHER_API_KEY environment variable is not set.\n");
        return 1;
    }

    if (fetch_weather_data(city, api_key, &out_data)) {
        printf("Weather in %s: %.2f°C, %s, time: %s\n", city, out_data.temperature, out_data.condition,out_data.localtime);
    } else {
        printf("Failed to fetch weather data.\n");
    }

    return 0;
}









int main(){
    get_weather_teminaltest();
}
