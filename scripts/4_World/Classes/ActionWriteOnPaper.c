class ActionWriteOnPaperCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(2);
	}
}

class ActionWriteOnPaper : ActionContinuousBase
{
	void ActionWriteOnPaper()
	{
		m_CallbackClass = ActionWriteOnPaperCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;		
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override string GetText()
	{
		return "Написать";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		// original behaviour: a note can be written only once - afterwards it is read-only.
		// The client does not know the body until it is read, so ask for it once and keep
		// the action hidden until the answer arrives (otherwise "Write" shows up on an
		// already written note after a reconnect and the server refuses to start it).
		Paper pp;
		if (!Class.CastTo(pp, item))
			return false;

		pp.EnsureTextRequested();
		return (pp.IsTextKnown() && !pp.HasAnyText());
	}
	
	override void OnStartClient( ActionData action_data )
	{
		Paper pp = Paper.Cast(action_data.m_MainItem);
		if (pp && g_PHM)
		{
			pp.RequestPaperData();
			g_PHM.RequestPaperHud(1);
		}
	}
};
