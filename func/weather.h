#ifndef WEATHER_H
#define WEATHER_H

typedef struct{

    /* data */
    char city[150];
    float temperature;
    char condition[20];
}weatherData;

int fetch_weather_data(const char* city, const char* api_key, weatherData* data);

#endif // WEATHER_H
