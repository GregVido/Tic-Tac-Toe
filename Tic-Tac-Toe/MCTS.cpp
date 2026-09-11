#include "MCTS.h"

#include <array>
#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <limits>
#include <algorithm>

namespace
{
	using SimBoard = std::array<int, 9>;

	std::mt19937 rng(std::random_device{}());

	int OtherPlayer(int player)
	{
		return (player == 1) ? 2 : 1;
	}

	SimBoard CopyBoard(const int board[3][3])
	{
		SimBoard result{};

		for (int row = 0; row < 3; row++)
		{
			for (int column = 0; column < 3; column++)
			{
				result[row * 3 + column] = board[row][column];
			}
		}

		return result;
	}

	int CheckWinner(const SimBoard& board)
	{
		constexpr int lines[8][3] =
		{
			{ 0, 1, 2 },
			{ 3, 4, 5 },
			{ 6, 7, 8 },

			{ 0, 3, 6 },
			{ 1, 4, 7 },
			{ 2, 5, 8 },

			{ 0, 4, 8 },
			{ 2, 4, 6 }
		};

		for (const auto& line : lines)
		{
			int player = board[line[0]];

			if (player != 0 &&
				player == board[line[1]] &&
				player == board[line[2]])
			{
				return player;
			}
		}

		// Vérifie s'il reste une case libre
		for (int cell : board)
		{
			if (cell == 0)
				return 0;
		}

		// Match nul
		return 3;
	}

	std::vector<int> GetLegalMoves(const SimBoard& board)
	{
		std::vector<int> moves;

		for (int i = 0; i < 9; i++)
		{
			if (board[i] == 0)
			{
				moves.push_back(i);
			}
		}

		return moves;
	}

	struct MCTSNode
	{
		SimBoard state{};

		int playerToMove = 1;
		int move = -1;

		MCTSNode* parent = nullptr;

		int visits = 0;
		double score = 0.0;

		std::vector<int> untriedMoves;
		std::vector<std::unique_ptr<MCTSNode>> children;

		MCTSNode(
			const SimBoard& board,
			int player,
			MCTSNode* parentNode = nullptr,
			int playedMove = -1
		)
			:
			state(board),
			playerToMove(player),
			move(playedMove),
			parent(parentNode),
			untriedMoves(GetLegalMoves(board))
		{
		}
	};

	MCTSNode* SelectChild(
		MCTSNode* node,
		int aiPlayer
	)
	{
		MCTSNode* bestChild = nullptr;

		double bestValue =
			-std::numeric_limits<double>::infinity();

		for (auto& childPtr : node->children)
		{
			MCTSNode* child = childPtr.get();

			if (child->visits == 0)
			{
				return child;
			}

			double average =
				child->score / child->visits;

			// Si c'est au tour de l'IA :
			// on préfère un bon score pour l'IA.
			//
			// Si c'est au tour de l'adversaire :
			// on suppose qu'il essaiera de nous gêner.
			double exploitation;

			if (node->playerToMove == aiPlayer)
			{
				exploitation = average;
			}
			else
			{
				exploitation = 1.0 - average;
			}

			constexpr double C = 1.414;

			double exploration =
				C * std::sqrt(
					std::log(
						static_cast<double>(
							std::max(1, node->visits)
							)
					) /
					child->visits
				);

			double value =
				exploitation + exploration;

			if (value > bestValue)
			{
				bestValue = value;
				bestChild = child;
			}
		}

		return bestChild;
	}

	MCTSNode* Expand(MCTSNode* node)
	{
		if (node->untriedMoves.empty())
			return node;

		std::uniform_int_distribution<int> distribution(
			0,
			static_cast<int>(node->untriedMoves.size()) - 1
		);

		int index = distribution(rng);

		int move = node->untriedMoves[index];

		// Retire le coup de la liste
		node->untriedMoves[index] =
			node->untriedMoves.back();

		node->untriedMoves.pop_back();

		SimBoard newState = node->state;

		newState[move] = node->playerToMove;

		int nextPlayer =
			OtherPlayer(node->playerToMove);

		auto child = std::make_unique<MCTSNode>(
			newState,
			nextPlayer,
			node,
			move
		);

		MCTSNode* childPointer = child.get();

		node->children.push_back(
			std::move(child)
		);

		return childPointer;
	}

	int RandomSimulation(
		SimBoard state,
		int playerToMove
	)
	{
		int result = CheckWinner(state);

		while (result == 0)
		{
			std::vector<int> moves =
				GetLegalMoves(state);

			if (moves.empty())
				break;

			std::uniform_int_distribution<int> distribution(
				0,
				static_cast<int>(moves.size()) - 1
			);

			int move =
				moves[distribution(rng)];

			state[move] = playerToMove;

			playerToMove =
				OtherPlayer(playerToMove);

			result = CheckWinner(state);
		}

		return result;
	}
}

std::pair<int, int> FindMCTSMove(
	const int board[3][3],
	int aiPlayer,
	int iterations
)
{
	SimBoard rootBoard = CopyBoard(board);

	std::vector<int> legalMoves =
		GetLegalMoves(rootBoard);

	if (legalMoves.empty())
	{
		return { -1, -1 };
	}

	// Un seul coup possible
	if (legalMoves.size() == 1)
	{
		int move = legalMoves[0];

		return {
			move / 3,
			move % 3
		};
	}

	MCTSNode root(
		rootBoard,
		aiPlayer
	);

	for (int iteration = 0;
		iteration < iterations;
		iteration++)
	{
		MCTSNode* node = &root;

		// ==============================
		// 1. Selection
		// ==============================

		while (
			CheckWinner(node->state) == 0 &&
			node->untriedMoves.empty() &&
			!node->children.empty()
			)
		{
			node = SelectChild(
				node,
				aiPlayer
			);
		}

		// ==============================
		// 2. Expansion
		// ==============================

		if (
			CheckWinner(node->state) == 0 &&
			!node->untriedMoves.empty()
			)
		{
			node = Expand(node);
		}

		// ==============================
		// 3. Simulation
		// ==============================

		int result = RandomSimulation(
			node->state,
			node->playerToMove
		);

		double reward = 0.0;

		if (result == aiPlayer)
		{
			reward = 1.0;
		}
		else if (result == 3)
		{
			reward = 0.5;
		}

		// ==============================
		// 4. Backpropagation
		// ==============================

		while (node != nullptr)
		{
			node->visits++;
			node->score += reward;

			node = node->parent;
		}
	}

	// Choisit la branche la plus explorée
	MCTSNode* bestChild = nullptr;

	int bestVisits = -1;

	for (auto& child : root.children)
	{
		if (child->visits > bestVisits)
		{
			bestVisits = child->visits;
			bestChild = child.get();
		}
	}

	if (bestChild == nullptr)
	{
		return { -1, -1 };
	}

	return {
		bestChild->move / 3,
		bestChild->move % 3
	};
}