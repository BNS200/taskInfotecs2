#include <iostream>
#include <thread>
#include <signal.h>
#include <atomic>
#include <memory>
#include "library_loader.hpp"
#include "socket_server.hpp"
#include "client_handler.hpp"

std::atomic<bool> running(true);
std::unique_ptr<SocketServer> server;
LibraryLoader loader;

void signalHandler(int signum)
{
    std::cout << "\n[Main] Received signal " << signum << ", shutting down" << std::endl;
    running = false;

    if (server)
    {
        server->stop();
    }
}

void setupSignalHandlers()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

int main()
{
    setupSignalHandlers();

    if (!loader.load("./lib.so"))
    {
        std::cerr << "[Main] Failed to load library" << std::endl;
        return 1;
    }

    server = std::make_unique<SocketServer>();
    if (!server->start(8080))
    {
        std::cerr << "[Main] Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "[Main] Server is running. Press Ctrl+C to stop" << std::endl;
    std::cout << "[Main] Waiting for connection from Program 1" << std::endl;

    while (running)
    {
        int clientFd = server->acceptClient();
        if (clientFd < 0)
        {
            if (running)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            continue;
        }

        std::cout << "[Main] New client connected" << std::endl;

        ClientHandler handler(clientFd, &loader, running);
        handler.process();

        std::cout << "[Main] Waiting for new connection" << std::endl;
    }

    if (server)
    {
        server->stop();
    }

    std::cout << "[Main] Shutdown complete." << std::endl;
    return 0;
}