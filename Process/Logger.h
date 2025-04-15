/**
 * Name : Yashasvini Bhanuraj
 * Student ID : 164581217
 * Email : ybhanuraj@myseneca.ca
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <string>

// Log severity levels
typedef enum {
    DEBUG,
    WARNING,
    ERROR,
    CRITICAL
} LOG_LEVEL;

// Function declarations
int InitializeLog();
void SetLogLevel(LOG_LEVEL level);
void Log(LOG_LEVEL level, const char *file, const char *func, int line, const char *message);
void ExitLog();

#endif // LOGGER_H
