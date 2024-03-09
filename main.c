#include <stdio.h>
#include <stdlib.h>
#include <signal.h>     
#include <pthread.h>
#include "func/weather.h"
#include "func/memo.h"
#include "EPD_2in9_V2.h"
#include "ICNT86X.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "time.h"

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
char* photo_dog[2] = {"./pic/dog1.bmp","./pic/cat.bmp"};

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
        printf("Weather in %s: %.2f°C, %s\n", city, out_data.temperature, out_data.condition);
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

int touchtest(){
	IIC_Address = 0x48;
	//star exe
	signal(SIGINT,Handler_2in9);// when ctrl+c press, run Handler_2in9
	//initialization
	DEV_ModuleInit();
	pthread_create(&t1, NULL, pthread_irq_2in9, NULL);

	EPD_2IN9_V2_Init();
	EPD_2IN9_V2_Clear();
	ICNT_Init();
	DEV_Delay_ms(100);
	pthread_create(&t2, NULL, pthread_dis_2in9, NULL);
	while(1){
		if(ICNT_Scan() == 1 || (ICNT86_Dev_Now.X[0] == ICNT86_Dev_Old.X[0] && ICNT86_Dev_Now.Y[0] == ICNT86_Dev_Old.Y[0])){
			continue;
		}
	}
	
	return 0;
}
//time
void Get_Current_Time(PAINT_TIME *pTime)
{
    time_t t;
    struct tm *nowtime;
    
    time(&t);
	nowtime = localtime(&t);
	
	pTime->Year = nowtime->tm_year + 1900;
	pTime->Month = nowtime->tm_mon + 1;
	pTime->Day = nowtime->tm_mday;
	pTime->Hour = nowtime->tm_hour;
	pTime->Min = nowtime->tm_min;
}

//Execution
int displayCode(){
	IIC_Address = 0x48;
	UDOUBLE i = 0;
	//star exe
	signal(SIGINT,Handler_2in9);// when ctrl+c press, run Handler_2in9
	//initialization
	DEV_ModuleInit();
	pthread_create(&t1, NULL, pthread_irq_2in9, NULL);

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

	//dispplay weather
	Paint_DrawString_EN( 2, 0,city,&Font12, WHITE, BLACK);//City
	Paint_DrawFloatNum( 170, 0,out_data.temperature,2,&Font12, WHITE, BLACK);//Temperature
	Paint_DrawString_EN( 200, 0,out_data.condition,&Font12, WHITE, BLACK);//Condition

	//adding character
	PAINT_TIME sPaint_time;
	Get_Current_Time(&sPaint_time);
	Paint_DrawTime(55, 0, &sPaint_time, &Font12, WHITE, BLACK);
	Paint_DrawDate(90, 0, &sPaint_time, &Font12, WHITE, BLACK);

	//adding memo
	Paint_DrawString_EN( 0, 15,memo_data.name,&Font12, WHITE, BLACK);//Author
	Paint_DrawString_EN( 0, 30,memo_data.content,&Font12, WHITE, BLACK);//Memo

	//adding character
	GUI_ReadBmp(photo_dog[0], 74, 64);

	EPD_2IN9_V2_Display_Base(BlackImage);
	memcpy(BlackImage_ASYNC, BlackImage, Imagesize);
	pthread_create(&t2, NULL, pthread_dis_2in9, NULL);
	//runing forever
	time_t lastUpdate = 0; // Add this line at the beginning of your function
	while (1) {
		time_t currentTime = time(NULL);
		if(difftime(currentTime, lastUpdate) >= 60) { // Check if a minute has passed
			lastUpdate = currentTime; // Update the last update time
			get_weather_teminaltest();
			get_memo_terminaltest();
			while(strlen(memo_data.content) > 110){
				get_memo_terminaltest();
			}
			Paint_ClearWindows(0,0,296,128,WHITE);
			
			Paint_DrawString_EN( 2, 0,city,&Font12, WHITE, BLACK);//City
			Paint_DrawFloatNum( 170, 0,out_data.temperature,2,&Font12, WHITE, BLACK);//Temperature
			Paint_DrawString_EN( 205, 0,out_data.condition,&Font12, WHITE, BLACK);//Condition
			Get_Current_Time(&sPaint_time);
			Paint_DrawTime(55, 0, &sPaint_time, &Font12, WHITE, BLACK);
			Paint_DrawDate(90, 0, &sPaint_time, &Font12, WHITE, BLACK);
			Paint_DrawString_EN( 0, 15,memo_data.name,&Font12, WHITE, BLACK);//Author
			Paint_DrawString_EN( 0, 30,memo_data.content,&Font12, WHITE, BLACK);//Memo
			GUI_ReadBmp(photo_dog[i], 74, 64);
			EPD_2IN9_V2_Display_Base(BlackImage);
		}
	
		if(ICNT_Scan() == 1|| (ICNT86_Dev_Now.X[0] == ICNT86_Dev_Old.X[0] && ICNT86_Dev_Now.Y[0] == ICNT86_Dev_Old.Y[0])){
			continue;
		}

		if(ICNT86_Dev_Now.TouchCount){
			if(ICNT86_Dev_Now.X[0] >73 && ICNT86_Dev_Now.X[0] < 175 && ICNT86_Dev_Now.Y[0] > 63 && ICNT86_Dev_Now.Y[0] < 128){
				printf("swtiching character\r\n");
				i = i ^ 1; // Toggle i between 0 and 1
				GUI_ReadBmp(photo_dog[i], 74, 64);
				EPD_2IN9_V2_Display_Partial(BlackImage);
			}
		}
		memcpy(BlackImage_ASYNC, BlackImage, Imagesize);
	}

	return 0;
}

int main(){
    get_weather_teminaltest();	
	get_memo_terminaltest();
	while(strlen(memo_data.content) > 110){
			get_memo_terminaltest();
	}
	displayCode();
	//touchtest();
	return 0;
}
