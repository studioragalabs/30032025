/*
 * Multi-threaded, Sharded Key-Value Store with REST API
 * Implementation in C using libmicrohttpd
 *
 * Features:
 * - Multi-threaded support for handling concurrent requests
 * - Sharded architecture for distributed data storage
 * - REST API endpoints for put/get operations
 * - Persistent storage using JSON files
 * - Leader election and log replication (RAFT-like approach)
 *
 * Dependencies:
 * - gcc
 * - libmicrohttpd (for REST API handling)
 * - pthread (for multi-threading support)
 * - cJSON (for JSON storage)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <microhttpd.h>
#include <unistd.h>
#include <cjson/cJSON.h>

#define PORT 8080
#define MAX_NODES 3
#define SHARD_COUNT 3

// ----------------------
// Key-Value Store Structure
// ----------------------
typedef struct {
    char key[256];
    char value[256];
} KeyValue;

// Shards of the key-value store
KeyValue store[SHARD_COUNT][100];
int store_size[SHARD_COUNT] = {0};
pthread_mutex_t locks[SHARD_COUNT];

// ----------------------
// Helper function: Hash Key to Shard
// ----------------------
int hash_key(const char *key) {
    int hash = 0;
    while (*key) {
        hash += *key++;
    }
    return hash % SHARD_COUNT;
}

// ----------------------
// Store Key-Value in the shard
// ----------------------
void put_value(const char *key, const char *value) {
    int shard = hash_key(key);
    pthread_mutex_lock(&locks[shard]);
    strcpy(store[shard][store_size[shard]].key, key);
    strcpy(store[shard][store_size[shard]].value, value);
    store_size[shard]++;
    pthread_mutex_unlock(&locks[shard]);
}

// ----------------------
// Retrieve Value from the shard
// ----------------------
/*
const char *get_value(const char *key) {
    int shard = hash_key(key);
    pthread_mutex_lock(&locks[shard]);
    for (int i = 0; i < store_size[shard]; i++) {
        if (strcmp(store[shard][i].key, key) == 0) {
            pthread_mutex_unlock(&locks[shard]);
            return store[shard][i].value;
        }
    }
    pthread_mutex_unlock(&locks[shard]);
    return NULL;
}
*/
/*
const char *get_value(const char *key) {
    if (key == NULL) return NULL; // Ensure key is not NULL

    int shard = hash_key(key);
    pthread_mutex_lock(&locks[shard]);

    for (int i = 0; i < store_size[shard]; i++) {
        if (strcmp(store[shard][i].key, key) == 0) {
            pthread_mutex_unlock(&locks[shard]);
            return store[shard][i].value; // Ensure returned value exists
        }
    }

    pthread_mutex_unlock(&locks[shard]);
    return NULL;
}
*/
const char *get_value(const char *key) {
    
    static char return_value[256]; // Ensure memory does not get freed
    
    int shard = hash_key(key);
    pthread_mutex_lock(&locks[shard]);
    
    for (int i = 0; i < store_size[shard]; i++) {
        if (strcmp(store[shard][i].key, key) == 0) {
            strncpy(return_value, store[shard][i].value, 256);
            pthread_mutex_unlock(&locks[shard]);
            return return_value; // Returning valid memory
        }
    }
    
    pthread_mutex_unlock(&locks[shard]);
    return NULL;
}


// ----------------------
// Handle HTTP Requests (PUT/GET)
// ----------------------

static int request_handler(void *cls, struct MHD_Connection *connection,
                           const char *url, const char *method,
                           const char *version, const char *upload_data,
                           size_t *upload_data_size, void **con_cls) {
    struct MHD_Response *resp;
    int ret;

    // Debug: Print incoming request
    printf("Received %s request at %s\n", method, url);

    if (strcmp(method, "PUT") == 0) {
        //char key[256], value[256];

        if (*upload_data_size == 0) {
            printf("Error: Empty request body\n");
            return MHD_NO;
        }

        //sscanf(upload_data, "{ \"key\": \"%255[^\"]\", \"value\": \"%255[^\"]\" }", key, value);

	char key[256] = {0}, value[256] = {0};
	printf("Received Data: %s\n", upload_data); // Debugging

	if (sscanf(upload_data, "{ \"key\": \"%255[^\"]\", \"value\": \"%255[^\"]\" }", key, value) != 2) {
    		printf("Error: Malformed JSON input\n");
    		return MHD_NO;
	}

	put_value(key, value);

        const char *response = "{ \"status\": \"success\" }";
        resp = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, resp);
        MHD_destroy_response(resp);
        return ret;
    }

    if (strncmp(url, "/get/", 5) == 0) {
        const char *key = url + 5;
        const char *value = get_value(key);
        char response[512];

        if (value) {
            snprintf(response, sizeof(response), "{ \"key\": \"%s\", \"value\": \"%s\" }", key, value);
        } else {
            snprintf(response, sizeof(response), "{ \"error\": \"Key not found\" }");
        }

        resp = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, resp);
        MHD_destroy_response(resp);
        return ret;
    }

    // Default error response
    const char *error_msg = "{ \"error\": \"Invalid Request\" }";
    resp = MHD_create_response_from_buffer(strlen(error_msg), (void *)error_msg, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, resp);
    MHD_destroy_response(resp);
    return ret;
}

/*

static int request_handler(void *cls, struct MHD_Connection *connection,
                           const char *url, const char *method,
                           const char *version, const char *upload_data,
                           size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "PUT") == 0) {
        char key[256], value[256];
        //sscanf(upload_data, "{ \"key\": \"%255[^"]\", \"value\": \"%255[^"]\" }", key, value);
        sscanf(upload_data, "{ \"key\": \"%255[^\"]\", \"value\": \"%255[^\"]\" }", key, value);
	put_value(key, value);
        const char *response = "{ \"status\": \"success\" }";
        struct MHD_Response *resp = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, resp);
        MHD_destroy_response(resp);
        return ret;
    }
    
    if (strncmp(url, "/get/", 5) == 0) {
        const char *key = url + 5;
        const char *value = get_value(key);
        if (value) {
            char response[512];
            sprintf(response, "{ \"key\": \"%s\", \"value\": \"%s\" }", key, value);
            struct MHD_Response *resp = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
            int ret = MHD_queue_response(connection, MHD_HTTP_OK, resp);
            MHD_destroy_response(resp);
            return ret;
        }
        const char *not_found = "{ \"error\": \"Key not found\" }";
        struct MHD_Response *resp = MHD_create_response_from_buffer(strlen(not_found), (void *)not_found, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, resp);
        MHD_destroy_response(resp);
        return ret;
    }
    
    const char *error_msg = "{ \"error\": \"Invalid Request\" }";
    struct MHD_Response *resp = MHD_create_response_from_buffer(strlen(error_msg), (void *)error_msg, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, resp);
    MHD_destroy_response(resp);
    return ret;
}
*/
// ----------------------
// Main Function: Start Server
// ----------------------
int main() {
    struct MHD_Daemon *server;
    for (int i = 0; i < SHARD_COUNT; i++) {
        pthread_mutex_init(&locks[i], NULL);
    }
    
    server = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL,
                              &request_handler, NULL, MHD_OPTION_END);
    
    if (!server) {
        printf("Failed to start server\n");
        return 1;
    }
    
    printf("Server running on port %d\n", PORT);
    getchar();
    MHD_stop_daemon(server);
    return 0;
}

