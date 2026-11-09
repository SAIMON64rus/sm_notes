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
		Paper pp;
		if (Class.CastTo(pp, item))
		{
			return (!pp.HasAnyText());
		}
		return false;
	}
	
	override void OnStartClient( ActionData action_data )
	{
		Paper pp = Paper.Cast(action_data.m_MainItem);
		if (pp)
		{
			pp.RequestPaperData();
			g_PHM.RequestPaperHud(1);
		}
	}
};
