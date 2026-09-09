#pragma once

#include <Windows.h>

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

MenuLayout GetMenuLayout(int windowWidth);

EndGameLayout GetEndGameLayout(
	int windowWidth,
	int boardY
);

void DrawMenuButton(
	HDC hdc,
	const RECT& rect,
	const wchar_t* text,
	bool selected,
	COLORREF selectedColor
);

void DrawMenu(
	HDC hdc,
	int windowWidth
);