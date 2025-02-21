#include "NetworkManager.hpp"

class Game {
public:
    Game();
    void startNetwork(short port);

private:
    void handleMovement(int playerId, int targetX, int targetY);

    NetworkManager m_network;
    boost::asio::io_context m_ioContext;
    std::thread m_networkThread;
};
