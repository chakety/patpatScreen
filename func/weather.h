#ifndef WEATHER_H
#define WEATHER_H

typedef struct{

    /* data */
    char city[10];
    float temperature;
    char condition[100];
    char localtime[100];
}weatherData;

int fetch_weather_data(const char* city, const char* api_key, weatherData* data);

#endif // WEATHER_H
