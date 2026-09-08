// Tic-Tac-Toe.cpp : Définit le point d'entrée de l'application.
//

#include "framework.h"
#include "Tic-Tac-Toe.h"

#include <random>

#define MAX_LOADSTRING 100

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

Screen currentScreen = Screen::Menu;

PlayerType player1Type = PlayerType::Human;
PlayerType player2Type = PlayerType::Human;

BotDifficulty player1Difficulty = BotDifficulty::Easy;
BotDifficulty player2Difficulty = BotDifficulty::Easy;

int currentPlayer = 1;
int winner = 0;

// 0 = vide
// 1 = X
// 2 = O
int board[3][3] = {};

// Générateur aléatoire
std::random_device rd;
std::mt19937 randomGenerator(rd());
std::uniform_int_distribution<int> randomSymbol(1, 2);

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct MenuLayout
{
	RECT j1Human;
	RECT j1Bot;

	RECT j1Easy;
	RECT j1Medium;
	RECT j1Hard;

	RECT j2Human;
	RECT j2Bot;

	RECT j2Easy;
	RECT j2Medium;
	RECT j2Hard;

	RECT startButton;
};

struct EndGameLayout
{
	RECT restartButton;
	RECT changePlayersButton;
};

int CheckWinner();

void ResetGame()
{
	ZeroMemory(board, sizeof(board));

	currentPlayer = 1;
	winner = 0;
}

MenuLayout GetMenuLayout(int windowWidth)
{
	MenuLayout layout{};

	int center = windowWidth / 2;

	// Panneau joueur 1
	int j1X = center - 300;

	// Panneau joueur 2
	int j2X = center + 20;

	// Humain / Bot
	layout.j1Human = { j1X, 180, j1X + 125, 225 };
	layout.j1Bot = { j1X + 135, 180, j1X + 260, 225 };

	layout.j2Human = { j2X, 180, j2X + 125, 225 };
	layout.j2Bot = { j2X + 135, 180, j2X + 260, 225 };

	// Difficultés joueur 1
	layout.j1Easy = { j1X,       290, j1X + 80, 335 };
	layout.j1Medium = { j1X + 90,  290, j1X + 170, 335 };
	layout.j1Hard = { j1X + 180, 290, j1X + 260, 335 };

	// Difficultés joueur 2
	layout.j2Easy = { j2X,       290, j2X + 80, 335 };
	layout.j2Medium = { j2X + 90,  290, j2X + 170, 335 };
	layout.j2Hard = { j2X + 180, 290, j2X + 260, 335 };

	// Bouton Jouer
	layout.startButton = {
		center - 110,
		430,
		center + 110,
		490
	};

	return layout;
}

void DrawMenuButton(
	HDC hdc,
	const RECT& rect,
	const wchar_t* text,
	bool selected,
	COLORREF selectedColor)
{
	HBRUSH backgroundBrush;

	if (selected)
		backgroundBrush = CreateSolidBrush(selectedColor);
	else
		backgroundBrush = CreateSolidBrush(RGB(235, 235, 235));

	FillRect(hdc, &rect, backgroundBrush);

	FrameRect(
		hdc,
		&rect,
		(HBRUSH)GetStockObject(BLACK_BRUSH)
	);

	SetBkMode(hdc, TRANSPARENT);

	if (selected)
		SetTextColor(hdc, RGB(255, 255, 255));
	else
		SetTextColor(hdc, RGB(30, 30, 30));

	RECT textRect = rect;

	DrawTextW(
		hdc,
		text,
		-1,
		&textRect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE
	);

	DeleteObject(backgroundBrush);
}

