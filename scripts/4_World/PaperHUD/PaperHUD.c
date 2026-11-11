class PaperHUD extends UIScriptedMenu
{
	private ImageWidget				m_Img;
	private MultilineTextWidget 	m_TxtBox;
	private MultilineEditBoxWidget 	m_EditBox;

	private int						m_Type;
	private bool					m_Saved;

	private const string TexturePath = "sm_notes/layouts/loot_paper_co.edds";

	void PaperHUD(int type)
	{
		Mission mission = GetGame().GetMission();
		if (mission)
			mission.PlayerControlDisable(INPUT_EXCLUDE_ALL);
		m_Type = type;
	}

	void ~PaperHUD()
	{
		if (!GetGame())
			return;

		// Fallback: normally the text is already sent by SaveText() before the menu closes.
		SaveText();

		Mission mission = GetGame().GetMission();
		if (mission)
			mission.PlayerControlEnable(true);

		// release the manager's reference even when the menu was closed by the engine
		// (death, disconnect) instead of CloseMenu() - otherwise the next note never opens
		if (g_PHM && g_PHM.hud == this)
			g_PHM.hud = null;
	}

	//! Sends the edited text to the server. MUST be called before UIManager.Back():
	//! once the menu is closed the widget tree is gone and GetText() can no longer be read,
	//! so saving from the destructor alone silently loses everything the player typed.
	void SaveText()
	{
		if (m_Saved || m_Type == 0)
			return;
		if (!GetGame() || !m_EditBox)
			return;

		string text;
		m_EditBox.GetText(text);

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		Paper pp;
		if (player && player.GetHumanInventory())
			pp = Paper.Cast(player.GetHumanInventory().GetEntityInHands());
		if (!pp)
			return;

		pp.RPCSingleParam(-9048573, new Param1<string>(text), true, null);
		// keep the local copy in sync with the server so both sides show the same note
		pp.SetPaperText(text);
		m_Saved = true;
	}

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("sm_notes/layouts/Paper.layout");
		if (!layoutRoot)
			return null;

		m_TxtBox	= MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("TxtBox"));
		m_EditBox	= MultilineEditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBox"));
		m_Img		= ImageWidget.Cast(layoutRoot.FindAnyWidget("Img"));

		if (m_Img)
			m_Img.LoadImageFile(0, TexturePath);

		if (m_Type == 0)
		{
			if (m_TxtBox)
				m_TxtBox.Show(true);
		}
		else
		{
			if (m_EditBox)
			{
				m_EditBox.Show(true);
			}
		}

		return layoutRoot;
	}

	void SetText(string msg)
	{
		string wrapped = msg;
		if (m_TxtBox)
			m_TxtBox.SetText(wrapped);
		if (m_EditBox)
			m_EditBox.SetText(wrapped);

		// an RPC with the note text can arrive right after the menu opened and steal
		// the focus from the edit box, leaving the player unable to type
		FocusEditBox();
	}

	//! Keyboard input only reaches the edit box when it has focus; UIScriptedMenu does not
	//! focus widgets automatically (vanilla does it by hand: chatinputmenu.c:58).
	void FocusEditBox()
	{
		if (m_Type != 0 && m_EditBox)
			SetFocus(m_EditBox);
	}

	override void OnShow()
	{
		super.OnShow();
		FocusEditBox();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		// with the edit box focused ESC can be consumed by the widget, so close the note
		// the same way the vanilla note menu does (5_mission/gui/notemenu.c:130-138)
		if (g_PHM && g_PHM.hud == this && GetUApi().GetInputByID(UAUIBack).LocalPress())
			g_PHM.CloseMenu();
	}

}
