modded class Paper
{
	private string m_WriteText;

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		if (version >= 108 && !ctx.Read(m_WriteText))
			return false;
				
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_WriteText);		
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		if (GetGame().IsClient())
		{
			if(rpc_type == -9048574) // apply info
			{
				Param2<string, Paper> paperData;
				if (!ctx.Read(paperData)) return;
				Paper pp = paperData.param2;
				pp.SetPaperText(paperData.param1);
			}
		}
		if (GetGame().IsServer())
		{
			if (rpc_type == -9048573) // save new text
			{
				Param1<string> rpb;
				if (!ctx.Read(rpb)) return;
				m_WriteText = rpb.param1;
			}
			if (rpc_type == -9048572) // send text
			{
				this.RPCSingleParam(-9048574, new Param2<string, Paper>(m_WriteText, this), true, sender);
			}
		}
	}

	void RequestPaperData()
	{
		this.RPCSingleParam(-9048572, null, true, null);
	}

	void ApplyPaperData(ParamsReadContext ctx)
	{
		Param1<string> rpb;
		if (!ctx.Read(rpb)) return;
		m_WriteText = rpb.param1;
	}

	void SetPaperText(string msg)
	{
		m_WriteText = msg;
		g_PHM.hud.SetText(m_WriteText);
	}

	override void SetActions()
	{
		//super.SetActions();
		AddAction(ActionTakeItem);
		AddAction(ActionTakeItemToHands);
		AddAction(ActionReadPaperText);
		AddAction(ActionWriteOnPaper);
	}

	bool HasAnyText()
	{
		return m_WriteText.Length();
	}
}