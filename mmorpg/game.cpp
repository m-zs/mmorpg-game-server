#include "game.h"
// todo
Game::Game() : m_network(m_ioContext, 1234) {
    m_network.setMovementCallback([this](int id, int x, int y) {
        handleMovement(id, x, y);
    });
}

void Game::startNetwork(short port) {
    m_networkThread = std::thread([this]() {
        m_ioContext.run();
    });
}

void Game::handleMovement(int playerId, int targetX, int targetY) {
    // 1. Find the player entity
    auto entity = findPlayerEntity(playerId);

    // 2. Validate movement (anti-cheat)
    if (isValidMove(entity, targetX, targetY)) {
        // 3. Schedule the movement (using your scheduler)
        m_scheduler.schedule(entity.movementSpeed.delayMs, [this, entity, targetX, targetY]() {
            auto& pos = m_registry.get<Position>(entity);
            pos.x = targetX;
            pos.y = targetY;
            broadcastMovement(entity); // Notify other players
        });
    }
    else {
        // Kick cheater or log the attempt
        std::cerr << "Invalid move from player " << playerId << std::endl;
    }
}