void DrawMenu(HDC hdc, int windowWidth)
{
	MenuLayout layout = GetMenuLayout(windowWidth);

	SetBkMode(hdc, TRANSPARENT);

	// ==============================
	// Titre
	// ==============================

	HFONT titleFont = CreateFontW(
		42,
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Segoe UI"
	);

	HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);

	SetTextColor(hdc, RGB(30, 30, 30));

	RECT titleRect = {
		0,
		40,
		windowWidth,
		100
	};

	DrawTextW(
		hdc,
		L"TIC TAC TOE",
		-1,
		&titleRect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE
	);

	SelectObject(hdc, oldFont);
	DeleteObject(titleFont);

	// ==============================
	// Police normale
	// ==============================

	HFONT menuFont = CreateFontW(
		22,
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Segoe UI"
	);

	oldFont = (HFONT)SelectObject(hdc, menuFont);

	int center = windowWidth / 2;

	// ==============================
	// Joueur 1
	// ==============================

	RECT j1Title = {
		center - 300,
		115,
		center - 40,
		160
	};

	SetTextColor(hdc, RGB(220, 40, 40));

	DrawTextW(
		hdc,
		L"Joueur 1 - X",
		-1,
		&j1Title,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE
	);

	DrawMenuButton(
		hdc,
		layout.j1Human,
		L"Humain",
		player1Type == PlayerType::Human,
		RGB(220, 40, 40)
	);

	DrawMenuButton(
		hdc,
		layout.j1Bot,
		L"Bot",
		player1Type == PlayerType::Bot,
		RGB(220, 40, 40)
	);

	// ==============================
	// Joueur 2
	// ==============================

	RECT j2Title = {
		center + 20,
		115,
		center + 280,
		160
	};

	SetTextColor(hdc, RGB(40, 100, 220));

	DrawTextW(
		hdc,
		L"Joueur 2 - O",
		-1,
		&j2Title,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE
	);

	DrawMenuButton(
		hdc,
		layout.j2Human,
		L"Humain",
		player2Type == PlayerType::Human,
		RGB(40, 100, 220)
	);

	DrawMenuButton(
		hdc,
		layout.j2Bot,
		L"Bot",
		player2Type == PlayerType::Bot,
		RGB(40, 100, 220)
	);

	// ==============================
	// Difficulté joueur 1
	// ==============================

	if (player1Type == PlayerType::Bot)
	{
		RECT difficultyTitle = {
			center - 300,
			240,
			center - 40,
			280
		};

		SetTextColor(hdc, RGB(60, 60, 60));

		DrawTextW(
			hdc,
			L"Difficulté",
			-1,
			&difficultyTitle,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE
		);

		DrawMenuButton(
			hdc,
			layout.j1Easy,
			L"Facile",
			player1Difficulty == BotDifficulty::Easy,
			RGB(220, 40, 40)
		);

		DrawMenuButton(
			hdc,
			layout.j1Medium,
			L"Moyen",
			player1Difficulty == BotDifficulty::Medium,
			RGB(220, 40, 40)
		);

		DrawMenuButton(
			hdc,
			layout.j1Hard,
			L"Difficile",
			player1Difficulty == BotDifficulty::Hard,
			RGB(220, 40, 40)
		);
	}

	// ==============================
	// Difficulté joueur 2
	// ==============================

	if (player2Type == PlayerType::Bot)
	{
		RECT difficultyTitle = {
			center + 20,
			240,
			center + 280,
			280
		};

		SetTextColor(hdc, RGB(60, 60, 60));

		DrawTextW(
			hdc,
			L"Difficulté",
			-1,
			&difficultyTitle,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE
		);

		DrawMenuButton(
			hdc,
			layout.j2Easy,
			L"Facile",
			player2Difficulty == BotDifficulty::Easy,
			RGB(40, 100, 220)
		);

		DrawMenuButton(
			hdc,
			layout.j2Medium,
			L"Moyen",
			player2Difficulty == BotDifficulty::Medium,
			RGB(40, 100, 220)
		);

		DrawMenuButton(
			hdc,
			layout.j2Hard,
			L"Difficile",
			player2Difficulty == BotDifficulty::Hard,
			RGB(40, 100, 220)
		);
	}

	// ==============================
	// Jouer
	// ==============================

	DrawMenuButton(
		hdc,
		layout.startButton,
		L"JOUER",
		true,
		RGB(50, 150, 70)
	);

	SelectObject(hdc, oldFont);
	DeleteObject(menuFont);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Placez le code ici.

	// Initialise les chaînes globales
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Effectue l'initialisation de l'application :
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

	MSG msg;

	// Boucle de messages principale :
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FONCTION : MyRegisterClass()
//
//  OBJECTIF : Inscrit la classe de fenêtre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TICTACTOE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FONCTION : InitInstance(HINSTANCE, int)
//
//   OBJECTIF : enregistre le handle d'instance et crée une fenêtre principale
//
//   COMMENTAIRES :
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        nous créons et affichons la fenêtre principale du programme.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Stocke le handle d'instance dans la variable globale

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		700,
		700,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
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

	// Diagonale \

	if (board[0][0] != 0 &&
	board[0][0] == board[1][1] &&
		board[1][1] == board[2][2])
		{
			return board[0][0];
	}

	// Diagonale /
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
				return 0;
		}
	}

	// Plus aucune case vide et aucun gagnant
	return 3;
}

