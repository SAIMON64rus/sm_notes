class PaperHudManager
{
	ref PaperHUD hud;

	void OnKeyPress(int key)
	{
		switch (key)
		{
			case KeyCode.KC_ESCAPE:
				CloseMenu();
			break;
		}
	}

	void RequestPaperHud(int type)
	{
		if (hud || GetGame().GetUIManager().GetMenu())
			return;

		Mission mission = GetGame().GetMission();
		if (mission && mission.GetHud())
			mission.GetHud().ShowQuickbarUI(false);

		hud = new PaperHUD(type);
		GetGame().GetUIManager().ShowScriptedMenu(hud, NULL);
	}

	void CloseMenu()
	{
		if (hud && GetGame().GetUIManager().GetMenu())
		{
			// Persist before Back(): the widget tree is destroyed by the engine on close.
			hud.SaveText();
			GetGame().GetUIManager().Back();

			Mission mission = GetGame().GetMission();
			if (mission && mission.GetHud())
				mission.GetHud().ShowQuickbarUI(true);

			hud = null;
		}
	}
}
ref PaperHudManager g_PHM;
