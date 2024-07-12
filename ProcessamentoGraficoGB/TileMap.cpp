class TileMap {
    float zLevel;               // caso de eventual de vários tilemaps sobrepostos
    unsigned int tileSetId;      // indicação do tileset utilizado
    int mapWidth, mapHeight;     // dimensões da matriz
    unsigned char* tileMap; // mapa com ids dos tiles que formam o cenário

public:
    TileMap(int width, int height, unsigned char initWith) {
        this->tileMap = new unsigned char[width * height];
        this->mapWidth = width;
        this->mapHeight = height;
        this->zLevel = 0.0f;
        this->tileSetId = 0;
    }

    unsigned char* getMap() {
        return this->tileMap;
    }

    int getWidth() {
        return this->mapWidth;
    }

    int getHeight() {
        return this->mapHeight;
    }

    int getTile(int col, int row) {
        return this->tileMap[col + row * this->mapWidth];
    }

    void setTile(int col, int row, unsigned char tile) {
        this->tileMap[col + row * this->mapWidth] = tile;
    }

    int getTileSet() {
        return this->tileSetId;
    }

    float getZ() {
        return this->zLevel;
    }

    void setZ(float zLevel) {
        this->zLevel = zLevel;
    }

    void setTid(int tileSetId) {
        this->tileSetId = tileSetId;
    }
};