//
//  FONCTION : WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  OBJECTIF : Traite les messages pour la fenêtre principale.
//
//  WM_COMMAND  - traite le menu de l'application
//  WM_PAINT    - Dessine la fenêtre principale
//  WM_DESTROY  - génère un message d'arrêt et retourne
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Analyse les sélections de menu :
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// Récupère la taille intérieure de la fenêtre
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		int windowWidth = clientRect.right - clientRect.left;
		int windowHeight = clientRect.bottom - clientRect.top;

		if (currentScreen == Screen::Menu)
		{
			DrawMenu(hdc, windowWidth);

			EndPaint(hWnd, &ps);
			return 0;
		}

		// Position du plateau pour le centrer
		int boardX = (windowWidth - BOARD_SIZE) / 2;
		int boardY = (windowHeight - BOARD_SIZE) / 2;

		// ==============================
		// Texte du joueur actuel
		// ==============================

		HFONT playerFont = CreateFontW(
			32,
			0,
			0,
			0,
			FW_BOLD,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			L"Segoe UI"
		);

		HFONT oldFont = (HFONT)SelectObject(hdc, playerFont);

		SetBkMode(hdc, TRANSPARENT);

		RECT playerTextRect = {
			0,
			boardY - 60,
			windowWidth,
			boardY - 10
		};

		if (winner == 1)
		{
			// Joueur 1 gagne
			SetTextColor(hdc, RGB(220, 40, 40));

			DrawTextW(
				hdc,
				L"Joueur 1 a gagné !",
				-1,
				&playerTextRect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE
			);
		}
		else if (winner == 2)
		{
			// Joueur 2 gagne
			SetTextColor(hdc, RGB(40, 100, 220));

			DrawTextW(
				hdc,
				L"Joueur 2 a gagné !",
				-1,
				&playerTextRect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE
			);
		}
		else if (winner == 3)
		{
			// Match nul
			SetTextColor(hdc, RGB(50, 50, 50));

			DrawTextW(
				hdc,
				L"Match nul !",
				-1,
				&playerTextRect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE
			);
		}
		else if (currentPlayer == 1)
		{
			// Tour joueur 1
			SetTextColor(hdc, RGB(220, 40, 40));

			DrawTextW(
				hdc,
				L"Au tour du Joueur 1 (X)",
				-1,
				&playerTextRect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE
			);
		}
		else
		{
			// Tour joueur 2
			SetTextColor(hdc, RGB(40, 100, 220));

			DrawTextW(
				hdc,
				L"Au tour du Joueur 2 (O)",
				-1,
				&playerTextRect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE
			);
		}

		SelectObject(hdc, oldFont);
		DeleteObject(playerFont);

		// Bordure extérieure
		Rectangle(
			hdc,
			boardX,
			boardY,
			boardX + BOARD_SIZE,
			boardY + BOARD_SIZE
		);

		// Lignes verticales
		for (int i = 1; i < 3; i++)
		{
			int x = boardX + i * CELL_SIZE;

			MoveToEx(hdc, x, boardY, nullptr);
			LineTo(hdc, x, boardY + BOARD_SIZE);
		}

		// Lignes horizontales
		for (int i = 1; i < 3; i++)
		{
			int y = boardY + i * CELL_SIZE;

			MoveToEx(hdc, boardX, y, nullptr);
			LineTo(hdc, boardX + BOARD_SIZE, y);
		}

		// Stylos pour les symboles
		HPEN redPen = CreatePen(
			PS_SOLID,
			8,
			RGB(220, 40, 40)
		);

		HPEN bluePen = CreatePen(
			PS_SOLID,
			8,
			RGB(40, 100, 220)
		);

		HPEN oldPen = (HPEN)GetCurrentObject(hdc, OBJ_PEN);

		constexpr int MARGIN = 30;

		for (int row = 0; row < 3; row++)
		{
			for (int column = 0; column < 3; column++)
			{
				int cellX = boardX + column * CELL_SIZE;
				int cellY = boardY + row * CELL_SIZE;

				// X rouge
				if (board[row][column] == 1)
				{
					SelectObject(hdc, redPen);

					MoveToEx(
						hdc,
						cellX + MARGIN,
						cellY + MARGIN,
						nullptr
					);

					LineTo(
						hdc,
						cellX + CELL_SIZE - MARGIN,
						cellY + CELL_SIZE - MARGIN
					);

					MoveToEx(
						hdc,
						cellX + CELL_SIZE - MARGIN,
						cellY + MARGIN,
						nullptr
					);

					LineTo(
						hdc,
						cellX + MARGIN,
						cellY + CELL_SIZE - MARGIN
					);
				}

				// O bleu
				else if (board[row][column] == 2)
				{
					SelectObject(hdc, bluePen);

					Ellipse(
						hdc,
						cellX + MARGIN,
						cellY + MARGIN,
						cellX + CELL_SIZE - MARGIN,
						cellY + CELL_SIZE - MARGIN
					);
				}
			}
		}

		// Remet le stylo précédent
		SelectObject(hdc, oldPen);

		// Libère les stylos
		DeleteObject(redPen);
		DeleteObject(bluePen);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		// Position de la souris
		int mouseX = LOWORD(lParam);
		int mouseY = HIWORD(lParam);

		if (currentScreen == Screen::Menu)
		{
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);

			int windowWidth = clientRect.right - clientRect.left;

			MenuLayout layout = GetMenuLayout(windowWidth);

			POINT mousePoint = {
				mouseX,
				mouseY
			};

			// ==============================
			// Joueur 1
			// ==============================

			if (PtInRect(&layout.j1Human, mousePoint))
			{
				player1Type = PlayerType::Human;
			}
			else if (PtInRect(&layout.j1Bot, mousePoint))
			{
				player1Type = PlayerType::Bot;
			}

			// Difficulté J1
			else if (
				player1Type == PlayerType::Bot &&
				PtInRect(&layout.j1Easy, mousePoint))
			{
				player1Difficulty = BotDifficulty::Easy;
			}
			else if (
				player1Type == PlayerType::Bot &&
				PtInRect(&layout.j1Medium, mousePoint))
			{
				player1Difficulty = BotDifficulty::Medium;
			}
			else if (
				player1Type == PlayerType::Bot &&
				PtInRect(&layout.j1Hard, mousePoint))
			{
				player1Difficulty = BotDifficulty::Hard;
			}

			// ==============================
			// Joueur 2
			// ==============================

			else if (PtInRect(&layout.j2Human, mousePoint))
			{
				player2Type = PlayerType::Human;
			}
			else if (PtInRect(&layout.j2Bot, mousePoint))
			{
				player2Type = PlayerType::Bot;
			}

			// Difficulté J2
			else if (
				player2Type == PlayerType::Bot &&
				PtInRect(&layout.j2Easy, mousePoint))
			{
				player2Difficulty = BotDifficulty::Easy;
			}
			else if (
				player2Type == PlayerType::Bot &&
				PtInRect(&layout.j2Medium, mousePoint))
			{
				player2Difficulty = BotDifficulty::Medium;
			}
			else if (
				player2Type == PlayerType::Bot &&
				PtInRect(&layout.j2Hard, mousePoint))
			{
				player2Difficulty = BotDifficulty::Hard;
			}

			// ==============================
			// Jouer
			// ==============================

			else if (PtInRect(&layout.startButton, mousePoint))
			{
				currentScreen = Screen::Game;

				// Nouvelle partie
				ResetGame();
			}

			InvalidateRect(hWnd, nullptr, TRUE);

			return 0;
		}

		// Taille actuelle de la fenêtre
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		int windowWidth = clientRect.right - clientRect.left;
		int windowHeight = clientRect.bottom - clientRect.top;

		// Position du plateau
		int boardX = (windowWidth - BOARD_SIZE) / 2;
		int boardY = (windowHeight - BOARD_SIZE) / 2;

		// Vérifie si on a cliqué à l'intérieur du plateau
		if (mouseX >= boardX &&
			mouseX < boardX + BOARD_SIZE &&
			mouseY >= boardY &&
			mouseY < boardY + BOARD_SIZE)
		{
			// Trouve la colonne et la ligne
			int column = (mouseX - boardX) / CELL_SIZE;
			int row = (mouseY - boardY) / CELL_SIZE;

			// Seulement si la case est vide
			if (board[row][column] == 0 && winner == 0)
			{
				// Place le symbole
				board[row][column] = currentPlayer;

				// Vérifie si ce coup termine la partie
				winner = CheckWinner();

				// Change de joueur seulement si la partie continue
				if (winner == 0)
				{
					currentPlayer = (currentPlayer == 1) ? 2 : 1;
				}

				InvalidateRect(hWnd, nullptr, TRUE);
			}
		}



		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
