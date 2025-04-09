#include <iostream>
#include "networkManager.h"
#include <boost/endian/conversion.hpp>

NetworkManager::NetworkManager(boost::asio::io_context &ioContext, short port)
    : m_acceptor(ioContext, tcp::endpoint(tcp::v4(), port))
{
    startAccept();
}

void NetworkManager::setMovementCallback(std::function<void(int, int, int)> callback)
{
    m_movementCallback = callback;
}

void NetworkManager::broadcastToAll(const std::vector<char> &data)
{
    std::lock_guard<std::mutex> lock(m_playersMutex);

    // Create a copy of the connected players
    auto playersCopy = m_connectedPlayers;

    for (const auto &entry : playersCopy)
    {
        int playerId = entry.first;
        auto socket = entry.second; // Copy the socket to avoid invalidation

        if (socket && socket->is_open())
        {
            // Create a shared pointer to the data to ensure it lives long enough
            auto dataCopy = std::make_shared<std::vector<char>>(data);

            boost::asio::async_write(*socket, boost::asio::buffer(*dataCopy),
                                     [this, playerId, socket, dataCopy](const boost::system::error_code &ec, size_t)
                                     {
                                         if (ec)
                                         {
                                             std::cerr << "Error sending to player " << playerId << ": " << ec.message() << "\n";
                                             // Remove player if there was an error
                                             std::lock_guard<std::mutex> lock(m_playersMutex);
                                             m_connectedPlayers.erase(playerId);
                                         }
                                     });
        }
    }
}

void NetworkManager::startAccept()
{
    auto socket = std::make_shared<tcp::socket>(m_acceptor.get_executor());
    m_acceptor.async_accept(*socket, [this, socket](const boost::system::error_code &error)
                            {
        if (error) {
            std::cerr << "Accept failed: " << error.message() << std::endl;
        }
        else {
            std::cout << "Connection accepted!" << std::endl;
            handleAccept(socket);
        }

        startAccept(); });
}

// networkmanager.cpp - handleAccept
void NetworkManager::handleAccept(std::shared_ptr<tcp::socket> socket)
{
    auto playerIdBuffer = std::make_shared<uint32_t>();

    boost::asio::async_read(*socket, boost::asio::buffer(playerIdBuffer.get(), sizeof(uint32_t)),
                            [this, socket, playerIdBuffer](const boost::system::error_code &e, size_t)
                            {
                                if (!e)
                                {
                                    uint32_t playerId = boost::endian::big_to_native(*playerIdBuffer); // Endian conversion
                                    std::cout << "Player " << playerId << " connected\n";
                                    {
                                        std::lock_guard<std::mutex> lock(m_playersMutex);
                                        m_connectedPlayers[playerId] = socket;
                                    }
                                    startReading(socket, playerId);
                                }
                            });
}

void NetworkManager::startReading(std::shared_ptr<tcp::socket> socket, uint32_t playerId)
{
    auto buffer = std::make_shared<std::array<char, 8>>();

    boost::asio::async_read(*socket, boost::asio::buffer(*buffer),
                            [this, socket, buffer, playerId](const boost::system::error_code &ec, size_t bytes)
                            {
                                if (!ec && bytes == 8)
                                {
                                    int32_t targetX, targetY;
                                    memcpy(&targetX, buffer->data(), 4);
                                    memcpy(&targetY, buffer->data() + 4, 4);

                                    targetX = boost::endian::big_to_native(targetX);
                                    targetY = boost::endian::big_to_native(targetY);

                                    if (m_movementCallback)
                                    {
                                        m_movementCallback(playerId, targetX, targetY);
                                    }
                                    startReading(socket, playerId);
                                }
                                else
                                {
                                    if (ec)
                                    {
                                        std::cerr << "Player " << playerId << " disconnected\n";
                                        std::lock_guard<std::mutex> lock(m_playersMutex);
                                        m_connectedPlayers.erase(playerId);
                                    }
                                }
                            });
}

void NetworkManager::sendToPlayer(int playerId, const std::vector<char> &data)
{
    std::lock_guard<std::mutex> lock(m_playersMutex);
    auto it = m_connectedPlayers.find(playerId);

    if (it != m_connectedPlayers.end())
    {
        auto &socket = it->second;
        boost::asio::async_write(*socket, boost::asio::buffer(data),
                                 [playerId](const boost::system::error_code &ec, size_t)
                                 {
                                     if (ec)
                                     {
                                     }
                                 });
    }
}