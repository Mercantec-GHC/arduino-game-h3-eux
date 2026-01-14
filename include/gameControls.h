#include <Arduino.h>
#include <Arduino_JSON.h>

enum GameState {
    
    DISCONNECTED, // 0
    IN_QUEUE, // 1
    IN_GAME, // 2
    GAME_OVER, // 3
};

enum Direction {
    MOVE_RIGHT,
    MOVE_LEFT,
};

typedef struct {
    bool success;
    Direction direction;
    GameState state;
} QueueResponse;

typedef struct {
    bool success;
    GameState state;
    int score;
    double multiplier;
} HeartbeatResponse;

void game_init(String deviceId);
QueueResponse game_joinQueue();
HeartbeatResponse game_sendHeartbeat();
bool game_move(bool move);
