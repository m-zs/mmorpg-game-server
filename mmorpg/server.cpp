#include "server.h"
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>

GameServer::GameServer(short port)
    : m_network(m_ioContext, port),
      m_workGuard(make_work_guard(m_ioContext))
{
    m_network.setMovementCallback([this](uint32_t playerId, int32_t x, int32_t y)
                                  { handlePlayerMovement(playerId, x, y); });
}

GameServer::~GameServer()
{
    stop();
}

void GameServer::start()
{
    m_networkThread = thread([this]
                             {
        cout << "Starting network on port 1234..." << endl;
        try {
            m_ioContext.run();
        }
        catch (const exception& e) {
            cerr << "Network error: " << e.what() << endl;
        } });

    m_scheduler.run();
}

void GameServer::stop()
{
    m_ioContext.stop();
    m_scheduler.shutdown();
    if (m_networkThread.joinable())
        m_networkThread.join();
}

void GameServer::handlePlayerMovement(uint32_t playerId, int32_t x, int32_t y)
{
    cout << "Player " << playerId << " moving to (" << x << ", " << y << ")" << endl;

    bool isValid = x >= 0 && y >= 0 && x < 1000 && y < 1000;

    std::vector<char> response;
    if (isValid)
    {
        cout << "valid" << endl;
        // Broadcast to all players
        std::vector<char> broadcastPacket = createMovementPacket(playerId, x, y);
        m_network.broadcastToAll(broadcastPacket);
    }
    else
    {
        // Send rejection
        cout << "not valid" << endl;
        std::vector<char> rejectPacket = createRejectionPacket();
        m_network.sendToPlayer(playerId, rejectPacket);
    }
}

std::vector<char> GameServer::createMovementPacket(uint32_t playerId, int32_t x, int32_t y)
{
    std::vector<char> packet(13);
    packet[0] = 0x21;
    uint32_t netId = boost::endian::native_to_big(playerId);
    int32_t netX = boost::endian::native_to_big(x);
    int32_t netY = boost::endian::native_to_big(y);

    memcpy(packet.data() + 1, &netId, 4);
    memcpy(packet.data() + 5, &netX, 4);
    memcpy(packet.data() + 9, &netY, 4);
    return packet;
}

std::vector<char> GameServer::createConfirmationPacket(uint32_t playerId)
{
    std::vector<char> packet(5);
    packet[0] = 0x22; // Confirmation header
    uint32_t netId = boost::endian::native_to_big(playerId);
    memcpy(packet.data() + 1, &netId, 4);
    return packet;
}

std::vector<char> GameServer::createRejectionPacket()
{
    return std::vector<char>{0x23};
}
