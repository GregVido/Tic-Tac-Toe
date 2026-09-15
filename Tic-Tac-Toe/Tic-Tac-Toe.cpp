// Tic-Tac-Toe.cpp : fenêtre principale Win32

#include "framework.h"
#include "Tic-Tac-Toe.h"

#include "Game.h"
#include "Menu.h"
#include "UI.h"

#include <algorithm>
#include <cmath>
#include <cwchar>
#include <dwmapi.h>
#include <windowsx.h>

#pragma comment(lib, "Dwmapi.lib")

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#define MAX_LOADSTRING 100
#define TIMER_BOT 1
#define TIMER_UI 2

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

namespace
{
	POINT gMousePosition{ -1000, -1000 };
	bool gMouseTracking = false;

	int gAnimatedRow = -1;
	int gAnimatedColumn = -1;
	float gMoveAnimation = 1.0f;
	float gWinAnimation = 1.0f;

	float EaseOutBack(float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		constexpr float c1 = 1.70158f;
		constexpr float c3 = c1 + 1.0f;
		float p = t - 1.0f;
		return 1.0f + c3 * p * p * p + c1 * p * p;
	}

	void BeginMoveAnimation(int row, int column)
	{
		gAnimatedRow = row;
		gAnimatedColumn = column;
		gMoveAnimation = 0.0f;
	}

	void ResetGameAnimations()
	{
		gAnimatedRow = -1;
		gAnimatedColumn = -1;
		gMoveAnimation = 1.0f;
		gWinAnimation = 1.0f;
	}

	bool FindNewMove(const int before[3][3], int& row, int& column)
	{
		for (int r = 0; r < 3; ++r)
		{
			for (int c = 0; c < 3; ++c)
			{
				if (before[r][c] == 0 && board[r][c] != 0)
				{
					row = r;
					column = c;
					return true;
				}
			}
		}

		return false;
	}

	bool GetWinningCells(int& row1, int& col1, int& row2, int& col2)
	{
		if (winner != 1 && winner != 2)
			return false;

		for (int row = 0; row < 3; ++row)
		{
			if (board[row][0] == winner && board[row][1] == winner && board[row][2] == winner)
			{
				row1 = row; col1 = 0;
				row2 = row; col2 = 2;
				return true;
			}
		}

		for (int column = 0; column < 3; ++column)
		{
			if (board[0][column] == winner && board[1][column] == winner && board[2][column] == winner)
			{
				row1 = 0; col1 = column;
				row2 = 2; col2 = column;
				return true;
			}
		}

		if (board[0][0] == winner && board[1][1] == winner && board[2][2] == winner)
		{
			row1 = 0; col1 = 0;
			row2 = 2; col2 = 2;
			return true;
		}

		if (board[0][2] == winner && board[1][1] == winner && board[2][0] == winner)
		{
			row1 = 0; col1 = 2;
			row2 = 2; col2 = 0;
			return true;
		}

		return false;
	}

	const wchar_t* DifficultyName(BotDifficulty difficulty)
	{
		switch (difficulty)
		{
		case BotDifficulty::Easy: return L"RANDOM";
		case BotDifficulty::Medium: return L"MCTS";
		case BotDifficulty::Hard: return L"MINIMAX";
		default: return L"BOT";
		}
	}

	void GetPlayerModeText(int player, wchar_t* output, size_t outputSize)
	{
		PlayerType type = player == 1 ? player1Type : player2Type;
		BotDifficulty difficulty = player == 1 ? player1Difficulty : player2Difficulty;

		if (type == PlayerType::Human)
		{
			swprintf_s(output, outputSize, L"J%d  •  HUMAIN", player);
		}
		else
		{
			swprintf_s(output, outputSize, L"J%d  •  %ls", player, DifficultyName(difficulty));
		}
	}

