modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();

		g_PHM		= new PaperHudManager();
	}

	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		g_PHM.OnKeyPress(key);
	}
}