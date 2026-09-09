#include "Game.h"

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

	currentPlayer = 1;
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