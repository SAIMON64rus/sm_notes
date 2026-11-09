class PaperHUD extends UIScriptedMenu
{
	private ImageWidget				m_Img;
	private MultilineTextWidget 	m_TxtBox;
	private MultilineEditBoxWidget 	m_EditBox;

	private int						m_Type;
	private string					m_Message;
	private bool					m_Saved;
	private bool					m_Reflow;

	private const string TexturePath = "sm_notes/layouts/loot_paper_co.edds";

	void PaperHUD(int type)
    {
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
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
		text = Paper.WrapNoteText(text);

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		Paper pp;
		if (player && player.GetHumanInventory())
			pp = Paper.Cast(player.GetHumanInventory().GetEntityInHands());
		if (!pp)
			return;

		pp.RPCSingleParam(-9048573, new Param1<string>(text), true, null);
		// keep the local copy in sync with the server, otherwise the client still offers
		// "Write" on a note that already has text (ActionCondition reads this field)
		pp.SetPaperText(text);
		m_Saved = true;

		Print("[sm_notes] client save len=" + text.Length());
	}

	override Widget Init()
	{
		layoutRoot 	= GetGame().GetWorkspace().CreateWidgets("sm_notes/layouts/Paper.layout");
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
				m_EditBox.Show(true);
		}

		return layoutRoot;
	}

	void SetText(string msg)
	{
		m_Message = msg;
		string wrapped = Paper.WrapNoteText(msg);
		if (m_TxtBox)
			m_TxtBox.SetText(wrapped);
		if (m_EditBox)
			m_EditBox.SetText(wrapped);
	}

	//! The edit box does not wrap by itself: without this every character past the
	//! 25th on a line is drawn outside the paper and the player cannot see it.
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w == m_EditBox && !m_Reflow && m_EditBox)
		{
			string txt;
			m_EditBox.GetText(txt);
			string wrapped = Paper.WrapNoteText(txt);
			if (wrapped != txt)
			{
				m_Reflow = true;
				m_EditBox.SetText(wrapped);
				m_Reflow = false;
			}
		}

		return super.OnChange(w, x, y, finished);
	}
}
