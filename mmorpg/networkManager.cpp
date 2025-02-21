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
            handleAccept(socket);
        }

        startAccept();
    });
}

void NetworkManager::handleAccept(std::shared_ptr<tcp::socket> socket) {
    auto playerIdBuffer = std::make_shared<int>();

    boost::asio::async_read(*socket, boost::asio::buffer(playerIdBuffer.get(), sizeof(int)),
        [this, socket, playerIdBuffer](const boost::system::error_code& e, size_t) {
            if (!e) {
                int playerId = *playerIdBuffer;
                std::cout << "Player " << playerId << " connected\n";
                startReading(socket, playerId);
            }
        });
}

void NetworkManager::startReading(std::shared_ptr<tcp::socket> socket, int playerId) {
    auto buffer = std::make_shared<std::array<char, 8>>();

    boost::asio::async_read(*socket, boost::asio::buffer(*buffer),
        [this, socket, buffer, playerId](const boost::system::error_code& e, size_t bytes) {
            if (!e && bytes == 8) {
                int targetX = *reinterpret_cast<int*>(buffer->data());
                int targetY = *reinterpret_cast<int*>(buffer->data() + 4);

                if (m_movementCallback) {
                    m_movementCallback(playerId, targetX, targetY);
                }

                startReading(socket, playerId);
            }
            else {
                if (e) {
                    std::cerr << "Player " << playerId << " disconnected: " << e.message() << "\n";
                }
                // Connection will close automatically when 'socket' is destroyed
            }
        });
}
