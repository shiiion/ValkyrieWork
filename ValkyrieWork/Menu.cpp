#include "Menu.h"
#include "Renderer.h"
#include "CD3DFont.h"

#include <utility>

namespace menu
{
	cMenu Menu;

	using std::string;

	static string Default[] = { "Off", "On" };
	static string Single[] = { "" };
	static string legitBotSpeeds[] = { "Off", "Very Slow", "Slow", "Medium", "Fast" };
	static string legitBotRandoms[] = { "Off", "Small", "Medium", "High" };
	static string rageBotHitboxSettings[] = { "Head", "Torso" };
	static string AimBotFOVSettings[] = { "0", "0.5", "0.7", "1", "2", "5", "10", "30", "50", "90" };
	static string boxSettings[] = { "Off", "2D", "3D" };
	static string AimKeys[] = { "Left Mouse", "Right Mouse", "Middle Mouse", "Side Mouse 1", "Side Mouse 2" };
	static string ESPSchemes[] = { "Scheme 1", "Scheme 2" };

	static const string menuTitle = "InfinityHacks Valkyrie";

	cMenu::cMenu()
	{
		Visible = true;
		ActiveText = Blue;
		ActiveR = 192;
		ActiveG = 56;
		ActiveB = 60;
		X = 30;
		Y = 100;
	}

	static POINT MousePos()
	{
		POINT cur;
		GetCursorPos(&cur);

		return cur;
	}

	static bool IsMouseInRect(int x, int y, int width, int height)
	{
		const auto cur = MousePos();

		return cur.x >= x && cur.x <= x + width && cur.y >= y && cur.y <= y + height;
	}

	static struct ClickState
	{
		WORD current = 0;
		WORD prev = 0;
	};

	static bool ClickedLeftMouse()
	{
		auto result = false;
		static ClickState leftMouse;
		leftMouse.current = GetAsyncKeyState(VK_LBUTTON);
		if ((leftMouse.current & 0x8001) == 0x8001 && leftMouse.prev == 0)
			result = true;

		leftMouse.prev = leftMouse.current;
		return result;
	}

	static bool ClickedRightMouse()
	{
		auto result = false;
		static ClickState rightMouse;
		rightMouse.current = GetAsyncKeyState(VK_RBUTTON);
		if ((rightMouse.current & 0x8001) == 0x8001 && rightMouse.prev == 0)
			result = true;

		rightMouse.prev = rightMouse.current;
		return result;
	}