	void DrawPlayerChip(HDC hdc, RECT rect, int player)
	{
		const COLORREF accent = player == 1 ? UI::Red : UI::Blue;
		bool active = winner == 0 && currentPlayer == player;

		COLORREF fill = active ? UI::MixColor(UI::Surface, accent, 0.16f) : UI::Surface;
		COLORREF border = active ? accent : UI::Border;

		UI::DrawRoundedPanel(hdc, rect, 14, fill, border, 1);

		wchar_t label[64]{};
		GetPlayerModeText(player, label, 64);

		HFONT font = UI::CreateUiFont(12, FW_SEMIBOLD);
		UI::DrawCenteredText(hdc, rect, label, font, active ? UI::Text : UI::Muted);
		DeleteObject(font);
	}

	void DrawGame(HDC hdc, int windowWidth, int windowHeight)
	{
		RECT client{ 0, 0, windowWidth, windowHeight };
		UI::FillGradient(hdc, client, UI::BackgroundTop, UI::BackgroundBottom);
		SetBkMode(hdc, TRANSPARENT);

		const int boardX = (windowWidth - BOARD_SIZE) / 2;
		const int boardY = (windowHeight - BOARD_SIZE) / 2;

		RECT brandRect{ 28, 18, 260, 52 };
		HFONT brandFont = UI::CreateUiFont(17, FW_BOLD, L"Segoe UI Variable Display");
		UI::DrawCenteredText(hdc, brandRect, L"TIC TAC TOE", brandFont, UI::Text, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DeleteObject(brandFont);

		RECT chip1{ windowWidth - 308, 16, windowWidth - 168, 52 };
		RECT chip2{ windowWidth - 156, 16, windowWidth - 16, 52 };
		DrawPlayerChip(hdc, chip1, 1);
		DrawPlayerChip(hdc, chip2, 2);

		RECT statusCard{ boardX + 56, boardY - 76, boardX + BOARD_SIZE - 56, boardY - 22 };
		COLORREF statusAccent = currentPlayer == 1 ? UI::Red : UI::Blue;
		const wchar_t* statusText = currentPlayer == 1 ? L"Au tour du Joueur 1  •  X" : L"Au tour du Joueur 2  •  O";

		if (winner == 1)
		{
			statusAccent = UI::Red;
			statusText = L"Joueur 1 remporte la partie";
		}
		else if (winner == 2)
		{
			statusAccent = UI::Blue;
			statusText = L"Joueur 2 remporte la partie";
		}
		else if (winner == 3)
		{
			statusAccent = UI::Purple;
			statusText = L"Match nul  •  Belle partie";
		}

		UI::DrawShadow(hdc, statusCard, 18, 5);
		UI::DrawRoundedPanel(hdc, statusCard, 18, UI::MixColor(UI::Surface, statusAccent, 0.12f), statusAccent, 1);
		HFONT statusFont = UI::CreateUiFont(17, FW_BOLD, L"Segoe UI Variable Display");
		UI::DrawCenteredText(hdc, statusCard, statusText, statusFont, UI::Text);
		DeleteObject(statusFont);

		RECT boardPanel{ boardX - 14, boardY - 14, boardX + BOARD_SIZE + 14, boardY + BOARD_SIZE + 14 };
		UI::DrawShadow(hdc, boardPanel, 28, 10);
		UI::DrawRoundedPanel(hdc, boardPanel, 28, RGB(16, 21, 34), UI::Border, 1);

		int hoveredRow = -1;
		int hoveredColumn = -1;

		if (winner == 0 && !IsCurrentPlayerBot() &&
			gMousePosition.x >= boardX && gMousePosition.x < boardX + BOARD_SIZE &&
			gMousePosition.y >= boardY && gMousePosition.y < boardY + BOARD_SIZE)
		{
			hoveredColumn = (gMousePosition.x - boardX) / CELL_SIZE;
			hoveredRow = (gMousePosition.y - boardY) / CELL_SIZE;
		}

		for (int row = 0; row < 3; ++row)
		{
			for (int column = 0; column < 3; ++column)
			{
				int cellX = boardX + column * CELL_SIZE;
				int cellY = boardY + row * CELL_SIZE;

				RECT cell{
					cellX + 6,
					cellY + 6,
					cellX + CELL_SIZE - 6,
					cellY + CELL_SIZE - 6
				};

				bool hovered = row == hoveredRow && column == hoveredColumn && board[row][column] == 0;
				COLORREF fill = hovered ? UI::SurfaceHover : UI::Surface;
				COLORREF border = hovered ? UI::MixColor(UI::Border, UI::Text, 0.16f) : UI::Border;

				UI::DrawRoundedPanel(hdc, cell, 20, fill, border, 1);

				if (board[row][column] == 0)
					continue;

				float scale = 1.0f;
				if (row == gAnimatedRow && column == gAnimatedColumn && gMoveAnimation < 1.0f)
				{
					scale = max(0.05f, EaseOutBack(gMoveAnimation));
				}

				const int centerX = cellX + CELL_SIZE / 2;
				const int centerY = cellY + CELL_SIZE / 2;
				const int halfSize = static_cast<int>(45.0f * scale);
				const COLORREF accent = board[row][column] == 1 ? UI::Red : UI::Blue;

				HPEN pen = CreatePen(PS_SOLID, 11, accent);
				HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
				HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(NULL_BRUSH)));

				if (board[row][column] == 1)
				{
					MoveToEx(hdc, centerX - halfSize, centerY - halfSize, nullptr);
					LineTo(hdc, centerX + halfSize, centerY + halfSize);
					MoveToEx(hdc, centerX + halfSize, centerY - halfSize, nullptr);
					LineTo(hdc, centerX - halfSize, centerY + halfSize);
				}
				else
				{
					Ellipse(hdc, centerX - halfSize, centerY - halfSize, centerX + halfSize, centerY + halfSize);
				}

				SelectObject(hdc, oldBrush);
				SelectObject(hdc, oldPen);
				DeleteObject(pen);
			}
		}

