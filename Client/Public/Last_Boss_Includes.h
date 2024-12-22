#pragma once

enum class LAST_BOSS_STATE {
	STATE_ROOT,
	STATE_IDLE,
	STATE_WALK,
	STATE_CANNON,
	STATE_PUNCH,
	STATE_STUN,
	STATE_SPARK,
	STATE_APPEAR,

	STATE_END,
};

enum class LAST_BOSS_ANI {
	ANI_CH_Destroyman_DMG_BlownOff_B_In				 ,
	ANI_CH_Destroyman_DMG_BlownOff_B_Landing		 ,
	ANI_CH_Destroyman_DMG_BlownOff_B_Loop			 ,
	ANI_CH_Destroyman_DMG_BlownOff_B_Out			 ,
	ANI_CH_Destroyman_DMG_BlownOff_F_In				 ,
	ANI_CH_Destroyman_DMG_BlownOff_F_Landing		 ,
	ANI_CH_Destroyman_DMG_BlownOff_F_Loop			 ,
	ANI_CH_Destroyman_DMG_BlownOff_F_Out			 ,

	ANI_CH_Destroyman_DMG_Down_B_Loop				 ,
	ANI_CH_Destroyman_DMG_Down_B_Out				 ,
	ANI_CH_Destroyman_DMG_Down_F_Loop				 ,
	ANI_CH_Destroyman_DMG_Down_F_Out				 ,

	ANI_CH_Destroyman_DMG_Force						 ,

	ANI_CH_Destroyman_DMG_Heavy_B					 ,
	ANI_CH_Destroyman_DMG_Heavy_F					 ,
	ANI_CH_Destroyman_DMG_Heavy_L					 ,
	ANI_CH_Destroyman_DMG_Heavy_R					 ,

	ANI_CH_Destroyman_DMG_Light_B					 ,
	ANI_CH_Destroyman_DMG_Light_F					 ,
	ANI_CH_Destroyman_DMG_Light_L					 ,
	ANI_CH_Destroyman_DMG_Light_R					 ,

	ANI_CH_Destroyman_DMG_Multi_F					 ,
	ANI_CH_Destroyman_DMG_Multi_L					 ,
	ANI_CH_Destroyman_DMG_Multi_R					 ,

	ANI_CH_Destroyman_DMG_PickedUpDownHeavy_B		 ,
	ANI_CH_Destroyman_DMG_PickedUpDownHeavy_F		 ,
	ANI_CH_Destroyman_DMG_PickedUpDownLight_B		 ,
	ANI_CH_Destroyman_DMG_PickedUpDownLight_F		 ,

	ANI_CH_Destroyman_DMG_Stun_Loop					 ,
	ANI_CH_Destroyman_DMG_Stun_Out					 ,

	ANI_CH_Destroyman_DMG_TRW_PushBack				 ,
	ANI_CH_Destroyman_DMG_TRW_SwivelTypeBackDrop	 ,
	ANI_CH_Destroyman_DMG_TRW_ThrowLeaveGerman		 ,
	ANI_CH_Destroyman_DMG_TRW_Untergriff			 ,

	ANI_CH_Destroyman_DTH_Down_Death_B				 ,
	ANI_CH_Destroyman_DTH_Down_Death_F				 ,

	ANI_CH_Destroyman_DTH_Down_Freeze_B				 ,
	ANI_CH_Destroyman_DTH_Down_Freeze_F				 ,

	ANI_CH_Destroyman_DTH_Down_Standing				 ,
	ANI_CH_Destroyman_DTH_Down_ThrowLeaveGerman		 ,

	ANI_CH_Destroyman_MOV_Walk_B					 ,
	ANI_CH_Destroyman_MOV_Walk_F					 ,
	ANI_CH_Destroyman_MOV_Walk_L					 ,
	ANI_CH_Destroyman_MOV_Walk_R					 ,

	ANI_CH_DestroymanTF_ATK_DDC_Avoid				 ,

	ANI_CH_DestroymanTF_ATK_DestroyBigCannon		 ,
	ANI_CH_DestroymanTF_ATK_DestroyBuster_Charge_In	 ,
	ANI_CH_DestroymanTF_ATK_DestroyBuster_Charge_Loop,
	ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_In	 ,
	ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_Loop	 ,
	ANI_CH_DestroymanTF_ATK_DestroyBuster_Shot_Out	 ,
	ANI_CH_DestroymanTF_ATK_DestroyCannon			 ,
	ANI_CH_DestroymanTF_ATK_DestroyCannon_Avoid		 ,
	ANI_CH_DestroymanTF_ATK_DestroyChop				 ,
	ANI_CH_DestroymanTF_ATK_DestroyChopSingle		 ,
	ANI_CH_DestroymanTF_ATK_DestroyDoubleCannon		 ,
	ANI_CH_DestroymanTF_ATK_DestroyPunch_In			 ,
	ANI_CH_DestroymanTF_ATK_DestroyPunch_Loop		 ,
	ANI_CH_DestroymanTF_ATK_DestroyPunch_Out		 ,
	ANI_CH_DestroymanTF_ATK_DestroySpark			 ,
	ANI_CH_DestroymanTF_ATK_DestroyUpper			 ,
	ANI_CH_DestroymanTF_ATK_HellStabx2				 ,
	ANI_CH_DestroymanTF_ATK_WLariat					 ,
	ANI_CH_DestroymanTF_ATK_WLariatSingle			 ,

	ANI_CH_DestroymanTF_DTH_Gore_Slash_01_A			 ,
	ANI_CH_DestroymanTF_DTH_Gore_Slash_01_B			 ,
	ANI_CH_DestroymanTF_DTH_Gore_Slash_02_A			 ,
	ANI_CH_DestroymanTF_DTH_Gore_Slash_02_B			 ,

	ANI_CH_DestroymanTF_IDL_CombatNeutral			 ,

	ANI_CH_DestroymanTF_MOV_Avoid_B					 ,
	ANI_CH_DestroymanTF_MOV_Avoid_L					 ,
	ANI_CH_DestroymanTF_MOV_Avoid_R					 ,

	ANI_END,
};