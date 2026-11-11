modded class Paper
{
	//! Client-side knowledge flag: the note body is not networked with the entity, the client
	//! learns it only over RPC. Without this flag "Write" was offered on already written
	//! notes after a reconnect, and the server silently refused to start the action.
	private bool m_TextKnown;
	private int m_LastTextRequest;

	//! The text lives in the vanilla WrittenNoteData (m_NoteContents), which Paper already
	//! serializes on its own. No custom persisted field - so installing or removing this
	//! mod never changes the Paper storage format ("Scripted variables corrupted").
	string GetPaperText()
	{
		WrittenNoteData data = GetWrittenNoteData();
		if (!data)
			return "";
		return data.GetNoteText();
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

				// only the player who actually holds this paper may change its text
				PlayerBase plr;
				if (sender)
					plr = PlayerBase.Cast(sender.GetPlayer());
				if (plr && plr.GetHumanInventory())
				{
					Paper inHands = Paper.Cast(plr.GetHumanInventory().GetEntityInHands());
					if (inHands != this)
						return;
				}

				SetPaperText(rpb.param1);
			}
			if (rpc_type == -9048572) // send text
			{
				this.RPCSingleParam(-9048574, new Param2<string, Paper>(GetPaperText(), this), true, sender);
			}
		}
	}

	void RequestPaperData()
	{
		this.RPCSingleParam(-9048572, null, true, null);
	}

	//! Server always knows its own text; the client knows it after a read/write round-trip.
	bool IsTextKnown()
	{
		return GetGame().IsDedicatedServer() || m_TextKnown;
	}

	//! Called from ActionCondition (runs every frame): asks the server for the note body once
	//! per second while the client still does not know whether the note is blank.
	void EnsureTextRequested()
	{
		if (GetGame().IsDedicatedServer() || m_TextKnown)
			return;

		int now = GetGame().GetTime();
		if (now - m_LastTextRequest < 1000)
			return;

		m_LastTextRequest = now;
		RequestPaperData();
	}

	//! Text is stored verbatim. The editor and the reader share the same geometry and font,
	//! so it wraps identically in both - re-flowing it here used to eat line capacity and
	//! silently truncate long notes.
	void SetPaperText(string msg)
	{
		WrittenNoteData data = GetWrittenNoteData();
		if (!data)
			return;

		// SanitizeString is NOT used: it cuts the string at 512 bytes, which for Cyrillic
		// is only ~250 characters (see WrittenNoteDataRaw)
		data.SetNoteTextRaw(msg);
		m_TextKnown = true;

		if (g_PHM && g_PHM.hud)
			g_PHM.hud.SetText(GetPaperText());
	}

	override void SetActions()
	{
		super.SetActions();

		// PunchedCard (bunker key card) and MorsecodeLeaflet are Paper subclasses but not
		// writable notes - they must not get read/write actions.
		if (!IsInherited(PunchedCard) && !IsInherited(MorsecodeLeaflet_ColorBase))
		{
			AddAction(ActionReadPaperText);
			AddAction(ActionWriteOnPaper);
		}
	}

	bool HasAnyText()
	{
		return GetPaperText() != "";
	}
}