		int r1, c1, r2, c2;
		if (GetWinningCells(r1, c1, r2, c2))
		{
			const COLORREF winColor = winner == 1 ? UI::Red : UI::Blue;
			const int x1 = boardX + c1 * CELL_SIZE + CELL_SIZE / 2;
			const int y1 = boardY + r1 * CELL_SIZE + CELL_SIZE / 2;
			const int x2 = boardX + c2 * CELL_SIZE + CELL_SIZE / 2;
			const int y2 = boardY + r2 * CELL_SIZE + CELL_SIZE / 2;

			float t = std::clamp(gWinAnimation, 0.0f, 1.0f);
			int currentX = x1 + static_cast<int>((x2 - x1) * t);
			int currentY = y1 + static_cast<int>((y2 - y1) * t);

			HPEN winPen = CreatePen(PS_SOLID, 7, UI::MixColor(winColor, RGB(255, 255, 255), 0.18f));
			HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, winPen));
			MoveToEx(hdc, x1, y1, nullptr);
			LineTo(hdc, currentX, currentY);
			SelectObject(hdc, oldPen);
			DeleteObject(winPen);
		}

		if (winner != 0)
		{
			EndGameLayout endLayout = GetEndGameLayout(windowWidth, boardY);
			bool restartHover = PtInRect(&endLayout.restartButton, gMousePosition) != FALSE;
			bool menuHover = PtInRect(&endLayout.changePlayersButton, gMousePosition) != FALSE;

			DrawMenuButton(hdc, endLayout.restartButton, L"RECOMMENCER", true, UI::Success, restartHover);
			DrawMenuButton(hdc, endLayout.changePlayersButton, L"CHANGER LES JOUEURS", true, RGB(72, 84, 112), menuHover);
		}
	}

	bool TickVisualAnimations()
	{
		bool redraw = false;

		if (currentScreen == Screen::Menu)
		{
			redraw |= TickMenuAnimation();
		}

		if (gMoveAnimation < 1.0f)
		{
			gMoveAnimation = min(1.0f, gMoveAnimation + 0.075f);
			redraw = true;
		}

		if ((winner == 1 || winner == 2) && gWinAnimation < 1.0f)
		{
			gWinAnimation = min(1.0f, gWinAnimation + 0.055f);
			redraw = true;
		}

		return redraw;
	}
}

