#pragma once
#include <boost/asio.hpp>
#include <functional>
#include <memory>

using boost::asio::ip::tcp;

class NetworkManager {
public:
    NetworkManager(boost::asio::io_context& ioContext, short port);

    using MovementCallback = std::function<void(int, int, int)>;
    void setMovementCallback(MovementCallback callback);

private:
    void startAccept();
    void handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
    void readMovement(std::shared_ptr<tcp::socket> socket, int playerId);

    tcp::acceptor m_acceptor;
    MovementCallback m_movementCallback;
};
