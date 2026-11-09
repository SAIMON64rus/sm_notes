modded class Paper
{
	private string m_WriteText;

	//! Note formatting: the paper writing area is narrow, so a line must not exceed
	//! NOTE_LINE_CHARS characters - the edit box does not wrap by itself and everything
	//! past the 25th character would be drawn outside the paper and become invisible.
	static const int NOTE_LINE_CHARS = 25;
	static const int NOTE_MAX_LINES = 16;

	//! Re-flows text into lines of at most NOTE_LINE_CHARS characters (word-aware where possible).
	static string WrapNoteText(string src)
	{
		string flat = src;
		flat.Replace("\n", " ");

		string result = "";
		int pos = 0;
		int len = flat.Length();
		int line = 0;

		while (pos < len && line < NOTE_MAX_LINES)
		{
			int take = NOTE_LINE_CHARS;
			if (pos + take > len)
				take = len - pos;

			if (pos + take < len)
			{
				string chunk = flat.Substring(pos, take);
				int space = chunk.LastIndexOf(" ");
				if (space > 0)
					take = space;
			}

			if (line > 0)
				result = result + "\n";
			result = result + flat.Substring(pos, take);
			pos = pos + take;

			// skip the separator space we broke on
			while (pos < len && flat.Substring(pos, 1) == " ")
				pos++;

			line++;
		}

		return result;
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		// m_WriteText is appended after the vanilla WrittenNoteData, so saves made
		// before this mod was installed do not contain it. A failed read here is a
		// normal migration case (field stays empty), not a corrupted entity - returning
		// false would spam "Scripted variables corrupted upon Paper" for every paper
		// already present in the world.
		if (version >= 108)
			ctx.Read(m_WriteText);
				
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
				Print("[sm_notes] client apply len=" + paperData.param1.Length());
				Paper pp = paperData.param2;
				if (pp)
					pp.SetPaperText(paperData.param1);
			}
		}
		if (GetGame().IsServer())
		{
			if (rpc_type == -9048573) // save new text
			{
				Param1<string> rpb;
				if (!ctx.Read(rpb)) return;
				m_WriteText = Paper.WrapNoteText(MiscGameplayFunctions.SanitizeString(rpb.param1));
				Print("[sm_notes] server save len=" + m_WriteText.Length());
			}
			if (rpc_type == -9048572) // send text
			{
				Print("[sm_notes] server read len=" + m_WriteText.Length());
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
		m_WriteText = Paper.WrapNoteText(msg);
		if (g_PHM && g_PHM.hud)
			g_PHM.hud.SetText(m_WriteText);
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionReadPaperText);
		AddAction(ActionWriteOnPaper);
	}

	bool HasAnyText()
	{
		return m_WriteText.Length();
	}
}