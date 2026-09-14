#include "MinimaxAI.h"

#include <algorithm>

namespace
{
	int OtherPlayer(int player)
	{
		return (player == 1) ? 2 : 1;
	}

	int CheckWinnerLocal(const int board[3][3])
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

		// Vérifie s'il reste une case vide
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

	int Minimax(
		int board[3][3],
		int depth,
		bool maximizing,
		int alpha,
		int beta,
		int aiPlayer
	)
	{
		int result = CheckWinnerLocal(board);

		int opponent = OtherPlayer(aiPlayer);

		// L'IA gagne
		if (result == aiPlayer)
		{
			return 10 - depth;
		}

		// L'adversaire gagne
		if (result == opponent)
		{
			return depth - 10;
		}

		// Match nul
		if (result == 3)
		{
			return 0;
		}

		// ==============================
		// Tour de l'IA : MAX
		// ==============================

		if (maximizing)
		{
			int bestScore = -1000;

			for (int row = 0; row < 3; row++)
			{
				for (int column = 0; column < 3; column++)
				{
					if (board[row][column] != 0)
						continue;

					// Simule le coup
					board[row][column] = aiPlayer;

					int score = Minimax(
						board,
						depth + 1,
						false,
						alpha,
						beta,
						aiPlayer
					);

					// Annule le coup
					board[row][column] = 0;

					bestScore = std::max(
						bestScore,
						score
					);

					alpha = std::max(
						alpha,
						bestScore
					);

					// Élagage alpha-bêta
					if (beta <= alpha)
					{
						return bestScore;
					}
				}
			}

			return bestScore;
		}

		// ==============================
		// Tour adversaire : MIN
		// ==============================

		else
		{
			int bestScore = 1000;

			for (int row = 0; row < 3; row++)
			{
				for (int column = 0; column < 3; column++)
				{
					if (board[row][column] != 0)
						continue;

					// Simule le coup adverse
					board[row][column] = opponent;

					int score = Minimax(
						board,
						depth + 1,
						true,
						alpha,
						beta,
						aiPlayer
					);

					// Annule le coup
					board[row][column] = 0;

					bestScore = std::min(
						bestScore,
						score
					);

					beta = std::min(
						beta,
						bestScore
					);

					// Élagage alpha-bêta
					if (beta <= alpha)
					{
						return bestScore;
					}
				}
			}

			return bestScore;
		}
	}
}

std::pair<int, int> FindBestMinimaxMove(
	const int board[3][3],
	int aiPlayer
)
{
	// Copie du plateau réel
	int simulationBoard[3][3];

	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			simulationBoard[row][column] =
				board[row][column];
		}
	}

	int bestScore = -1000;

	int bestRow = -1;
	int bestColumn = -1;

	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 3; column++)
		{
			if (simulationBoard[row][column] != 0)
				continue;

			// Simule le coup de l'IA
			simulationBoard[row][column] = aiPlayer;

			int score = Minimax(
				simulationBoard,
				0,
				false,
				-1000,
				1000,
				aiPlayer
			);

			// Annule
			simulationBoard[row][column] = 0;

			if (score > bestScore)
			{
				bestScore = score;

				bestRow = row;
				bestColumn = column;
			}
		}
	}

	return {
		bestRow,
		bestColumn
	};
}