#include "memo.h"
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

int fetch_memo_data(memo* data) {
    CURL* curl;
    CURLcode res;
    MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        char url[512];
        snprintf(url, sizeof(url), "https://api.quotable.io/random");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cJSON* json = cJSON_Parse(chunk.memory);
            if (json != NULL) {
                cJSON* content = cJSON_GetObjectItemCaseSensitive(json, "content");
                cJSON* name = cJSON_GetObjectItemCaseSensitive(json, "name");

                if (cJSON_IsString(name) && (name->valuestring != NULL)) {
                    strncpy(data->name, name->valuestring, sizeof(data->name) - 1);
                }

                if (cJSON_IsString(content) && (content->valuestring != NULL)) {
                    strncpy(data->content, content->valuestring, sizeof(data->content) - 1);
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
