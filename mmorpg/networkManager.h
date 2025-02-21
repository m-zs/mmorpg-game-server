#pragma once
#include <boost/asio.hpp>
#include <functional>
#include <memory>

using boost::asio::ip::tcp;

class NetworkManager {
public:
    NetworkManager(boost::asio::io_context& ioContext, short port);

    using MovementCallback = std::function<void(int, int, int)>;
    void setMovementCallback(std::function<void(int, int, int)> callback);

private:
    void startAccept();
    void handleAccept(std::shared_ptr<tcp::socket> socket);
    void startReading(std::shared_ptr<tcp::socket> socket, int playerId);

    tcp::acceptor m_acceptor;
    MovementCallback m_movementCallback;
};
