#include <iostream>
#include "networkManager.h"

NetworkManager::NetworkManager(boost::asio::io_context& ioContext, short port)
    : m_acceptor(ioContext, tcp::endpoint(tcp::v4(), port)) {
    startAccept();
}

void NetworkManager::setMovementCallback(std::function<void(int, int, int)> callback) {
    m_movementCallback = callback;
}

void NetworkManager::startAccept() {
    auto socket = std::make_shared<tcp::socket>(m_acceptor.get_executor());
    m_acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
        if (error) {
            std::cerr << "Accept failed: " << error.message() << std::endl;
        }
        else {
            std::cout << "Connection accepted!" << std::endl;
            handleAccept(socket, error);
            startAccept(); // Continue accepting connections
        }
    });
}


void NetworkManager::handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error) {
    if (!error) {
        // Read player ID (auth token would go here)
        int playerId = 0;
        boost::asio::read(*socket, boost::asio::buffer(&playerId, sizeof(playerId)));

        // Listen for movement packets
        auto buffer = std::make_shared<std::array<char, 8>>(); // 8 bytes: x (int) + y (int)
        boost::asio::async_read(*socket, boost::asio::buffer(*buffer),
            [this, socket, buffer, playerId](const boost::system::error_code& error, size_t bytes) {
                if (!error && bytes == 8) {
                    int targetX = *reinterpret_cast<int*>(buffer->data());
                    int targetY = *reinterpret_cast<int*>(buffer->data() + 4);
                    if (m_movementCallback) {
                        m_movementCallback(playerId, targetX, targetY);
                    }
                }
            });
    }
}

void NetworkManager::readMovement(std::shared_ptr<tcp::socket> socket, int playerId)
{
}
