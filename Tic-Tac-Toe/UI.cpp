#include "UI.h"

#include <algorithm>

#pragma comment(lib, "Msimg32.lib")

namespace UI
{
	HFONT CreateUiFont(int height, int weight, const wchar_t* face)
	{
		return CreateFontW(
			-height,
			0,
			0,
			0,
			weight,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			face
		);
	}

	COLORREF MixColor(COLORREF a, COLORREF b, float amount)
	{
		amount = std::clamp(amount, 0.0f, 1.0f);

		auto mix = [amount](BYTE x, BYTE y)
			{
				return static_cast<BYTE>(x + (y - x) * amount);
			};

		return RGB(
			mix(GetRValue(a), GetRValue(b)),
			mix(GetGValue(a), GetGValue(b)),
			mix(GetBValue(a), GetBValue(b))
		);
	}

	void FillGradient(HDC hdc, const RECT& rect, COLORREF top, COLORREF bottom)
	{
		TRIVERTEX vertices[2]{};

		vertices[0].x = rect.left;
		vertices[0].y = rect.top;
		vertices[0].Red = static_cast<COLOR16>(GetRValue(top) << 8);
		vertices[0].Green = static_cast<COLOR16>(GetGValue(top) << 8);
		vertices[0].Blue = static_cast<COLOR16>(GetBValue(top) << 8);
		vertices[0].Alpha = 0xFF00;

		vertices[1].x = rect.right;
		vertices[1].y = rect.bottom;
		vertices[1].Red = static_cast<COLOR16>(GetRValue(bottom) << 8);
		vertices[1].Green = static_cast<COLOR16>(GetGValue(bottom) << 8);
		vertices[1].Blue = static_cast<COLOR16>(GetBValue(bottom) << 8);
		vertices[1].Alpha = 0xFF00;

		GRADIENT_RECT gradientRect{ 0, 1 };
		GradientFill(hdc, vertices, 2, &gradientRect, 1, GRADIENT_FILL_RECT_V);
	}

	void DrawRoundedPanel(HDC hdc, const RECT& rect, int radius, COLORREF fill, COLORREF border, int borderWidth)
	{
		HBRUSH brush = CreateSolidBrush(fill);
		HPEN pen = CreatePen(PS_SOLID, borderWidth, border);

		HGDIOBJ oldBrush = SelectObject(hdc, brush);
		HGDIOBJ oldPen = SelectObject(hdc, pen);

		RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);

		SelectObject(hdc, oldPen);
		SelectObject(hdc, oldBrush);

		DeleteObject(pen);
		DeleteObject(brush);
	}

	void DrawShadow(HDC hdc, const RECT& rect, int radius, int offset)
	{
		RECT shadowRect = rect;
		OffsetRect(&shadowRect, 0, offset);
		DrawRoundedPanel(hdc, shadowRect, radius, Shadow, Shadow, 1);
	}

	void DrawCenteredText(HDC hdc, RECT rect, const wchar_t* text, HFONT font, COLORREF color, UINT flags)
	{
		HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, color);
		DrawTextW(hdc, text, -1, &rect, flags);
		SelectObject(hdc, oldFont);
	}

	void DrawButton(HDC hdc, const RECT& rect, const wchar_t* text, bool selected, COLORREF accent, bool hovered, int radius)
	{
		RECT drawRect = rect;

		if (hovered)
		{
			OffsetRect(&drawRect, 0, -2);
		}

		DrawShadow(hdc, drawRect, radius, hovered ? 6 : 4);

		COLORREF fill;
		COLORREF borderColor;
		COLORREF textColor;

		if (selected)
		{
			fill = hovered ? MixColor(accent, RGB(255, 255, 255), 0.10f) : accent;
			borderColor = MixColor(accent, RGB(255, 255, 255), 0.22f);
			textColor = RGB(255, 255, 255);
		}
		else
		{
			fill = hovered ? SurfaceHover : Surface;
			borderColor = hovered ? MixColor(Border, RGB(255, 255, 255), 0.18f) : Border;
			textColor = hovered ? Text : Muted;
		}

		DrawRoundedPanel(hdc, drawRect, radius, fill, borderColor, 1);

		HFONT font = CreateUiFont(15, FW_SEMIBOLD);
		DrawCenteredText(hdc, drawRect, text, font, textColor);
		DeleteObject(font);
	}
}
