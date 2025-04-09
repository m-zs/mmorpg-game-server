#pragma once
#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <unordered_map>
#include <mutex>

using boost::asio::ip::tcp;

class NetworkManager
{
public:
    NetworkManager(boost::asio::io_context &ioContext, short port);

    using MovementCallback = std::function<void(uint32_t, int32_t, int32_t)>;
    void setMovementCallback(std::function<void(int, int, int)> callback);
    void broadcastToAll(const std::vector<char> &data);
    void sendToPlayer(int playerId, const std::vector<char> &data);

private:
    void startAccept();
    void handleAccept(std::shared_ptr<tcp::socket> socket);
    void startReading(std::shared_ptr<tcp::socket> socket, uint32_t playerId);

    tcp::acceptor m_acceptor;
    MovementCallback m_movementCallback;

    std::unordered_map<int, std::shared_ptr<tcp::socket>> m_connectedPlayers;
    std::mutex m_playersMutex;
};
