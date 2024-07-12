#ifndef DiamondView_h
#define DiamondView_h

#include "TilemapView.h"
#include <iostream>
using namespace std;

class DiamondView : public TilemapView {
public:
    void computeDrawPosition(const int col, const int row, const float tileWidth, const float tileHeight, float& targetX, float& targetY) const {
        targetX = col * tileWidth / 2 + row * tileWidth / 2;
        targetY = col * tileHeight / 2 - row * tileHeight / 2;
    }

    void computeMouseMap(int& col, int& row, const float tileWidth, const float tileHeight, const float mouseX, const float mouseY) const {
        float halfTileWidth = tileWidth / 2.0f;
        float halfTileHeight = tileHeight / 2.0f;

        row = mouseY / tileHeight;
        col = mouseX / tileWidth;
        cout << "dest: " << col << "," << row << endl;
        cout << "tw/th " << tileWidth << "," << tileHeight << endl;
        cout << "mx/my " << mouseX << "," << mouseY << endl;
    }

    void computeTileWalking(int& col, int& row, const int direction) const {
        switch (direction) {
        case DIRECTION_NORTH:
            row--;
            col++;
            break;
        case DIRECTION_NORTHEAST:
            col++;
            break;
        case DIRECTION_EAST:
            col--;
            row--;
            break;
        case DIRECTION_SOUTHEAST:
            row++;
        case DIRECTION_SOUTH:
            row++;
            col--;
            break;
        case DIRECTION_SOUTHWEST:
            col--;
            break;
        case DIRECTION_WEST:
            row++;
            col++;
            break;
        case DIRECTION_NORTHWEST:
            row--;
            break;
        }
    }
};

#endif /* DiamondView_h */
