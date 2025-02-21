#include "server.h"
#include <boost/asio.hpp>

GameServer::GameServer(short port)
    : m_network(m_ioContext, port),
      m_workGuard(make_work_guard(m_ioContext))
{
    m_network.setMovementCallback([this](int playerId, int x, int y) {
        handlePlayerMovement(playerId, x, y);
    });
}

GameServer::~GameServer()
{
    stop();
}

void GameServer::start()
{
    m_networkThread = thread([this] {
        cout << "Starting network on port 1234..." << endl;
        try {
            m_ioContext.run();
        }
        catch (const exception& e) {
            cerr << "Network error: " << e.what() << endl;
        }
    });

    m_scheduler.run();
}

void GameServer::stop()
{
    m_ioContext.stop();
    m_scheduler.shutdown();
    if (m_networkThread.joinable()) m_networkThread.join();
}

void GameServer::handlePlayerMovement(int playerId, int x, int y)
{
    cout << "Player " << playerId << " moving to (" << x << ", " << y << ")" << endl;

    m_scheduler.schedule(500, [playerId]() {
        cout << "Player " << playerId << " movement completed" << endl;
    });
}
