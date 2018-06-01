#pragma once

#include "Draw.h"

namespace renderer
{

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define Red D3DCOLOR_ARGB(255, 192, 56, 60)
#define UltraRed D3DCOLOR_ARGB(255, 255, 0, 0)
#define Yellow D3DCOLOR_ARGB(255, 255, 255, 0)
#define CoolYellow D3DCOLOR_ARGB(255, 255, 252, 76)
#define Green D3DCOLOR_ARGB(255, 0, 255, 0)
#define CoolGreen D3DCOLOR_ARGB(255, 75, 234, 109)
#define Blue D3DCOLOR_ARGB(255, 0, 120, 255)
#define Cyan D3DCOLOR_ARGB(255, 0, 255, 255)
#define Purple D3DCOLOR_ARGB(255, 102, 0, 153)
#define Pink D3DCOLOR_ARGB(255, 255, 20, 147)
#define Orange D3DCOLOR_ARGB(255, 255, 165, 0)
#define Black D3DCOLOR_ARGB(255, 0, 0, 0)
#define White D3DCOLOR_ARGB(255, 255, 255, 255)

#define Gray D3DCOLOR_ARGB(255, 50, 50, 50)
#define DGray D3DCOLOR_ARGB(255, 40, 40, 40)
#define DDGray D3DCOLOR_ARGB(255, 30, 30, 30)
#define ItemTextInactive D3DCOLOR_ARGB(255, 200, 200, 200)
#define ItemTextActive D3DCOLOR_ARGB(255, 0, 120, 255)
#define DarkGreen D3DCOLOR_ARGB(255, 34, 139, 34)
#define TBlack D3DCOLOR_ARGB(255, 0, 0, 0)

#define LightGreenF D3DCOLOR_ARGB(255, 0, 255, 186)
#define LightGreenA D3DCOLOR_ARGB(80, 161, 255, 219)
#define LightGreenB D3DCOLOR_ARGB(80, 0, 255, 186)
#define DarkBlue D3DCOLOR_ARGB(255, 0, 60, 255)
#define LightBlue D3DCOLOR_ARGB(255, 0, 150, 255)

#define TDarkBlue D3DCOLOR_ARGB(220, 0, 60, 255)
#define TLightBlue D3DCOLOR_ARGB(220, 0, 150, 255)

#define TDarkRed D3DCOLOR_ARGB(220, 190, 0, 0)
#define TLightRed D3DCOLOR_ARGB(220, 130, 0, 0)

#define SAFE_RELEASE(p)                                                                                                 \
	if (p)                                                                                                              \
	{                                                                                                                   \
		p->Release();                                                                                                   \
		p = NULL;                                                                                                       \
	}

	struct D3DTLVERTEX
	{
		float x, y, z, rhw;
		DWORD color;
	};

	struct hsv
	{
		float h;
		float s;
		float v;
	};

	struct rgb
	{
		float r; // a fraction between 0 and 1
		float g; // a fraction between 0 and 1
		float b; // a fraction between 0 and 1
	};

	class CVertexList
	{
	public:
		FLOAT X, Y, Z;
		DWORD dColor;
	};

	class CRenderer
	{
	public:
		VOID Create(LPDIRECT3DDEVICE9 pGameDevice);

		VOID Begin();
		VOID End();

		VOID DrawRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor);
		VOID DrawGradientRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor1, D3DCOLOR dColor2);
		VOID DrawGradientRect2(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor1, D3DCOLOR dColor2);
		VOID DrawLine(FLOAT X, FLOAT Y, FLOAT X2, FLOAT Y2, DWORD dColor, FLOAT thickness);
		VOID DrawBorderLine(FLOAT X, FLOAT Y, FLOAT X2, FLOAT Y2, DWORD dColor, FLOAT distance);
		VOID DrawCircle(FLOAT X, FLOAT Y, FLOAT Radius, FLOAT Sides, DWORD dColor1);
		VOID DrawBox(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT fSize, D3DCOLOR dColor);
		VOID DrawDot(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor);
		VOID DrawCross(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor);

		VOID DrawCrosshair(DWORD dColor1);
		VOID DrawCursor(FLOAT X, FLOAT Y);

		VOID DrawBar(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2, D3DCOLOR dColor3);
		VOID DrawBase(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2);
		VOID DrawTab(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2);
		VOID DrawTabOpen(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2);
		VOID
			DrawButton(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2, D3DCOLOR dColor3);
		VOID DrawString(wchar_t const* text, float x, float y, int orientation, bool bordered, DWORD color, DWORD bcolor = 0);
		VOID DrawString(char const* text, float x, float y, int orientation, bool bordered, DWORD color, DWORD bcolor = 0);

		hsv rgb2hsv(rgb in);
		rgb hsv2rgb(hsv in);

		LPD3DXSPRITE pSprite;
		LPD3DXLINE pLine;
		LPDIRECT3DDEVICE9 pDevice;
		LPDIRECT3DTEXTURE9 pTexture;
		CDraw* pDrawer;

		BOOL bNeedsFlush;

	private:
		DWORD dwOldFVF;
	};

	extern CRenderer* pRenderer;
}