#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>
#include <winsock2.h>

class HeartbeatManager {
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    struct ClientStatus {
        TimePoint lastSeen;
        SOCKET socket;
    };

    std::unordered_map<std::string, ClientStatus> clients;
    std::mutex mtx;
    bool running;
    std::thread monitorThread;

    const std::chrono::seconds TIMEOUT_THRESHOLD = std::chrono::seconds(30);
    const std::chrono::seconds CHECK_INTERVAL = std::chrono::seconds(5);

    std::function<void(std::string, SOCKET)> onTimeoutCallback;

public:
    HeartbeatManager() : running(false) {}

    ~HeartbeatManager() {
        stop();
    }

    void setOnTimeoutCallback(std::function<void(std::string, SOCKET)> callback) {
        onTimeoutCallback = callback;
    }

    void start() {
        running = true;
        monitorThread = std::thread(&HeartbeatManager::monitorLoop, this);
    }

    void stop() {
        running = false;
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }

    // Call this whenever a message (of any type) is received from a user
    void updateActivity(const std::string& username, SOCKET s) {
        std::lock_guard<std::mutex> lock(mtx);
        clients[username] = { Clock::now(), s };
    }

    // Explicitly remove when a user logs out normally
    void removeClient(const std::string& username) {
        std::lock_guard<std::mutex> lock(mtx);
        clients.erase(username);
    }

private:
    void monitorLoop() {
        while (running) {
            std::this_thread::sleep_for(CHECK_INTERVAL);

            std::vector<std::pair<std::string, SOCKET>> toRemove;
            auto now = Clock::now();

            {
                std::lock_guard<std::mutex> lock(mtx);
                auto it = clients.begin();
                while (it != clients.end()) {
                    if (now - it->second.lastSeen > TIMEOUT_THRESHOLD) {
                        std::cout << "[Heartbeat] Timing out user: " << it->first << std::endl;
                        toRemove.push_back({ it->first, it->second.socket });
                        it = clients.erase(it); // Remove from map
                    }
                    else {
                        ++it;
                    }
                }
            }

            // Execute callbacks outside the lock to avoid deadlocks
            for (const auto& client : toRemove) {
                if (onTimeoutCallback) {
                    onTimeoutCallback(client.first, client.second);
                }
            }
        }
    }
};