#pragma once

#include <stdio.h>

typedef struct log_message_t {
    char message[100];
    double end;
} log_message;


#define TRACE(...) printf("TRACE:%s:%04d -> ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\033[0;37m\n"); add_message(5, __VA_ARGS__)
#define INFO(...) printf("\033[0;34mINFO:%s:%04d -> ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\033[0;37m\n"); add_message(5, __VA_ARGS__)
#define WARN(...) printf("\033[0;33mWARN:%s:%04d -> ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\033[0;37m\n"); add_message(5, __VA_ARGS__)
#define ERROR(...) printf("\033[0;31mERROR:%s:%04d -> ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\033[0;37m\n"); add_message(5, __VA_ARGS__)
#define CRITICAL(...) printf("\033[0;31mCRITICAL:%s:%04d -> ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\033[0;37m\n"); add_message(5, __VA_ARGS__); assert(false)

void log_init();
void add_message(double showFor, const char *format, ...);
void show_messages();