void StartBotTurn(HWND hWnd)
{
	if (currentScreen == Screen::Game && winner == 0 && IsCurrentPlayerBot())
	{
		SetTimer(hWnd, TIMER_BOT, 420, nullptr);
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName = nullptr; // Retire l'ancien menu Win32 pour un look plus propre
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	constexpr int clientWidth = 860;
	constexpr int clientHeight = 700;

	DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	RECT windowRect{ 0, 0, clientWidth, clientHeight };
	AdjustWindowRect(&windowRect, windowStyle, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	RECT workArea{};
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);

	int windowX = workArea.left + ((workArea.right - workArea.left) - windowWidth) / 2;
	int windowY = workArea.top + ((workArea.bottom - workArea.top) - windowHeight) / 2;

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
		return FALSE;

	BOOL darkMode = TRUE;
	DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

	ResetMenuAnimation();
	SetTimer(hWnd, TIMER_UI, 16, nullptr); // ~60 FPS pour les animations

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
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
		break;
	}

	case WM_TIMER:
	{
		if (wParam == TIMER_UI)
		{
			if (TickVisualAnimations())
				InvalidateRect(hWnd, nullptr, FALSE);
			return 0;
		}

		if (wParam == TIMER_BOT)
		{
			KillTimer(hWnd, TIMER_BOT);

			if (currentScreen == Screen::Game && winner == 0 && IsCurrentPlayerBot())
			{
				int before[3][3]{};
				for (int r = 0; r < 3; ++r)
					for (int c = 0; c < 3; ++c)
						before[r][c] = board[r][c];

				PlayBotMove();

				int row = -1;
				int column = -1;
				if (FindNewMove(before, row, column))
					BeginMoveAnimation(row, column);

				if (winner == 1 || winner == 2)
					gWinAnimation = 0.0f;

				InvalidateRect(hWnd, nullptr, FALSE);

				if (winner == 0 && IsCurrentPlayerBot())
					StartBotTurn(hWnd);
			}

			return 0;
		}

		break;
	}

	case WM_MOUSEMOVE:
	{
		gMousePosition = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if (!gMouseTracking)
		{
			TRACKMOUSEEVENT track{};
			track.cbSize = sizeof(track);
			track.dwFlags = TME_LEAVE;
			track.hwndTrack = hWnd;
			TrackMouseEvent(&track);
			gMouseTracking = true;
		}

		if (currentScreen == Screen::Menu)
		{
			RECT client{};
			GetClientRect(hWnd, &client);
			UpdateMenuHover(gMousePosition, client.right - client.left);
		}

		InvalidateRect(hWnd, nullptr, FALSE);
		return 0;
	}

	case WM_MOUSELEAVE:
		gMouseTracking = false;
		gMousePosition = { -1000, -1000 };
		ClearMenuHover();
		InvalidateRect(hWnd, nullptr, FALSE);
		return 0;

	case WM_ERASEBKGND:
		// Le rendu est double-bufferisé dans WM_PAINT.
		return 1;

	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		HDC windowDC = BeginPaint(hWnd, &ps);

		RECT clientRect{};
		GetClientRect(hWnd, &clientRect);
		int windowWidth = clientRect.right - clientRect.left;
		int windowHeight = clientRect.bottom - clientRect.top;

		HDC bufferDC = CreateCompatibleDC(windowDC);
		HBITMAP bufferBitmap = CreateCompatibleBitmap(windowDC, windowWidth, windowHeight);
		HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(bufferDC, bufferBitmap));

		if (currentScreen == Screen::Menu)
			DrawMenu(bufferDC, windowWidth, windowHeight);
		else
			DrawGame(bufferDC, windowWidth, windowHeight);

		BitBlt(windowDC, 0, 0, windowWidth, windowHeight, bufferDC, 0, 0, SRCCOPY);

		SelectObject(bufferDC, oldBitmap);
		DeleteObject(bufferBitmap);
		DeleteDC(bufferDC);

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		POINT mousePoint{ mouseX, mouseY };

		RECT clientRect{};
		GetClientRect(hWnd, &clientRect);
		int windowWidth = clientRect.right - clientRect.left;
		int windowHeight = clientRect.bottom - clientRect.top;

		if (currentScreen == Screen::Menu)
		{
			MenuLayout layout = GetMenuLayout(windowWidth);

			if (PtInRect(&layout.j1Human, mousePoint))
				player1Type = PlayerType::Human;
			else if (PtInRect(&layout.j1Bot, mousePoint))
				player1Type = PlayerType::Bot;
			else if (player1Type == PlayerType::Bot && PtInRect(&layout.j1Easy, mousePoint))
				player1Difficulty = BotDifficulty::Easy;
			else if (player1Type == PlayerType::Bot && PtInRect(&layout.j1Medium, mousePoint))
				player1Difficulty = BotDifficulty::Medium;
			else if (player1Type == PlayerType::Bot && PtInRect(&layout.j1Hard, mousePoint))
				player1Difficulty = BotDifficulty::Hard;
			else if (PtInRect(&layout.j2Human, mousePoint))
				player2Type = PlayerType::Human;
			else if (PtInRect(&layout.j2Bot, mousePoint))
				player2Type = PlayerType::Bot;
			else if (player2Type == PlayerType::Bot && PtInRect(&layout.j2Easy, mousePoint))
				player2Difficulty = BotDifficulty::Easy;
			else if (player2Type == PlayerType::Bot && PtInRect(&layout.j2Medium, mousePoint))
				player2Difficulty = BotDifficulty::Medium;
			else if (player2Type == PlayerType::Bot && PtInRect(&layout.j2Hard, mousePoint))
				player2Difficulty = BotDifficulty::Hard;
			else if (PtInRect(&layout.startButton, mousePoint))
			{
				currentScreen = Screen::Game;
				ResetGame();
				ResetGameAnimations();
				InvalidateRect(hWnd, nullptr, FALSE);
				StartBotTurn(hWnd);
				return 0;
			}

			UpdateMenuHover(mousePoint, windowWidth);
			InvalidateRect(hWnd, nullptr, FALSE);
			return 0;
		}

		int boardX = (windowWidth - BOARD_SIZE) / 2;
		int boardY = (windowHeight - BOARD_SIZE) / 2;

		if (winner != 0)
		{
			EndGameLayout endLayout = GetEndGameLayout(windowWidth, boardY);

			if (PtInRect(&endLayout.restartButton, mousePoint))
			{
				ResetGame();
				ResetGameAnimations();
				InvalidateRect(hWnd, nullptr, FALSE);
				StartBotTurn(hWnd);
				return 0;
			}

			if (PtInRect(&endLayout.changePlayersButton, mousePoint))
			{
				KillTimer(hWnd, TIMER_BOT);
				ResetGame();
				ResetGameAnimations();
				currentScreen = Screen::Menu;
				ResetMenuAnimation();
				InvalidateRect(hWnd, nullptr, FALSE);
				return 0;
			}
		}

		if (winner == 0 && !IsCurrentPlayerBot() &&
			mouseX >= boardX && mouseX < boardX + BOARD_SIZE &&
			mouseY >= boardY && mouseY < boardY + BOARD_SIZE)
		{
			int column = (mouseX - boardX) / CELL_SIZE;
			int row = (mouseY - boardY) / CELL_SIZE;

			if (board[row][column] == 0)
			{
				board[row][column] = currentPlayer;
				BeginMoveAnimation(row, column);

				winner = CheckWinner();

				if (winner == 0)
					currentPlayer = (currentPlayer == 1) ? 2 : 1;
				else if (winner == 1 || winner == 2)
					gWinAnimation = 0.0f;

				InvalidateRect(hWnd, nullptr, FALSE);

				if (winner == 0 && IsCurrentPlayerBot())
					StartBotTurn(hWnd);
			}
		}

		return 0;
	}

	case WM_DESTROY:
		KillTimer(hWnd, TIMER_BOT);
		KillTimer(hWnd, TIMER_UI);
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		return static_cast<INT_PTR>(TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return static_cast<INT_PTR>(TRUE);
		}
		break;
	}

	return static_cast<INT_PTR>(FALSE);
}