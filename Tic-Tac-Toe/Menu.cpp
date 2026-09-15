#include "Menu.h"
#include "Game.h"
#include "UI.h"

#include <algorithm>

namespace
{
	MenuHoverTarget gHover = MenuHoverTarget::None;
	float gIntro = 0.0f;

	float Clamp01(float value)
	{
		return std::clamp(value, 0.0f, 1.0f);
	}

	float EaseOutCubic(float t)
	{
		t = Clamp01(t);
		float p = 1.0f - t;
		return 1.0f - p * p * p;
	}

	RECT OffsetAnimated(const RECT& rect, float delay)
	{
		float local = Clamp01((gIntro - delay) / (1.0f - delay));
		int offsetY = static_cast<int>((1.0f - EaseOutCubic(local)) * 28.0f);
		RECT result = rect;
		OffsetRect(&result, 0, offsetY);
		return result;
	}

	bool IsHovered(MenuHoverTarget target)
	{
		return gHover == target;
	}

	const wchar_t* DifficultyDescription(BotDifficulty difficulty)
	{
		switch (difficulty)
		{
		case BotDifficulty::Easy:
			return L"Coup aleatoire - rapide et imprevisible";
		case BotDifficulty::Medium:
			return L"MCTS - simule plusieurs parties avant de jouer";
		case BotDifficulty::Hard:
			return L"Minimax + alpha-beta - jeu optimal";
		default:
			return L"";
		}
	}

