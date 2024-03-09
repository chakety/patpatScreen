#include "weather.h"
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char* memory;
    size_t size;
} MemoryStruct;

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realSize = size * nmemb;
    MemoryStruct* mem = (MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + realSize + 1);
    if (!ptr) {
        return 0; // out of memory
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realSize);
    mem->size += realSize;
    mem->memory[mem->size] = 0; // null-terminate

    return realSize;
}

int fetch_weather_data(const char* city, const char* api_key, weatherData* out_data) {
    CURL* curl;
    CURLcode res;
    MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "http://api.weatherapi.com/v1/current.json?key=%s&q=%s&aqi=no", api_key, city);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cJSON* json = cJSON_Parse(chunk.memory);
            if (json != NULL) {
                cJSON* location = cJSON_GetObjectItemCaseSensitive(json, "location");
                cJSON* name = cJSON_GetObjectItemCaseSensitive(location, "name");
                cJSON* current = cJSON_GetObjectItemCaseSensitive(json, "current");
                cJSON* temp_c = cJSON_GetObjectItemCaseSensitive(current, "temp_c");
                cJSON* condition = cJSON_GetObjectItemCaseSensitive(current, "condition");
                cJSON* text = cJSON_GetObjectItemCaseSensitive(condition, "text");

                if (cJSON_IsString(name) && (name->valuestring != NULL)) {
                    strncpy(out_data->city, name->valuestring, sizeof(out_data->city) - 1);
                }

                if (cJSON_IsNumber(temp_c)) {
                    out_data->temperature = temp_c->valuedouble;
                }

                if (cJSON_IsString(text) && (text->valuestring != NULL)) {
                    strncpy(out_data->condition, text->valuestring, sizeof(out_data->condition) - 1);
                }


                cJSON_Delete(json);
            }
        }

        free(chunk.memory);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return (res == CURLE_OK) ? 1 : 0;
}
