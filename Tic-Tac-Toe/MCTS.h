#pragma once

#include <utility>

// Retourne {row, column}
// {-1, -1} si aucun coup n'est possible
std::pair<int, int> FindMCTSMove(
	const int board[3][3],
	int aiPlayer,
	int iterations = 2
);