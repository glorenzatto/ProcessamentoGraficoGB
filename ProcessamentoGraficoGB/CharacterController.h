#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include "TilemapView.h"
#include "Tank.h"
#include <vector>

class TileMap; // Forward declaration

class CharacterController {
public:
    CharacterController(TilemapView* tilemapView, TileMap* waterMap, std::vector<Tank>& tanks, int& tanksCollected, bool& isGameOver);

    void moveCharacter(int& characterX, int& characterY, const int direction);
    void restart(int& characterX, int& characterY);

    int action;
    int currentFrame;
    float offsetX;
    float offsetY;

private:
    TilemapView* tilemapView;
    TileMap* waterMap;
    std::vector<Tank>& tanks;
    int& tanksCollected;
    bool& isGameOver;
};

#endif // CHARACTERCONTROLLER_H
