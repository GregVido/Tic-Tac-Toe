#pragma once

constexpr int BOARD_SIZE = 450;
constexpr int CELL_SIZE = BOARD_SIZE / 3;

enum class Screen
{
	Menu,
	Game
};

enum class PlayerType
{
	Human,
	Bot
};

enum class BotDifficulty
{
	Easy,
	Medium,
	Hard
};

// État de l'application
extern Screen currentScreen;

// Configuration joueurs
extern PlayerType player1Type;
extern PlayerType player2Type;

extern BotDifficulty player1Difficulty;
extern BotDifficulty player2Difficulty;

// État de la partie
extern int currentPlayer;
extern int winner;

extern int board[3][3];

// Fonctions
void ResetGame();
int CheckWinner();
bool IsCurrentPlayerBot();
void PlayEasyBotMove();
void PlayMediumBotMove();
void PlayBotMove();