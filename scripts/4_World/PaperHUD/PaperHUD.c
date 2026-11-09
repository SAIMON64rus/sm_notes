class PaperHUD extends UIScriptedMenu
{
	private ImageWidget				m_Img;
	private MultilineTextWidget 	m_TxtBox;
	private MultilineEditBoxWidget 	m_EditBox;

	private int						m_Type;
	private string					m_Message;

	private const string TexturePath = "Kas_Notes/layouts/loot_paper_co.edds";

	void PaperHUD(int type)
    {
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		m_Type = type;
    }

    void ~PaperHUD()
    {
		if (!GetGame())
			return;

		if (m_Type && m_EditBox)
		{
			string text;
			m_EditBox.GetText(text);
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			Paper pp;
			if (player && player.GetHumanInventory())
				pp = Paper.Cast(player.GetHumanInventory().GetEntityInHands());
			if (pp)
				pp.RPCSingleParam(-9048573, new Param1<string>(text), true, null);
		}

		Mission mission = GetGame().GetMission();
		if (mission)
			mission.PlayerControlEnable(true);
    }

	override Widget Init()
	{
		layoutRoot 	= GetGame().GetWorkspace().CreateWidgets("Kas_Notes/layouts/Paper.layout");
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
		if (m_TxtBox)
			m_TxtBox.SetText(msg);
		if (m_EditBox)
			m_EditBox.SetText(msg);
	}
}
