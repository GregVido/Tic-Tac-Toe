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

enum class MenuHoverTarget
{
	None,
	J1Human,
	J1Bot,
	J1Easy,
	J1Medium,
	J1Hard,
	J2Human,
	J2Bot,
	J2Easy,
	J2Medium,
	J2Hard,
	Start
};

MenuLayout GetMenuLayout(int windowWidth);
EndGameLayout GetEndGameLayout(int windowWidth, int boardY);

void DrawMenuButton(
	HDC hdc,
	const RECT& rect,
	const wchar_t* text,
	bool selected,
	COLORREF selectedColor,
	bool hovered = false
);

void DrawMenu(HDC hdc, int windowWidth, int windowHeight);

bool UpdateMenuHover(POINT mousePoint, int windowWidth);
void ClearMenuHover();
void ResetMenuAnimation();
bool TickMenuAnimation();