/**
 * Name : Yashasvini Bhanuraj
 * Student ID : 164581217
 * Email : ybhanuraj@myseneca.ca
 */

#include "Logger.h"
#include <iostream>
#include <cstring>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUF_LEN 512 // Buffer size for storing log messages and server commands.

static int logSocket;                 // UDP socket used for communication with the log server.
static struct sockaddr_in serverAddr; // Holds the server's address (IP and port).
static std::mutex logMutex;           // Ensures thread-safe access to shared resources.
static LOG_LEVEL filterLevel = DEBUG; // Current filter level; logs below this are ignored.
static bool isRunning = true;         // Indicates if the logger is active, controlling thread execution.

/**
 * @brief Thread function to handle incoming commands from the server.
 * Listens for commands like "Set Log Level" to dynamically update the log filter level.
 * @param fd File descriptor for the UDP socket.
 */
void ReceiveThread(int fd);

/**
 * @brief Initializes the logger by setting up a UDP socket and starting a receiver thread.
 * The logger sends log messages to a remote server specified by its IP and port.
 * @return int Returns 0 on success, -1 on failure.
 */
int InitializeLog()
{
    logSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (logSocket < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);                   // Default server port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP

    std::thread recvThread(ReceiveThread, logSocket);
    recvThread.detach();

    return 0;
}

/**
 * @brief Sets the filter level for log messages.
 * Only messages with severity greater than or equal to the filter level will be sent to the server.
 * @param level The desired log level (DEBUG, WARNING, ERROR, CRITICAL).
 */
void SetLogLevel(LOG_LEVEL level)
{
    std::lock_guard<std::mutex> lock(logMutex);
    filterLevel = level;
}

/**
 * @brief Logs a message if its severity meets or exceeds the current filter level.
 * Adds a timestamp, severity, file name, function name, and line number to the log message.
 * @param level The severity level of the log message.
 * @param file The file name where the log is being generated.
 * @param func The function name where the log is being generated.
 * @param line The line number in the file where the log is being generated.
 * @param message The actual log message.
 */
void Log(LOG_LEVEL level, const char *file, const char *func, int line, const char *message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    if (level < filterLevel)
        return;

    char buf[BUF_LEN];
    memset(buf, 0, BUF_LEN);
    const char *levelStr[] = {"DEBUG", "WARNING", "ERROR", "CRITICAL"};

    time_t now = time(0);
    char *dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0'; // Remove newline

    int len = snprintf(buf, BUF_LEN, "%s [%s] %s:%s:%d %s\n", dt, levelStr[level], file, func, line, message);
    if (len < 0)
    {
        perror("Log formatting failed");
        return;
    }

    sendto(logSocket, buf, len, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
}

/**
 * @brief Cleans up the logger by stopping the receiver thread and closing the UDP socket.
 */
void ExitLog()
{
    isRunning = false;
    close(logSocket);
}

/**
 * @brief Thread function to handle commands from the server.
 * This function listens for incoming messages from the server and processes commands such as updating the log level.
 * @param fd File descriptor for the UDP socket.
 */
void ReceiveThread(int fd)
{
    char buffer[BUF_LEN];
    while (isRunning)
    {
        memset(buffer, 0, BUF_LEN);
        int recvLen = recvfrom(fd, buffer, BUF_LEN, MSG_DONTWAIT, NULL, NULL);
        if (recvLen > 0)
        {
            std::lock_guard<std::mutex> lock(logMutex);
            if (strncmp(buffer, "Set Log Level=", 14) == 0)
            {
                int newLevel = atoi(buffer + 14);
                if (newLevel >= DEBUG && newLevel <= CRITICAL)
                {
                    filterLevel = (LOG_LEVEL)newLevel;
                }
            }
        }
        sleep(1);
    }
}
