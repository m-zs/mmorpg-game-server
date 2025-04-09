#include "scheduler.h"
#include "networkManager.h"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <atomic>

using namespace std;
using namespace boost::asio;

class GameServer {
public:
    GameServer(short port);
    ~GameServer();

    void start();
    void stop();

private:
    void handlePlayerMovement(uint32_t playerId, int32_t x, int32_t y);

    std::vector<char> createMovementPacket(uint32_t playerId, int32_t x, int32_t y);

    std::vector<char> createConfirmationPacket(uint32_t playerId);

    std::vector<char> createRejectionPacket();

    io_context m_ioContext;
    NetworkManager m_network;
    Scheduler m_scheduler;
    std::thread m_networkThread;
    executor_work_guard<io_context::executor_type> m_workGuard;
};
