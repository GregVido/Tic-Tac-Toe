// Tic-Tac-Toe.cpp : Définit le point d'entrée de l'application.
//

#include "framework.h"
#include "Tic-Tac-Toe.h"

#include "Game.h"
#include "Menu.h"

#define MAX_LOADSTRING 100

// Variables globales :
HINSTANCE hInst;                                // instance actuelle
WCHAR szTitle[MAX_LOADSTRING];                  // Texte de la barre de titre
WCHAR szWindowClass[MAX_LOADSTRING];            // nom de la classe de fenêtre principale

// Déclarations anticipées des fonctions incluses dans ce module de code :
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
	hInst = hInstance;

	const int windowWidth = 700;
	const int windowHeight = 700;

	// Taille de l'écran
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Position pour centrer la fenêtre
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	DWORD windowStyle =
		WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX;

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		windowStyle,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
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

		// ==============================
		// Boutons de fin de partie
		// ==============================

		if (winner != 0)
		{
			EndGameLayout endLayout = GetEndGameLayout(
				windowWidth,
				boardY
			);

			HFONT buttonFont = CreateFontW(
				20,
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

			HFONT oldButtonFont =
				(HFONT)SelectObject(hdc, buttonFont);

			DrawMenuButton(
				hdc,
				endLayout.restartButton,
				L"RECOMMENCER",
				true,
				RGB(50, 150, 70)
			);

			DrawMenuButton(
				hdc,
				endLayout.changePlayersButton,
				L"CHANGER LES JOUEURS",
				true,
				RGB(80, 80, 80)
			);

			SelectObject(hdc, oldButtonFont);
			DeleteObject(buttonFont);
		}

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
				// Vérifie si J1 utilise un bot pas encore disponible
				bool player1BotUnavailable =
					player1Type == PlayerType::Bot &&
					player1Difficulty != BotDifficulty::Easy;

				// Vérifie si J2 utilise un bot pas encore disponible
				bool player2BotUnavailable =
					player2Type == PlayerType::Bot &&
					player2Difficulty != BotDifficulty::Easy;

				// Moyen / Difficile encore indisponibles
				if (player1BotUnavailable || player2BotUnavailable)
				{
					MessageBoxW(
						hWnd,
						L"Les bots Moyen et Difficile sont actuellement "
						L"en cours de développement.\n\n"
						L"Le bot Facile est disponible.",
						L"Fonctionnalité en développement",
						MB_OK | MB_ICONINFORMATION
					);

					return 0;
				}

				// Lance la partie
				currentScreen = Screen::Game;

				ResetGame();

				// Si le joueur tiré au sort est un bot facile,
				// il joue immédiatement
				if (IsCurrentPlayerBot())
				{
					PlayEasyBotMove();
				}

				InvalidateRect(hWnd, nullptr, TRUE);

				return 0;
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

		// ==============================
		// Clic sur les boutons de fin
		// ==============================

		if (winner != 0)
		{
			EndGameLayout endLayout = GetEndGameLayout(
				windowWidth,
				boardY
			);

			POINT mousePoint = {
				mouseX,
				mouseY
			};

			// Rejouer avec les mêmes joueurs
			if (PtInRect(&endLayout.restartButton, mousePoint))
			{
				ResetGame();

				InvalidateRect(hWnd, nullptr, TRUE);

				return 0;
			}

			// Retourner au menu de sélection
			if (PtInRect(&endLayout.changePlayersButton, mousePoint))
			{
				ResetGame();

				currentScreen = Screen::Menu;

				InvalidateRect(hWnd, nullptr, TRUE);

				return 0;
			}
		}

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

				// Coup du bot
				if (winner == 0 && IsCurrentPlayerBot())
				{
					PlayEasyBotMove();
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
