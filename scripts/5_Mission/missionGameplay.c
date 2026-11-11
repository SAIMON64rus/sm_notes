modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();

		g_PHM = new PaperHudManager();
	}

	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);

		if (g_PHM)
			g_PHM.OnKeyPress(key);
	}
}