	void DrawPlayerCard(
		HDC hdc,
		RECT card,
		bool playerOne,
		const MenuLayout& layout,
		float delay)
	{
		card = OffsetAnimated(card, delay);

		const COLORREF accent = playerOne ? UI::Red : UI::Blue;
		const PlayerType type = playerOne ? player1Type : player2Type;
		const BotDifficulty difficulty = playerOne ? player1Difficulty : player2Difficulty;

		UI::DrawShadow(hdc, card, 24, 8);
		UI::DrawRoundedPanel(hdc, card, 24, UI::Surface, UI::Border, 1);

		RECT badge = { card.left + 24, card.top + 22, card.left + 78, card.top + 76 };
		UI::DrawRoundedPanel(hdc, badge, 18, UI::MixColor(accent, UI::Surface, 0.70f), accent, 1);

		HFONT symbolFont = UI::CreateUiFont(28, FW_BOLD, L"Segoe UI Variable Display");
		UI::DrawCenteredText(hdc, badge, playerOne ? L"X" : L"O", symbolFont, accent);
		DeleteObject(symbolFont);

		RECT titleRect = { card.left + 94, card.top + 18, card.right - 20, card.top + 50 };
		HFONT titleFont = UI::CreateUiFont(20, FW_BOLD, L"Segoe UI Variable Display");
		UI::DrawCenteredText(
			hdc,
			titleRect,
			playerOne ? L"Joueur 1" : L"Joueur 2",
			titleFont,
			UI::Text,
			DT_LEFT | DT_VCENTER | DT_SINGLELINE
		);
		DeleteObject(titleFont);

		RECT subtitleRect = { card.left + 94, card.top + 49, card.right - 20, card.top + 74 };
		HFONT subtitleFont = UI::CreateUiFont(13, FW_NORMAL);
		UI::DrawCenteredText(
			hdc,
			subtitleRect,
			playerOne ? L"Croix rouge" : L"Cercle bleu",
			subtitleFont,
			UI::Muted,
			DT_LEFT | DT_VCENTER | DT_SINGLELINE
		);
		DeleteObject(subtitleFont);

		RECT sectionLabel = { card.left + 24, card.top + 96, card.right - 24, card.top + 122 };
		HFONT labelFont = UI::CreateUiFont(12, FW_SEMIBOLD);
		UI::DrawCenteredText(hdc, sectionLabel, L"TYPE DE JOUEUR", labelFont, UI::Muted, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DeleteObject(labelFont);

		RECT humanRect = playerOne ? layout.j1Human : layout.j2Human;
		RECT botRect = playerOne ? layout.j1Bot : layout.j2Bot;
		humanRect = OffsetAnimated(humanRect, delay);
		botRect = OffsetAnimated(botRect, delay);

		DrawMenuButton(
			hdc,
			humanRect,
			L"HUMAIN",
			type == PlayerType::Human,
			accent,
			IsHovered(playerOne ? MenuHoverTarget::J1Human : MenuHoverTarget::J2Human)
		);

		DrawMenuButton(
			hdc,
			botRect,
			L"BOT",
			type == PlayerType::Bot,
			accent,
			IsHovered(playerOne ? MenuHoverTarget::J1Bot : MenuHoverTarget::J2Bot)
		);

		RECT difficultyLabel = { card.left + 24, card.top + 182, card.right - 24, card.top + 208 };
		HFONT difficultyFont = UI::CreateUiFont(12, FW_SEMIBOLD);
		UI::DrawCenteredText(
			hdc,
			difficultyLabel,
			type == PlayerType::Bot ? L"DIFFICULTE" : L"MODE",
			difficultyFont,
			UI::Muted,
			DT_LEFT | DT_VCENTER | DT_SINGLELINE
		);
		DeleteObject(difficultyFont);

		if (type == PlayerType::Bot)
		{
			RECT easy = playerOne ? layout.j1Easy : layout.j2Easy;
			RECT medium = playerOne ? layout.j1Medium : layout.j2Medium;
			RECT hard = playerOne ? layout.j1Hard : layout.j2Hard;

			easy = OffsetAnimated(easy, delay);
			medium = OffsetAnimated(medium, delay);
			hard = OffsetAnimated(hard, delay);

			DrawMenuButton(hdc, easy, L"FACILE", difficulty == BotDifficulty::Easy, accent,
				IsHovered(playerOne ? MenuHoverTarget::J1Easy : MenuHoverTarget::J2Easy));
			DrawMenuButton(hdc, medium, L"MOYEN", difficulty == BotDifficulty::Medium, accent,
				IsHovered(playerOne ? MenuHoverTarget::J1Medium : MenuHoverTarget::J2Medium));
			DrawMenuButton(hdc, hard, L"DIFFICILE", difficulty == BotDifficulty::Hard, accent,
				IsHovered(playerOne ? MenuHoverTarget::J1Hard : MenuHoverTarget::J2Hard));

			RECT infoRect = { card.left + 24, card.bottom - 48, card.right - 24, card.bottom - 18 };
			HFONT infoFont = UI::CreateUiFont(11, FW_NORMAL);
			UI::DrawCenteredText(hdc, infoRect, DifficultyDescription(difficulty), infoFont, UI::Muted,
				DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
			DeleteObject(infoFont);
		}
		else
		{
			RECT localRect = { card.left + 24, card.top + 216, card.right - 24, card.top + 266 };
			UI::DrawRoundedPanel(hdc, localRect, 14, UI::MixColor(UI::SurfaceStrong, accent, 0.08f), UI::Border, 1);

			HFONT infoFont = UI::CreateUiFont(13, FW_NORMAL);
			UI::DrawCenteredText(hdc, localRect, L"Controle local au clavier / a la souris", infoFont, UI::Muted);
			DeleteObject(infoFont);
		}
	}
}

MenuLayout GetMenuLayout(int windowWidth)
{
	MenuLayout layout{};

	const int center = windowWidth / 2;
	const int cardWidth = 330;
	const int gap = 24;
	const int leftX = center - gap / 2 - cardWidth;
	const int rightX = center + gap / 2;

	const int typeTop = 298;
	const int typeHeight = 48;
	const int typeGap = 10;
	const int typeWidth = 136;

	layout.j1Human = { leftX + 24, typeTop, leftX + 24 + typeWidth, typeTop + typeHeight };
	layout.j1Bot = { leftX + 24 + typeWidth + typeGap, typeTop, leftX + 24 + typeWidth * 2 + typeGap, typeTop + typeHeight };

	layout.j2Human = { rightX + 24, typeTop, rightX + 24 + typeWidth, typeTop + typeHeight };
	layout.j2Bot = { rightX + 24 + typeWidth + typeGap, typeTop, rightX + 24 + typeWidth * 2 + typeGap, typeTop + typeHeight };

	const int diffTop = 384;
	const int diffHeight = 44;
	const int diffGap = 8;
	const int diffWidth = 88;

	layout.j1Easy = { leftX + 24, diffTop, leftX + 24 + diffWidth, diffTop + diffHeight };
	layout.j1Medium = { leftX + 24 + diffWidth + diffGap, diffTop, leftX + 24 + diffWidth * 2 + diffGap, diffTop + diffHeight };
	layout.j1Hard = { leftX + 24 + (diffWidth + diffGap) * 2, diffTop, leftX + 24 + diffWidth * 3 + diffGap * 2, diffTop + diffHeight };

	layout.j2Easy = { rightX + 24, diffTop, rightX + 24 + diffWidth, diffTop + diffHeight };
	layout.j2Medium = { rightX + 24 + diffWidth + diffGap, diffTop, rightX + 24 + diffWidth * 2 + diffGap, diffTop + diffHeight };
	layout.j2Hard = { rightX + 24 + (diffWidth + diffGap) * 2, diffTop, rightX + 24 + diffWidth * 3 + diffGap * 2, diffTop + diffHeight };

	layout.startButton = { center - 165, 548, center + 165, 610 };

	return layout;
}

EndGameLayout GetEndGameLayout(int windowWidth, int boardY)
{
	EndGameLayout layout{};

	const int center = windowWidth / 2;
	const int buttonWidth = 210;
	const int buttonHeight = 50;
	const int gap = 14;
	const int y = boardY + BOARD_SIZE + 20;

	layout.restartButton = {
		center - gap / 2 - buttonWidth,
		y,
		center - gap / 2,
		y + buttonHeight
	};

	layout.changePlayersButton = {
		center + gap / 2,
		y,
		center + gap / 2 + buttonWidth,
		y + buttonHeight
	};

	return layout;
}

void DrawMenuButton(HDC hdc, const RECT& rect, const wchar_t* text, bool selected, COLORREF selectedColor, bool hovered)
{
	UI::DrawButton(hdc, rect, text, selected, selectedColor, hovered, 14);
}

void DrawMenu(HDC hdc, int windowWidth, int windowHeight)
{
	RECT client{ 0, 0, windowWidth, windowHeight };
	UI::FillGradient(hdc, client, UI::BackgroundTop, UI::BackgroundBottom);
	SetBkMode(hdc, TRANSPARENT);

	MenuLayout layout = GetMenuLayout(windowWidth);
	const int center = windowWidth / 2;

	float titleProgress = EaseOutCubic(Clamp01(gIntro / 0.65f));
	int titleOffset = static_cast<int>((1.0f - titleProgress) * 22.0f);

	RECT eyebrowRect{ 0, 30 + titleOffset, windowWidth, 58 + titleOffset };
	HFONT eyebrowFont = UI::CreateUiFont(12, FW_BOLD);
	UI::DrawCenteredText(hdc, eyebrowRect, L"CLASSIC  /  AI ARENA", eyebrowFont, UI::Purple);
	DeleteObject(eyebrowFont);

	RECT titleRect{ 0, 54 + titleOffset, windowWidth, 108 + titleOffset };
	HFONT titleFont = UI::CreateUiFont(42, FW_BOLD, L"Segoe UI Variable Display");
	UI::DrawCenteredText(hdc, titleRect, L"Tic Tac Toe", titleFont, UI::Text);
	DeleteObject(titleFont);

	RECT subtitleRect{ 0, 108 + titleOffset, windowWidth, 136 + titleOffset };
	HFONT subtitleFont = UI::CreateUiFont(14, FW_NORMAL);
	UI::DrawCenteredText(hdc, subtitleRect, L"Configure les deux joueurs, puis lance la partie.", subtitleFont, UI::Muted);
	DeleteObject(subtitleFont);

	RECT card1{ center - 342, 176, center - 12, 494 };
	RECT card2{ center + 12, 176, center + 342, 494 };

	DrawPlayerCard(hdc, card1, true, layout, 0.05f);
	DrawPlayerCard(hdc, card2, false, layout, 0.13f);

	RECT startRect = OffsetAnimated(layout.startButton, 0.20f);
	DrawMenuButton(hdc, startRect, L"LANCER LA PARTIE", true, UI::Purple, IsHovered(MenuHoverTarget::Start));

	RECT footer{ 0, windowHeight - 54, windowWidth, windowHeight - 20 };
	HFONT footerFont = UI::CreateUiFont(11, FW_NORMAL);
	UI::DrawCenteredText(hdc, footer, L"Facile : Random   •   Moyen : MCTS   •   Difficile : Minimax alpha-beta", footerFont, UI::Muted);
	DeleteObject(footerFont);
}

bool UpdateMenuHover(POINT mousePoint, int windowWidth)
{
	MenuLayout layout = GetMenuLayout(windowWidth);
	MenuHoverTarget newHover = MenuHoverTarget::None;

	if (PtInRect(&layout.j1Human, mousePoint)) newHover = MenuHoverTarget::J1Human;
	else if (PtInRect(&layout.j1Bot, mousePoint)) newHover = MenuHoverTarget::J1Bot;
	else if (player1Type == PlayerType::Bot && PtInRect(&layout.j1Easy, mousePoint)) newHover = MenuHoverTarget::J1Easy;
	else if (player1Type == PlayerType::Bot && PtInRect(&layout.j1Medium, mousePoint)) newHover = MenuHoverTarget::J1Medium;
	else if (player1Type == PlayerType::Bot && PtInRect(&layout.j1Hard, mousePoint)) newHover = MenuHoverTarget::J1Hard;
	else if (PtInRect(&layout.j2Human, mousePoint)) newHover = MenuHoverTarget::J2Human;
	else if (PtInRect(&layout.j2Bot, mousePoint)) newHover = MenuHoverTarget::J2Bot;
	else if (player2Type == PlayerType::Bot && PtInRect(&layout.j2Easy, mousePoint)) newHover = MenuHoverTarget::J2Easy;
	else if (player2Type == PlayerType::Bot && PtInRect(&layout.j2Medium, mousePoint)) newHover = MenuHoverTarget::J2Medium;
	else if (player2Type == PlayerType::Bot && PtInRect(&layout.j2Hard, mousePoint)) newHover = MenuHoverTarget::J2Hard;
	else if (PtInRect(&layout.startButton, mousePoint)) newHover = MenuHoverTarget::Start;

	if (newHover == gHover)
		return false;

	gHover = newHover;
	return true;
}

void ClearMenuHover()
{
	gHover = MenuHoverTarget::None;
}

void ResetMenuAnimation()
{
	gIntro = 0.0f;
}

bool TickMenuAnimation()
{
	if (gIntro >= 1.0f)
		return false;

	gIntro = std::min(1.0f, gIntro + 0.045f);
	return true;
}