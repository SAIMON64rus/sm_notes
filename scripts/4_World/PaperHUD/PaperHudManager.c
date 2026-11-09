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
		if (hud || GetGame().GetUIManager().GetMenu()) return;
		GetGame().GetMission().GetHud().ShowQuickbarUI( false );
        hud = new PaperHUD(type);
        GetGame().GetUIManager().ShowScriptedMenu( hud, NULL );
	}

	void CloseMenu()
	{
		if ( GetGame().GetUIManager().GetMenu() && hud)
		{
			// Persist before Back(): the widget tree is destroyed by the engine on close.
			hud.SaveText();
			GetGame().GetUIManager().Back();
			GetGame().GetMission().GetHud().ShowQuickbarUI( true );
			hud = null;
		}
	}
}
ref PaperHudManager g_PHM;
