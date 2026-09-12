#include "Game.h"
#include "MCTS.h"

#include <random>
#include <vector>
#include <utility>

std::random_device rd;
std::mt19937 randomGenerator(rd());
std::uniform_int_distribution<int> randomPlayer(1, 2);

Screen currentScreen = Screen::Menu;

PlayerType player1Type = PlayerType::Human;
PlayerType player2Type = PlayerType::Human;

BotDifficulty player1Difficulty = BotDifficulty::Easy;
BotDifficulty player2Difficulty = BotDifficulty::Easy;

int currentPlayer = 1;
int winner = 0;

int board[3][3] = {};

void ResetGame()
{
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			board[row][column] = 0;
		}
	}

	// Joueur qui commence choisi aléatoirement
	currentPlayer = randomPlayer(randomGenerator);

	winner = 0;
}

int CheckWinner()
{
	// Lignes
	for (int row = 0; row < 3; row++)
	{
		if (board[row][0] != 0 &&
			board[row][0] == board[row][1] &&
			board[row][1] == board[row][2])
		{
			return board[row][0];
		}
	}

	// Colonnes
	for (int column = 0; column < 3; column++)
	{
		if (board[0][column] != 0 &&
			board[0][column] == board[1][column] &&
			board[1][column] == board[2][column])
		{
			return board[0][column];
		}
	}

	// Diagonale descendante
	if (board[0][0] != 0 &&
		board[0][0] == board[1][1] &&
		board[1][1] == board[2][2])
	{
		return board[0][0];
	}

	// Diagonale montante
	if (board[0][2] != 0 &&
		board[0][2] == board[1][1] &&
		board[1][1] == board[2][0])
	{
		return board[0][2];
	}

	// Case vide restante ?
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			if (board[row][column] == 0)
			{
				return 0;
			}
		}
	}

	// Match nul
	return 3;
}

bool IsCurrentPlayerBot()
{
	if (currentPlayer == 1)
	{
		return player1Type == PlayerType::Bot;
	}
	else
	{
		return player2Type == PlayerType::Bot;
	}
}

void PlayEasyBotMove()
{
	if (winner != 0)
		return;

	std::vector<std::pair<int, int>> emptyCells;

	// Cherche toutes les cases libres
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			if (board[row][column] == 0)
			{
				emptyCells.push_back({ row, column });
			}
		}
	}

	if (emptyCells.empty())
		return;

	// Choisit une case libre au hasard
	std::uniform_int_distribution<int> distribution(
		0,
		static_cast<int>(emptyCells.size()) - 1
	);

	int index = distribution(randomGenerator);

	int row = emptyCells[index].first;
	int column = emptyCells[index].second;

	// Joue
	board[row][column] = currentPlayer;

	// Vérifie la victoire
	winner = CheckWinner();

	// Change de joueur
	if (winner == 0)
	{
		currentPlayer = (currentPlayer == 1) ? 2 : 1;
	}
}

void PlayMediumBotMove()
{
	if (winner != 0)
		return;

	// 200 simulations = niveau moyen
	auto [row, column] = FindMCTSMove(
		board,
		currentPlayer,
		10
	);

	if (row < 0 || column < 0)
		return;

	board[row][column] = currentPlayer;

	winner = CheckWinner();

	if (winner == 0)
	{
		currentPlayer =
			(currentPlayer == 1) ? 2 : 1;
	}
}

void PlayBotMove()
{
	if (winner != 0)
		return;

	if (!IsCurrentPlayerBot())
		return;

	BotDifficulty difficulty;

	if (currentPlayer == 1)
	{
		difficulty = player1Difficulty;
	}
	else
	{
		difficulty = player2Difficulty;
	}

	switch (difficulty)
	{
	case BotDifficulty::Easy:
		PlayEasyBotMove();
		break;

	case BotDifficulty::Medium:
		PlayMediumBotMove();
		break;

	case BotDifficulty::Hard:
		// Pas encore implémenté
		break;
	}
}