	void cMenu::Initialize()
	{
		AddTab("ESP");
		AddTab("Legitbot");
		AddTab("Ragebot");
		AddTab("Misc");

		if (TabIndex == 0)
		{
			AddItem("Enabled", "ESP", Default, &ESPEnabled, 2, "Enables or disables ESP");
			AddItem("Color Scheme", ESPSchemes, &ESPColorScheme, 2, "Choose color scheme for ESP");
			AddItem("Team ESP", Default, &PlayerTeamESP, 2, "Applies all ESP settings to teammates");
			AddItem("Box ESP", boxSettings, &PlayerBoxESP, 3, "Draws a box around players");
			AddItem("Bone ESP", Default, &PlayerBoneESP, 2, "Draws player bones");
			AddItem("Health ESP", Default, &PlayerHealthESP, 2, "Draws a health bar next to players");
			AddItem("Name ESP", Default, &PlayerNameESP, 2, "Draws player names");
			AddItem("Distance ESP", Default, &PlayerDistanceESP, 2, "Draws distance to player in meters");
			AddItem("Weapon ESP", Default, &PlayerWeaponESP, 2, "Draws Player weapons under them");
			AddItem("Glow ESP", Default, &PlayerGlowESP, 2, "Draws a white glow outline around enemies ONLY");
		}
		if (TabIndex == 1)
		{
			AddItem("Enabled", "Legitbot", Default, &LegitBotEnabled, 2, "Enables or disables legit bot");
			AddItem("Aim Key", "Legit Aim Key", AimKeys, &LegitAimKeySetting, 5, "Sets the key for aimbot");
			AddItem("Friendly Fire", "Legit Friendly Fire", Default, &legitBotFriendlyFire, 2, "Enables or disables friendly fire");
			AddItem("Strength", "Aim Strength", legitBotSpeeds, &LegitBotSpeed, 5, "Configure the strength of the legitbot");
			AddItem("FOV", "Legit FOV", AimBotFOVSettings, &legitBotFOV, 10, "Configure FOV of legitbot target aquisition");
			AddItem("RCS", "Legit RCS", Default, &legitBotRCS, 2, "Enable or disable additional recoil control system");
		}
		if (TabIndex == 2)
		{
			AddItem("Enabled", "Ragebot", Default, &RageBotEnabled, 2, "Enables or disables rage bot");
			AddItem("Aim Key", "Rage Aim Key", AimKeys, &RageAimKeySetting, 5, "Sets the key for aimbot");
			AddItem("Friendly Fire", "Rage Friendly Fire", Default, &RagebotFriendlyFire, 2, "Enables or disables friendly fire");
			AddItem("FOV", "Rage FOV", AimBotFOVSettings, &RageBotFOV, 10, "Configure FOV of ragebot target aquisition");
			AddItem("RCS", "Rage RCS", Default, &RageBotRCS, 2, "Enable or disable additional recoil control system");
			AddItem("Bodyshots", rageBotHitboxSettings, &RageBotHitbox, 2, "Choose hitbox for ragebot");
		}

		else if (TabIndex == 3)
		{
			AddItem("Bunnyhop", Default, &BunnyHopEnabled, 2, "Enables bunnyhop");
			AddItem("Chat Spam", Default, &ChatSpam, 2, "Enables team callout chatspam");
			AddItem("Hitmarkers", Default, &Hitmarkers, 2, "Enables Hitmarkers and Hitsounds");
		}
	}

	void cMenu::RenderFramework()
	{
		D3DVIEWPORT9 pViewPort;
		renderer::pRenderer->pDevice->GetViewport(&pViewPort);

		ScreenX = pViewPort.Width;
		ScreenY = pViewPort.Height;

		if (TabMax == 0)
			Initialize();

		if (Visible)
		{
			RenderMenu();

			/*if (SaveSettings)
			{
			SaveSettings = false;
			Settings(true);
			}

			if (LoadSettings)
			{
			LoadSettings = false;
			Settings(false);
			}*/
		}
	}

	void cMenu::AddTab(string Text)
	{
		Tab[TabMax].Text = std::move(Text);
		TabMax++;
	}

	void cMenu::AddItem(string Text, string* Options, int* Variable, int MaxVal, string Desc)
	{
		Item[ItemMax].Text = std::move(Text);
		Item[ItemMax].ImplicitName = Item[ItemMax].Text;
		Item[ItemMax].Options = Options;
		Item[ItemMax].Variable = Variable;
		Item[ItemMax].MaxVal = MaxVal;
		Item[ItemMax].Desc = std::move(Desc);
		ItemMax++;
	}

