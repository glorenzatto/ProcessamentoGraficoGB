#include "CharacterController.h"
#include "TileMap.cpp" // Incluindo o TileMap diretamente aqui
#include <iostream>

CharacterController::CharacterController(TilemapView* tilemapView, TileMap* waterMap, std::vector<Tank>& tanks, int& tanksCollected, bool& isGameOver)
    : tilemapView(tilemapView), waterMap(waterMap), tanks(tanks), tanksCollected(tanksCollected), isGameOver(isGameOver), action(3), currentFrame(0), offsetX(0.0f), offsetY(0.0f) {}

void CharacterController::moveCharacter(int& characterX, int& characterY, const int direction) {
    int col = characterX;
    int row = characterY;
    tilemapView->computeTileWalking(col, row, direction);

    // Verifica se coletou algum tanque
    if (col == 5 && row == 13 && tanks[0].create) {
        tanksCollected++;
        tanks[0].create = false;
        std::cout << "Voce coletou " << tanksCollected << "/" << tanks.size() << " tanques de grito.\n" << std::endl;
    }
    else if (col == 12 && row == 12 && tanks[1].create) {
        tanksCollected++;
        tanks[1].create = false;
        std::cout << "Voce coletou " << tanksCollected << "/" << tanks.size() << " tanques de grito.\n" << std::endl;
    }
    else if (col == 9 && row == 8 && tanks[2].create) {
        tanksCollected++;
        tanks[2].create = false;
        std::cout << "Voce coletou " << tanksCollected << "/" << tanks.size() << " tanques de grito.\n" << std::endl;
    }
    else if (col == 0 && row == 7 && tanks[3].create) {
        tanksCollected++;
        tanks[3].create = false;
        std::cout << "Voce coletou " << tanksCollected << "/" << tanks.size() << " tanques de grito.\n" << std::endl;
    }
    else if (col == 6 && row == 0 && tanks[4].create) {
        tanksCollected++;
        tanks[4].create = false;
        std::cout << "Voce coletou " << tanksCollected << "/" << tanks.size() << " tanques de grito.\n" << std::endl;
    }
    else if (col == 11 && row == 0 && tanks[5].create) {
        tanksCollected++;
        tanks[5].create = false;
        std::cout << "Voce coletou " << tanksCollected << "/" << tanks.size() << " tanques de grito.\n" << std::endl;
    }

    // Verifica se o personagem saiu do mapa
    if ((col < 0) || (col >= waterMap->getWidth()) || (row < 0) || (row >= waterMap->getHeight())) {
        std::cout << "Fim de jogo! Voce deixou o Sully sair do mapa. Pressione [espaco] para jogar novamente.\n" << std::endl;
        isGameOver = true;
        restart(characterX, characterY); // Reinicia o jogo
        return;
    }

    // Verifica se o personagem chegou ao destino final
    if (col == 14 && row == 10) {
        if (tanksCollected == tanks.size()) {
            std::cout << "Parabens! Voce cumpriu o objetivo com sucesso e venceu o jogo, Sully eh o monstro mais assustador agora! Pressione [espaco] para jogar novamente.\n" << std::endl;
            isGameOver = true;
        }
        else {
            std::cout << "Espertinho! Voce ainda nao coletou todos os tanques de gritos. Colete-os e volte aqui para ganhar o jogo e fazer Sully ser o monstro mais assustador!\n" << std::endl;
        }
        return;
    }

    // Verifica se o personagem caiu na água
    unsigned char tileId = waterMap->getTile(col, row);
    if (tileId == 0) {
        std::cout << "Fim de jogo! Voce deixou o Sully cair na agua. Pressione [espaco] para jogar novamente.\n" << std::endl;
        isGameOver = true;
        restart(characterX, characterY); // Reinicia o jogo
        return;
    }

    characterX = col;
    characterY = row;
}

void CharacterController::restart(int& characterX, int& characterY) {
    isGameOver = false;
    tanksCollected = 0;
    action = 3;
    currentFrame = 0;
    offsetX = 0.0f;
    offsetY = 0.0f;

    for (auto& tank : tanks) {
        tank.create = true;
    }

    characterX = 0;
    characterY = 0;
}
