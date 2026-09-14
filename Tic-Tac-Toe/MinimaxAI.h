#pragma once

#include <utility>

// Retourne {ligne, colonne} du meilleur coup.
// {-1, -1} si aucun coup possible.
std::pair<int, int> FindBestMinimaxMove(
	const int board[3][3],
	int aiPlayer
);