#include "Renderer.h"
#include "CD3DFont.h"
#include "res.h"

namespace renderer
{

	CRenderer* pRenderer = new CRenderer();
	static bool bCreate = false;

	VOID CRenderer::Create(LPDIRECT3DDEVICE9 pGameDevice)
	{
		if (!bCreate)
		{
			bCreate = true;
			this->pDevice = pGameDevice;

			pDrawer = new CDraw();
			pDrawer->GetDevice(pGameDevice);
			pDrawer->AddFont("Segoe UI", 13, false, false);

			pText = new CD3DFont("Segoe UI", 8, 0);

			pText->InitDeviceObjects(this->pDevice);
			pText->RestoreDeviceObjects();

			D3DXCreateSprite(pDevice, &pSprite);
			D3DXCreateLine(pDevice, &pLine);
		}

		if (this->pDevice != pGameDevice)
		{
			this->pDevice = pGameDevice;

			SAFE_RELEASE(pLine)
				SAFE_RELEASE(pSprite);

			D3DXCreateSprite(pDevice, &pSprite);
			D3DXCreateLine(pDevice, &pLine);

			if (pText != nullptr)
			{
				pText->InvalidateDeviceObjects();
				pText->DeleteDeviceObjects();
			}

			pText = new CD3DFont("Segoe UI", 8, 0);

			pText->InitDeviceObjects(this->pDevice);
			pText->RestoreDeviceObjects();

			pDrawer->FontReset();
		}
	}

	VOID CRenderer::Begin()
	{
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);

