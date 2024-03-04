#include <stdio.h>
#include <stdlib.h>
#include <signal.h>     
#include <pthread.h>
#include "func/weather.h"
#include "func/memo.h"
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
weatherData out_data;
memo memo_data;
const char* city = "Boston";

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



//Display
char* photo_board[2] = {"./pic/whiteboard_1.bmp","./pic/whiteboard_2.bmp"}; 

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


// TEST Fucntions 
int get_weather_teminaltest() {
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

int get_memo_terminaltest() {
	if (fetch_memo_data(&memo_data)) {
        printf("Memo: %s\n", memo_data.content);
		printf("Author: %s\n", memo_data.name);
    } else {
        printf("Failed to fetch weather data.\n");
    }
	return 0;
}

//Execution
int displayCode(){
	IIC_Address = 0x48;
	pthread_create(&t1, NULL, pthread_irq_2in9, NULL);

	UDOUBLE i = 0, j = 0, k = 0;
	//star exe
	singal(SIGINT,Handler_2in9);// when ctrl+c press, run Handler_2in9
	//initialization
	if(DEV_ModuleInit()) {
		printf("init failed\r\n");
		return -1;
	}

	EPD_2IN9_V2_Init();
	EPD_2IN9_V2_Clear();
	ICNT_Init();
	DEV_Delay_ms(100);

	// adding image cache
	UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0)? (EPD_2IN9_V2_WIDTH / 8 ): (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT;
	if((BlackImage = (UBYTE*)malloc(Imagesize)) == NULL){
		printf("Failed to get memory....\n");
		return -1;
	}
	if((BlackImage_ASYNC = (UBYTE*)malloc(Imagesize)) == NULL){
		printf("Failed to get memory....\n");
		return -1;
	}

	// get whiteboard image
	printf("New Image\r\n");
	Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
	GUI_ReadBmp("./pic/whiteboard_1.bmp", 0, 0);

	//dispplay weather & time
	Paint_DrawString_EN( 0, 0,city,&Font16, WHITE, BLACK);//City
	Paint_DrawString_EN( 0, 20,out_data.localtime,&Font16, WHITE, BLACK);//Time
	Paint_DrawString_EN( 0, 60,(char)out_data.temperature,&Font16, WHITE, BLACK);//Temperature
	Paint_DrawString_EN( 0, 40,out_data.condition,&Font16, WHITE, BLACK);//Condition

	//adding memo
	Paint_DrawString_EN( 0, 80,memo_data.content,&Font16, WHITE, BLACK);//Memo
	Paint_DrawString_EN( 0, 100,memo_data.name,&Font16, WHITE, BLACK);//Author

	//adding character


	EPD_2IN9_V2_Display(BlackImage);
	memcpy(BlackImage_ASYNC, BlackImage, Imagesize);
	pthread_create(&t2, NULL, pthread_dis_2in9, NULL);


	//runing forever
	while (1) {
		get_weather_teminaltest();
		get_memo_terminaltest();
		EPD_2IN9_V2_Clear();
		Paint_DrawString_EN(0, 0, city, &Font16, WHITE, BLACK); // City
		Paint_DrawString_EN(0, 20, out_data.localtime, &Font16, WHITE, BLACK); // Time
		Paint_DrawString_EN(0, 60, (char)out_data.temperature, &Font16, WHITE, BLACK); // Temperature
		Paint_DrawString_EN(0, 40, out_data.condition, &Font16, WHITE, BLACK); // Condition
		Paint_DrawString_EN(0, 80, memo_data.content, &Font16, WHITE, BLACK); // Memo
		Paint_DrawString_EN(0, 100, memo_data.name, &Font16, WHITE, BLACK); // Author
		EPD_2IN9_V2_Display(BlackImage);
		memcpy(BlackImage_ASYNC, BlackImage, Imagesize);
		dis_flag = 1;
		DEV_Delay_ms(120000); // Delay for 2 minutes
	}

	return 0;
}




int main(){
    get_weather_teminaltest();	
	get_memo_terminaltest();
	//displayCode();
	return 0;
}
