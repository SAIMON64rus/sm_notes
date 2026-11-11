class ActionReadPaperText: ActionSingleUseBase
{
	void ActionReadPaperText()
	{
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
		return "Прочитать";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Paper pp;
		if (Class.CastTo(pp, item))
		{
			return true;
		}
		return false;
	}
	
	override void OnStartClient( ActionData action_data )
	{
		Paper pp = Paper.Cast(action_data.m_MainItem);
		if (pp && g_PHM)
		{
			pp.RequestPaperData();
			g_PHM.RequestPaperHud(0);
		}
	}
};