	void cMenu::AddItem(string Text, string implicitName, string* Options, int* Variable, int MaxVal, string Desc)
	{
		Item[ItemMax].Text = std::move(Text);
		Item[ItemMax].ImplicitName = std::move(implicitName);
		Item[ItemMax].Options = Options;
		Item[ItemMax].Variable = Variable;
		Item[ItemMax].MaxVal = MaxVal;
		Item[ItemMax].Desc = std::move(Desc);
		ItemMax++;
	}
	void cMenu::RenderMenu()
	{
		TabHeight = 24;
		TabWidth = 60;

		ItemHeight = 16;
		ItemWidth = 250;

		TextDistFromEndX = 5;
		TextDistFromEndY = 5;

		BottomBarHeight = 18;
		TopBarHeight = 18;

		TabYDistFromTop = 5;

		FormWidth = ItemWidth + (TextDistFromEndX * 2);
		FormHeight =
			TopBarHeight + TabYDistFromTop + TabHeight + BottomBarHeight + (ItemMax * ItemHeight) + (TextDistFromEndY * 2);

		TabXDistFromEnd = (FormWidth - (TabWidth * TabMax)) / 2;

		SIZE TitleText;
		renderer::pText->GetTextExtent(menuTitle.c_str(), &TitleText);

		// MENU BACK
		renderer::pRenderer->DrawBar(X, Y + TopBarHeight, FormWidth, FormHeight - TopBarHeight, Black, Gray, DGray);

		// TOP BAR
		renderer::pRenderer->DrawBar(X, Y, FormWidth, TopBarHeight, Black, Gray, DDGray);
		renderer::pText->DrawTextA(
			X + (FormWidth / 2), Y + (TopBarHeight / 2) - (TitleText.cy / 2), ItemTextInactive, menuTitle.c_str(), D3DFONT_CENTERED);

		// BOTTOM BAR
		renderer::pRenderer->DrawBar(X, Y + FormHeight - BottomBarHeight, FormWidth, BottomBarHeight, Black, Gray, DGray);

		// TABS
		for (auto i = 0; i < TabMax; i++)
		{
			SIZE TabText;
			renderer::pText->GetTextExtent(Tab[i].Text.c_str(), &TabText);

			const auto TabTextCol = i == TabIndex ? ActiveText : ItemTextInactive;

			renderer::pRenderer->DrawButton(
				X + TabXDistFromEnd + (i * TabWidth),
				Y + TopBarHeight + TabYDistFromTop,
				TabWidth,
				TabHeight,
				Black,
				i == TabIndex ? DGray : Gray,
				i == TabIndex ? Gray : DGray);
			renderer::pText->DrawTextA(
				X + TabXDistFromEnd + (TabWidth / 2) + (i * TabWidth),
				Y + TopBarHeight + (TabHeight / 2) - (TabText.cy / 2) + TabYDistFromTop,
				TabTextCol,
				Tab[i].Text.c_str(),
				D3DFONT_CENTERED);
		}

		// ITEMS
		for (auto i = 0; i < ItemMax; i++)
		{
			SIZE ItemText;
			renderer::pText->GetTextExtent(Item[i].Text.c_str(), &ItemText);

			const auto Val = (Item[i].Variable) ? (*Item[i].Variable) : 0;

			const auto ItemTextCol = Val ? ActiveText : ItemTextInactive;

			// ITEM SELECTED
			if (i == ItemIndex)
			{
				renderer::pRenderer->DrawBar(
					X + TextDistFromEndX,
					Y + TopBarHeight + TabYDistFromTop + TabHeight + TextDistFromEndY + (i * ItemHeight),
					ItemWidth,
					ItemHeight,
					TBlack,
					TBlack,
					TBlack);
				renderer::pText->DrawTextA(
					X + TextDistFromEndX + (ItemWidth / 2),
					Y + FormHeight - (BottomBarHeight / 2) - (ItemText.cy / 2),
					ItemTextInactive,
					Item[i].Desc.c_str(),
					D3DFONT_CENTERED);
			}

			if (Item[i].Options == Single)
			{
				renderer::pText->DrawTextA(
					X + TextDistFromEndX + (ItemWidth / 2),
					Y + TopBarHeight + TabYDistFromTop + TabHeight + TextDistFromEndY + (i * ItemHeight) + (ItemHeight / 2) -
					(ItemText.cy / 2),
					ItemTextCol,
					Item[i].Text.c_str(),
					D3DFONT_CENTERED);
			}
			else
			{
				renderer::pText->DrawTextA(
					X + (TextDistFromEndX * 2),
					Y + TopBarHeight + TabYDistFromTop + TabHeight + TextDistFromEndY + (i * ItemHeight) + (ItemHeight / 2) -
					(ItemText.cy / 2),
					ItemTextCol,
					Item[i].Text.c_str());
				renderer::pText->DrawTextA(
					X + ItemWidth,
					Y + TopBarHeight + TabYDistFromTop + TabHeight + TextDistFromEndY + (i * ItemHeight) + (ItemHeight / 2) -
					(ItemText.cy / 2),
					ItemTextCol,
					Item[i].Options[Val].c_str(),
					D3DFONT_RIGHT);
			}
		}
	}

