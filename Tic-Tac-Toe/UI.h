#pragma once

#include <Windows.h>

namespace UI
{
	inline constexpr COLORREF BackgroundTop = RGB(10, 14, 24);
	inline constexpr COLORREF BackgroundBottom = RGB(18, 24, 40);
	inline constexpr COLORREF Surface = RGB(24, 31, 48);
	inline constexpr COLORREF SurfaceHover = RGB(31, 40, 61);
	inline constexpr COLORREF SurfaceStrong = RGB(36, 46, 70);
	inline constexpr COLORREF Border = RGB(51, 63, 88);
	inline constexpr COLORREF Text = RGB(242, 246, 255);
	inline constexpr COLORREF Muted = RGB(145, 158, 182);
	inline constexpr COLORREF Red = RGB(255, 82, 112);
	inline constexpr COLORREF Blue = RGB(67, 169, 255);
	inline constexpr COLORREF Purple = RGB(125, 92, 255);
	inline constexpr COLORREF Success = RGB(55, 211, 153);
	inline constexpr COLORREF Shadow = RGB(5, 8, 15);

	HFONT CreateUiFont(int height, int weight = FW_NORMAL, const wchar_t* face = L"Segoe UI Variable Text");
	COLORREF MixColor(COLORREF a, COLORREF b, float amount);

	void FillGradient(HDC hdc, const RECT& rect, COLORREF top, COLORREF bottom);
	void DrawRoundedPanel(HDC hdc, const RECT& rect, int radius, COLORREF fill, COLORREF border = Border, int borderWidth = 1);
	void DrawShadow(HDC hdc, const RECT& rect, int radius, int offset = 5);
	void DrawButton(HDC hdc, const RECT& rect, const wchar_t* text, bool selected, COLORREF accent, bool hovered = false, int radius = 14);
	void DrawCenteredText(HDC hdc, RECT rect, const wchar_t* text, HFONT font, COLORREF color, UINT flags = DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
