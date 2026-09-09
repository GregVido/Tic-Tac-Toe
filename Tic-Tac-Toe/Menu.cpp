#include "Menu.h"
#include "Game.h"

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

EndGameLayout GetEndGameLayout(int windowWidth, int boardY)
{
	EndGameLayout layout{};

	int center = windowWidth / 2;

	int buttonWidth = 220;
	int buttonHeight = 45;

	int y = boardY + BOARD_SIZE + 20;

	layout.restartButton = {
		center - buttonWidth - 10,
		y,
		center - 10,
		y + buttonHeight
	};

	layout.changePlayersButton = {
		center + 10,
		y,
		center + buttonWidth + 10,
		y + buttonHeight
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