	void cMenu::ScrollUp()
	{
		ItemIndex--;
		if (ItemIndex < 0)
		{
			PrevTab();
		}
	}

	void cMenu::ScrollDown()
	{
		ItemIndex++;
		if (ItemIndex > ItemMax - 1)
		{
			NextTab();
		}
	}

	void cMenu::PrevTab()
	{
		TabIndex--;

		if (TabIndex < 0)
		{
			TabIndex = TabMax - 1;
		}

		Reset();
		ItemIndex = ItemMax - 1;
	}

	void cMenu::NextTab()
	{
		TabIndex++;

		if (TabIndex > TabMax - 1)
		{
			TabIndex = 0;
		}

		Reset();
		ItemIndex = 0;
	}

	void cMenu::Decrement()
	{
		*Item[ItemIndex].Variable -= 1;
		recentChange = true;
		if (*Item[ItemIndex].Variable < 0)
			*Item[ItemIndex].Variable = Item[ItemIndex].MaxVal - 1;
	}

	void cMenu::Increment()
	{
		*Item[ItemIndex].Variable += 1;
		recentChange = true;
		if (*Item[ItemIndex].Variable > Item[ItemIndex].MaxVal - 1)
			*Item[ItemIndex].Variable = 0;
	}

	void cMenu::MoveToTop() { ItemIndex = 0; }

	void cMenu::MoveToBottom() { ItemIndex = ItemMax - 1; }

	void cMenu::MouseInput()
	{
		if (!Menu.Visible)
			return;

		if (IsMouseInRect(X, Y, FormWidth, TopBarHeight) || readyToDrag)
		{
			const auto cur = MousePos();
			if (ClickedLeftMouse())
			{
				pinnedX = cur.x;
				pinnedY = cur.y;
				readyToDrag = true;
			}

			if (GetAsyncKeyState(VK_LBUTTON) == 0)
				readyToDrag = false;

			if (readyToDrag)
			{
				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				{
					Menu.X -= pinnedX - cur.x;
					Menu.Y -= pinnedY - cur.y;

					pinnedX = cur.x;
					pinnedY = cur.y;
				}
			}
		}

		for (auto i = 0; i < TabMax; i++)
		{
			if (IsMouseInRect(
				X + TabXDistFromEnd + (i * TabWidth), Y + TopBarHeight + TabYDistFromTop, TabWidth, TabHeight) &&
				ClickedLeftMouse())
			{
				TabIndex = i;
				Reset();
			}
		}

		for (auto i = 0; i < ItemMax; i++)
		{
			if (IsMouseInRect(
				X + TextDistFromEndX,
				Y + TopBarHeight + TabYDistFromTop + TabHeight + TextDistFromEndY + (i * ItemHeight),
				ItemWidth,
				ItemHeight))
			{
				ItemIndex = i;

				if (ClickedLeftMouse())
					Increment();
				if (ClickedRightMouse())
					Decrement();
			}
		}
	}

	void cMenu::Settings(bool Save)
	{
		const auto CurTabIndex = TabIndex;

		for (auto i = 0; i < TabMax; i++)
		{
			TabIndex = i;
			Reset();
			for (auto j = 0; j < ItemMax; j++)
			{
				if (Save)
				{
					auto Val = (Item[j].Variable) ? (*Item[j].Variable) : 0;
				}
			}
		}

		TabIndex = CurTabIndex;
		Reset();
	}

	void cMenu::Reset()
	{
		TabMax = 0;
		ItemMax = 0;
		Initialize();
	}
}