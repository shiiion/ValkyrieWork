#pragma once
#include "res.h"
#include <set>

namespace menu
{

	class cMenu
	{
		using string = std::string;
	public:
		DWORD ActiveText;

		int NewR, NewG, NewB;
		int ActiveR, ActiveG, ActiveB;

		bool isShiftDown;
		int pinnedX = 0;
		int pinnedY = 0;
		bool readyToDrag = false;

		struct
		{
			string Text;
		}Tab[25];

		struct
		{
			string Text;
			string ImplicitName;
			string Desc;
			string *Options;
			int  *Variable;
			int MaxVal;
		}Item[100];

		cMenu();

		void Initialize();

		void AddTab(string Text);
		void AddItem(string Text, string *Options, int *Variable, int MaxVal, string Desc);
		void AddItem(string Text, string ImplicitName, string *Options, int *Variable, int MaxVal, string Desc);

		void RenderFramework();
		void RenderMenu();

		void MouseInput();

		void ScrollUp();
		void ScrollDown();

		void PrevTab();
		void NextTab();

		void Decrement();
		void Increment();

		void MoveToTop();
		void MoveToBottom();

		void Reset();


		void Settings(bool Save);
		//int GetInt(char *AppName, char *KeyName);
		//BOOL SetInt(char *AppName, char *KeyName, int Data);

		char Path[255];

		int TabIndex, ItemIndex, TabMax, ItemMax, Visible, Key, Y, X, ScreenX, ScreenY;

		int TabHeight, TabWidth, ItemHeight, ItemWidth, TextDistFromEndX, TextDistFromEndY, BottomBarHeight, TopBarHeight, TabYDistFromTop, TabXDistFromEnd, FormWidth, FormHeight;

		bool recentChange;

		int ESPEnabled,
			ESPColorScheme,
			PlayerTeamESP,
			PlayerBoxESP,
			PlayerBoneESP,
			PlayerHealthESP,
			PlayerNameESP,
			PlayerDistanceESP,
			PlayerWeaponESP,

			LegitBotEnabled,
			LegitAimKeySetting,
			LegitBotSpeed,
			LegitBotRandom,
			legitBotFOV,
			legitBotRCS,
			legitBotFriendlyFire,

			RageBotEnabled,
			RageAimKeySetting,
			RageBotFOV,
			RageBotRCS,
			RageBotHitbox,
			RagebotFriendlyFire,

			BunnyHopEnabled,
			ChatSpam,
			Hitmarkers,
			PlayerGlowESP;

	};

	extern cMenu Menu;
}