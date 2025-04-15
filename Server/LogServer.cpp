/**
 * Name : Yashasvini Bhanuraj
 * Student ID : 164581217
 * Email : ybhanuraj@myseneca.ca
 */

#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <signal.h>

#define BUF_LEN 512
#define LOG_FILE "server.log"

static int serverSocket;
static struct sockaddr_in serverAddr, clientAddr;
static bool isRunning = true;
static std::mutex logMutex;

/**
 * @brief Function that handles receiving log messages from clients and writes them to a log file.
 * This runs in a separate thread and continuously listens for incoming UDP messages.
 */
void ReceiveThread();

/**
 * @brief Signal handler to gracefully shut down the server.
 * @param sig The signal received (e.g., SIGINT).
 * Stops the server's main loop, closes the socket, and exits the program.
 */
void ShutdownHandler(int sig)
{
    isRunning = false;
    close(serverSocket);
    exit(0);
}

/**
 * @brief The main function of the server application.
 * Initializes the UDP server, provides a user menu for managing logs,
 * and spawns a thread to handle incoming log messages from clients.
 * @return int Returns 0 on success, -1 on failure.
 */
int main()
{
    signal(SIGINT, ShutdownHandler);

    // Create a UDP socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    // Configure the server address and port
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the configured address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        return -1;
    }

    // Start the thread to receive log messages
    std::thread recvThread(ReceiveThread);
    recvThread.detach();

    while (isRunning)
    {
        std::cout << "Menu:\n1. Set Log Level\n2. Dump Log File\n0. Shut Down\nChoose an option: ";
        int choice;
        std::cin >> choice;

        if (choice == 1)
        {
            // Send a command to set the log level
            std::cout << "Enter new log level (0=DEBUG, 1=WARNING, 2=ERROR, 3=CRITICAL): ";
            int level;
            std::cin >> level;

            char buf[BUF_LEN];
            memset(buf, 0, BUF_LEN);
            snprintf(buf, BUF_LEN, "Set Log Level=%d", level);

            sendto(serverSocket, buf, strlen(buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
        }
        else if (choice == 2)
        {
            // Read and display the log file contents
            std::ifstream logFile(LOG_FILE);
            if (logFile.is_open())
            {
                std::string line;
                while (getline(logFile, line))
                {
                    std::cout << line << "\n";
                }
                logFile.close();
            }
            else
            {
                std::cerr << "Failed to open log file.\n";
            }
            std::cout << "Press any key to continue...\n";
            getchar();
            getchar();
            system("clear");
        }
        else if (choice == 0)
        {
            // Shut down the server
            isRunning = false;
            break;
        }
    }

    // Close the server socket
    close(serverSocket);
    return 0;
}

/**
 * @brief Receives log messages from clients via UDP and writes them to a log file.
 * This function runs in an endless loop (while `isRunning`) and processes incoming messages.
 * It uses a mutex to ensure thread-safe file writing.
 */
void ReceiveThread()
{
    char buffer[BUF_LEN];
    socklen_t addrLen = sizeof(clientAddr);
    std::ofstream logFile(LOG_FILE, std::ios::out | std::ios::app);

    while (isRunning)
    {
        memset(buffer, 0, BUF_LEN);
        int recvLen = recvfrom(serverSocket, buffer, BUF_LEN, MSG_DONTWAIT, (struct sockaddr *)&clientAddr, &addrLen);
        if (recvLen > 0)
        {
            // Write received messages to the log file in a thread-safe manner
            std::lock_guard<std::mutex> lock(logMutex);
            logFile << buffer;
            logFile.flush();
        }
        sleep(1);
    }

    // Close the log file when exiting the thread
    logFile.close();
}