		pDevice->GetFVF(&dwOldFVF);
		pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED);
		pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	VOID CRenderer::End()
	{
		pDevice->SetFVF(dwOldFVF);

		pSprite->End();
	}

	VOID CRenderer::DrawString(wchar_t const* text, float x, float y, int orientation, bool bordered, DWORD color, DWORD bcolor)
	{
		pDrawer->DrawString(text, x, y, orientation, bordered, color, bcolor);
	}

	VOID CRenderer::DrawString(char const* text, float x, float y, int orientation, bool bordered, DWORD color, DWORD bcolor)
	{
		pDrawer->DrawString(text, x, y, orientation, bordered, color, bcolor);
	}

	VOID CRenderer::DrawRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor)
	{
		if (bNeedsFlush)
		{
			pSprite->Flush();
			bNeedsFlush = FALSE;
		}

		CVertexList VertexList[4] =
		{
			{ X, Y + Height, 0.0f, dColor },
		{ X, Y, 0.0f, dColor },
		{ X + Width, Y + Height, 0.0f, dColor },
		{ X + Width , Y, 0.0f, dColor },
		};

		pDevice->SetTexture(0, nullptr);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(CVertexList));
	}

	VOID CRenderer::DrawBorderLine(FLOAT X, FLOAT Y, FLOAT X2, FLOAT Y2, DWORD dColor, FLOAT distance)
	{
		D3DXVECTOR2 vLine[2];
		pLine->SetAntialias(1);


		//float minWidth = 2.7f, maxWidth = 5.7f;
		float minWidth = 3.f, maxWidth = 3.f;
		float maxDist = 1700, minDist = 100;

		distance = min(maxDist, max(distance, minDist));
		float distanceScalar = (maxDist - distance) / (maxDist - minDist);

		float width = (maxWidth - minWidth) * distanceScalar + minWidth;

		pLine->SetWidth(width);

		pLine->Begin();

		vLine[0][0] = X;
		vLine[0][1] = Y;
		vLine[1][0] = X2;
		vLine[1][1] = Y2;

		pLine->Draw(vLine, 2, dColor);
		pLine->End();
	}

	VOID CRenderer::DrawLine(FLOAT X, FLOAT Y, FLOAT X2, FLOAT Y2, DWORD dColor, FLOAT thickness)
	{
		D3DXVECTOR2 vLine[2];
		pLine->SetAntialias(1);

		pLine->SetWidth(thickness);

		pLine->Begin();

		vLine[0][0] = X;
		vLine[0][1] = Y;
		vLine[1][0] = X2;
		vLine[1][1] = Y2;

		pLine->Draw(vLine, 2, dColor);
		pLine->End();
	}

	VOID CRenderer::DrawCircle(FLOAT X, FLOAT Y, FLOAT Radius, FLOAT Sides, DWORD dColor1)
	{
		D3DXVECTOR2 Line[128];
		FLOAT Step = valkyrie::pi * 2.0 / Sides;
		INT Count = 0;
		for (FLOAT a = 0; a < valkyrie::pi * 2.0; a += Step)
		{
			FLOAT X1 = Radius * cos(a) + X;
			FLOAT Y1 = Radius * sin(a) + Y;
			FLOAT X2 = Radius * cos(a + Step) + X;
			FLOAT Y2 = Radius * sin(a + Step) + Y;
			Line[Count].x = X1;
			Line[Count].y = Y1;
			Line[Count + 1].x = X2;
			Line[Count + 1].y = Y2;
			Count += 2;
		}
		pLine->Begin();
		pLine->Draw(Line, Count, dColor1);
		pLine->End();
	}

	VOID CRenderer::DrawGradientRect(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor1, D3DCOLOR dColor2)
	{
		if (bNeedsFlush)
		{
			pSprite->Flush();
			bNeedsFlush = FALSE;
		}

		CVertexList VertexList[4] =
		{
			{ X, Y + Height, 0.0f, dColor2 },
		{ X, Y, 0.0f, dColor1 },
		{ X + Width, Y + Height, 0.0f, dColor2 },
		{ X + Width, Y, 0.0f, dColor1 },
		};

		pDevice->SetTexture(0, nullptr);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(CVertexList));
	}

	VOID CRenderer::DrawGradientRect2(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor1, D3DCOLOR dColor2)
	{
		if (bNeedsFlush)
		{
			pSprite->Flush();
			bNeedsFlush = FALSE;
		}

		CVertexList VertexList[4] =
		{
			{ X, Y + Height, 0.0f, dColor1 },
		{ X, Y, 0.0f, dColor1 },
		{ X + Width, Y + Height, 0.0f, dColor2 },
		{ X + Width, Y, 0.0f, dColor2 },
		};

		pDevice->SetTexture(0, nullptr);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(CVertexList));
	}

	VOID CRenderer::DrawBox(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT fSize, D3DCOLOR dColor)
	{
		DrawRect(X, Y, Width, fSize, dColor);
		DrawRect(X, Y, 1.0f, Height, dColor);
		DrawRect(X, Y + Height, Width + fSize, fSize, dColor);
		DrawRect(X + Width, Y, fSize, Height + fSize, dColor);
	}

	VOID CRenderer::DrawDot(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor)
	{
		DrawRect(X - 1.0f, Y - 1.0f, Width + 2.0f, Height + 2.0f, dColor);
		DrawRect(X, Y, Width, Height, dColor);
	}

	VOID CRenderer::DrawCross(FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, D3DCOLOR dColor)
	{
		//DrawRect(X - Width, Y - 0.5f, (Width * 2.0f), 1.0f, dColor);
		//DrawRect(X - 0.5f, Y - Height, 1.0f, (Height * 2.0f), dColor);
		DrawLine(X - Width - 1, Y - Height - 1, X - 1, Y - 1, Black, 5);
		DrawLine(X + Width + 1, Y - Height - 1, X + 1, Y - 1, Black, 5);
		DrawLine(X + Width + 1, Y + Height + 1, X + 1, Y + 1, Black, 5);
		DrawLine(X - Width - 1, Y + Height + 1, X - 1, Y + 1, Black, 5);
		DrawLine(X - Width, Y - Height, X - 2, Y - 2, dColor, 1);
		DrawLine(X + Width, Y - Height, X + 2, Y - 2, dColor, 1);
		DrawLine(X + Width, Y + Height, X + 2, Y + 2, dColor, 1);
		DrawLine(X - Width, Y + Height, X - 2, Y + 2, dColor, 1);
	}

	VOID CRenderer::DrawCrosshair(DWORD dColor1)
	{
		D3DVIEWPORT9 pViewPort;
		pDevice->GetViewport(&pViewPort);

		FLOAT X = GetSystemMetrics(SM_CXSCREEN) / 2;
		FLOAT Y = GetSystemMetrics(SM_CYSCREEN) / 2;

		DrawCircle(X - 0.5, Y - 0.5, 7.0f, 50.0f, dColor1);
	}

	VOID CRenderer::DrawCursor(FLOAT X, FLOAT Y)
	{
		DrawRect(X, Y, 5.0f, 5.0f, White);
		DrawRect(X + 5.0f, Y + 5.0f, 2.0f, 2.0f, White);
		DrawRect(X + 7.0f, Y + 7.0f, 2.0f, 2.0f, White);
	}

	VOID CRenderer::DrawBar(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2, D3DCOLOR dColor3)
	{
		DrawGradientRect(BaseX, BaseY, BaseW, BaseH, dColor2, dColor3);

		DrawRect(BaseX, BaseY, BaseW, 1.0f, dColor1);
		DrawRect(BaseX, BaseY, 1.0f, BaseH, dColor1);
		DrawRect(BaseX, BaseY + BaseH, BaseW, 1.0f, dColor1);
		DrawRect(BaseX + BaseW, BaseY, 1.0f, BaseH, dColor1);
	}

	VOID CRenderer::DrawBase(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2)
	{
		DrawRect(BaseX, BaseY, BaseW, BaseH, dColor2);

		DrawRect(BaseX, BaseY, BaseW, 1.0f, dColor1);
		DrawRect(BaseX, BaseY, 1.0f, BaseH, dColor1);
		DrawRect(BaseX, BaseY + BaseH, BaseW, 1.0f, dColor1);
		DrawRect(BaseX + BaseW, BaseY, 1.0f, BaseH, dColor1);
	}

	VOID CRenderer::DrawTab(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2)
	{
		DrawRect(BaseX, BaseY, BaseW, BaseH, dColor2);

		DrawRect(BaseX, BaseY, BaseW, 1.0f, dColor1);
		DrawRect(BaseX, BaseY, 1.0f, BaseH, dColor1);
		DrawRect(BaseX, BaseY + BaseH, BaseW, 1.0f, dColor1);
		DrawRect(BaseX + BaseW, BaseY, 1.0f, BaseH, dColor1);
	}

	VOID CRenderer::DrawTabOpen(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2)
	{
		DrawRect(BaseX, BaseY, BaseW, BaseH, dColor2);

		DrawRect(BaseX, BaseY, BaseW, 1.0f, dColor1);
		DrawRect(BaseX, BaseY, 1.0f, BaseH, dColor1);
		DrawRect(BaseX, BaseY + BaseH, BaseW, 1.0f, dColor1);
	}

	VOID CRenderer::DrawButton(FLOAT BaseX, FLOAT BaseY, FLOAT BaseW, FLOAT BaseH, D3DCOLOR dColor1, D3DCOLOR dColor2, D3DCOLOR dColor3)
	{
		DrawGradientRect(BaseX, BaseY, BaseW, BaseH, dColor2, dColor3);

		DrawRect(BaseX, BaseY, BaseW, 1.0f, dColor1);
		DrawRect(BaseX, BaseY, 1.0f, BaseH, dColor1);
		DrawRect(BaseX, BaseY + BaseH, BaseW, 1.0f, dColor1);
		DrawRect(BaseX + BaseW, BaseY, 1.0f, BaseH, dColor1);
	}

	hsv CRenderer::rgb2hsv(rgb in)
	{
		hsv         out;
		double      min, max, delta;

		min = in.r < in.g ? in.r : in.g;
		min = min < in.b ? min : in.b;

		max = in.r > in.g ? in.r : in.g;
		max = max > in.b ? max : in.b;

		out.v = max;                                // v
		delta = max - min;
		if (delta < 0.00001)
		{
			out.s = 0;
			out.h = 0; // undefined, maybe nan?
			return out;
		}
		if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
			out.s = (delta / max);                  // s
		}
		else {
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			out.s = 0.0;
			out.h = NAN;                            // its now undefined
			return out;
		}
		if (in.r >= max)                           // > is bogus, just keeps compilor happy
			out.h = (in.g - in.b) / delta;        // between yellow & magenta
		else
			if (in.g >= max)
				out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
			else
				out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

		out.h *= 60.0;                              // degrees

		if (out.h < 0.0)
			out.h += 360.0;

		return out;
	}


	rgb CRenderer::hsv2rgb(hsv in)
	{
		double      hh, p, q, t, ff;
		long        i;
		rgb         out;

		if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
			out.r = in.v;
			out.g = in.v;
			out.b = in.v;
			return out;
		}
		hh = in.h;
		if (hh >= 360.0) hh = 0.0;
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = in.v * (1.0 - in.s);
		q = in.v * (1.0 - (in.s * ff));
		t = in.v * (1.0 - (in.s * (1.0 - ff)));

		switch (i) {
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
		}
		return out;
	}
}