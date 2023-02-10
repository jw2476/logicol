#include "log.h"

#include "base.h"
#include "raylib.h"

static log_message log_messages[10] = {};

void log_init() {
    CLEAR(log_messages);
}

void add_message(double showFor, const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (u32 i = 0; i < LENGTH(log_messages); i++) {
        if (log_messages[i].end == 0) {
            vsprintf(log_messages[i].message, format, args);
            log_messages[i].end = GetTime() + showFor;
            break;
        }
    }

    va_end(args);
}

void show_messages() {
    int y = 80;

    for (u32 i = 0; i < LENGTH(log_messages); i++) {
        if (log_messages[i].end != 0) {
            char buffer[200];
            memset(buffer, 0, sizeof(buffer));
            strcat(buffer, log_messages[i].message);
            if (log_messages[i].end < GetTime()) {
                memset(log_messages[i].message, 0, sizeof(log_messages[i].message));
                log_messages[i].end = 0;
            }
            DrawText(buffer, 32, y, 48, BLUE);
            y += 48;
        }
    }

}