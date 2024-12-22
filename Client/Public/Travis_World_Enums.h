#pragma once

enum class WORLDTRAVIS_STATES {
	// Common
	STATE_IDLE,
	STATE_MOV,
	STATE_MOV_OUT,
	STATE_RISINGUP,
	STATE_RIDE_START,
	STATE_RIDING,
	STATE_RIDING_OUT,

	// Event
	STATE_EVE_ITEMGET,
	STATE_EVE_PHONECALL,
	STATE_EVE_SUCCESS,
	STATE_EVE_ENTERPIPE,
	STATE_EVE_GETCRYSTAL,
	STATE_EVE_SUSHI,

	// In House
	STATE_POLE,
	STATE_EVE_WITHJEANE,

	// In Toilet
	STATE_TOILET,

	// In TrashCollector
	STATE_TC_IDLE,
	STATE_TC_MOV,
	STATE_TC_PICKINGUP,
	STATE_TC_BITEATTACK_IN,
	STATE_TC_BITEATTACK_LOSE,
	STATE_TC_BITEATTACK_WIN,
	STATE_TC_BITEATTACK_WIN_BRAINBUSTER,

	// In ToiletGame
	STATE_TG_Idle,
	STATE_TG_Hit,

	STATES_END
};

enum class WORLDTRAVIS_ANIMLIST {
	ANI_PL_Travis_MOV_NonBattle_Dash,
	ANI_PL_Travis_MOV_NonBattle_Dash_Out,
	ANI_PL_Travis_MOV_NonBattle_Run,
	ANI_PL_Travis_MOV_NonBattle_Run_Out,
	ANI_PL_Travis_MOV_NonBattle_Walk,
	ANI_PL_Travis_MOV_NonBattle_Walk_Out,
	ANI_PL_Travis_IDL_NonBattle_Neutral,
	ANI_PL_Travis_EVE_PalmTree_Climb_S,
	ANI_PL_Travis_EVE_PalmTree_Fall_S,
	ANI_PL_Travis_EVE_PalmTree_Grab_Loop_S,
	ANI_PL_Travis_EVE_PalmTree_Grab_S,
	ANI_PL_Travis_EVE_PalmTree_Leave_S,
	ANI_PL_Travis_EVE_NonBattle_CattleMutation_RiseUp,
	ANI_PL_Travis_EVE_NonBattle_CattleMutation_Rising,
	ANI_PL_Travis_EVE_NonBattle_Toilet_In,
	ANI_PL_Travis_EVE_NonBattle_Toilet_Loop,
	ANI_PL_Travis_EVE_NonBattle_Toilet_Out,
	ANI_PL_Travis_IDL_Vehicle_Neutral,
	ANI_PL_Travis_IDL_Vehicle_Neutral_AKIRA,
	ANI_PL_Travis_IDL_Vehicle_Neutral_AKIRA_R,
	ANI_PL_Travis_IDL_Vehicle_Neutral_L_In,
	ANI_PL_Travis_IDL_Vehicle_Neutral_L_Out,
	ANI_PL_Travis_IDL_Vehicle_Neutral_R_In,
	ANI_PL_Travis_IDL_Vehicle_Neutral_R_Out,
	ANI_PL_Travis_IDL_Vehicle_Neutral_Upright,
	ANI_PL_Travis_IDL_Vehicle_Neutral_Victory,
	ANI_PL_Travis_EVE_NonBattle_SushiStand_Eat,
	ANI_PL_Travis_EVE_NonBattle_SushiStand_In,
	ANI_PL_Travis_EVE_NonBattle_SushiStand_Loop,
	ANI_PL_Travis_EVE_NonBattle_SushiStand_Out,
	ANI_PL_Travis_IDL_NonBattle_CatToy2Jean_In,
	ANI_PL_Travis_IDL_NonBattle_CatToy2Jean_Loop,
	ANI_PL_Travis_IDL_NonBattle_CatToy2Jean_Out,
	ANI_PL_Travis_EVE_NonBattle_Success_In,
	ANI_PL_Travis_EVE_NonBattle_Success_Loop,
	ANI_PL_Travis_EVE_NonBattle_Success2_In,
	ANI_PL_Travis_EVE_NonBattle_Success2_Loop,
	ANI_PL_Travis_EVE_NonBattle_Success3_In,
	ANI_PL_Travis_EVE_NonBattle_Success3_Loop,
	ANI_PL_Travis_EVE_Toilet_Cleaning,
	ANI_PL_Travis_EVE_Toilet_CombatNeutral,
	ANI_PL_Travis_EVE_ToiletCleaning_GriGri_B,
	ANI_PL_Travis_EVE_ToiletCleaning_GriGri_F,
	ANI_PL_Travis_EVE_ToiletCleaning_GriGri_L,
	ANI_PL_Travis_EVE_ToiletCleaning_GriGri_R,
	ANI_PL_Travis_EVE_ToiletCleaning_Hit,
	ANI_PL_Travis_EVE_ToiletCleaning_HitFinish,
	ANI_PL_Travis_EVE_ToiletCleaning_HitNeutral,
	ANI_PL_Travis_EVE_ToiletCleaning_Neutral,
	ANI_PL_Travis_EVE_ItemGET,
	ANI_PL_Travis_EVE_ItemGET_Loop,
	ANI_PL_Travis_EVE_Telephone_Call_In,
	ANI_PL_Travis_EVE_Telephone_Call_Out,
	ANI_PL_Travis_IDL_Telephone_Neutral,
	ANI_PL_Travis_MOV_NonBattle_Jump_In,
	ANI_PL_Travis_MOV_NonBattle_Jump_Landing,
	ANI_PL_Travis_MOV_NonBattle_Jump_Loop,
	ANI_PL_Travis_EVE_NonBattle_CatCapture_Out,

	ANIMLIST_END
};

enum class WORLDTRAVIS_RUNNING_TOGGLE {
	TOGGLE_WALK,
	TOGGLE_RUN,
	TOGGLE_DASH,

	TOGGLE_END
};

enum class WORLDTRAVIS_CAMERA_ANIMLIST {
	ANI_PL_Travis_SKL_Warp_Out_cam,
	ANI_PL_Travis_EVE_Inhale_Cam,
	ANI_PL_Travis_TRW_BackDrop_Treatment_Cam,
	ANI_PL_AMTravis_ATK_STG_RailGun_Cam,
	ANI_PL_Travis_EVE_NonBattle_CattleMutation_Rising_Cam,
	ANI_PL_Travis_IDL_Transform_AMTGR_Cam,
	ANI_PL_Travis_EVE_BiteAttack_BrainBuster_Win_Cam,

	ANIMLIST_END
};

enum class WORLDTRAVIS_SUBMODELLIST {
	PLAYER_NMH1_TOILET,
	PLAYER_NMH1_TRASH,

	SUBMODELLIST_END
};

enum class WORLDTRAVIS_WEAPONS {
	WEAPON_TRASH_TONG,
	WEAPON_TRASH_TRASHCAN,
	WEAPON_TOILET_RUBBERCUP,

	WEAPON_END
};

enum class WORLDTRAVIS_FIXEDDIR {
	DIR_N,
	DIR_NE,

	DIR_E,
	DIR_ES,

	DIR_S,
	DIR_SW,

	DIR_W,
	DIR_WN,

	DIR_NONE,
	DIR_FIND,

	DIR_END
};

enum class TOILETTRAVIS_ANIMLIST {
	ANI_PL_Travis_EVE_NonBattle_Toilet_In,
	ANI_PL_Travis_EVE_NonBattle_Toilet_Loop,
	ANI_PL_Travis_EVE_NonBattle_Toilet_Out,

	ANIMLIST_END
};

enum class TRASHTRAVIS_ANIMLIST {
	ANI_PL_Travis_EVE_BiteAttack,
	ANI_PL_Travis_EVE_BiteAttack_BrainBuster_Win,
	ANI_PL_Travis_EVE_BiteAttack_DDT,
	ANI_PL_Travis_EVE_BiteAttack_DDT_LawnStandUp,
	ANI_PL_Travis_EVE_BiteAttack_DDT_StandUp,
	ANI_PL_Travis_EVE_BiteAttack_Lose,
	ANI_PL_Travis_EVE_BiteAttack_loss_LawnStandUp,
	ANI_PL_Travis_EVE_BiteAttack_loss_StandUp,
	ANI_PL_Travis_EVE_BiteAttack_Win,
	ANI_PL_Travis_EVE_NonBattle_Success_In,
	ANI_PL_Travis_EVE_NonBattle_Success_Loop,
	ANI_PL_Travis_EVE_NonBattle_Success2_In,
	ANI_PL_Travis_EVE_NonBattle_Success2_Loop,
	ANI_PL_Travis_EVE_NonBattle_Success3_In,
	ANI_PL_Travis_EVE_NonBattle_Success3_Loop,
	ANI_PL_Travis_EVE_PickingUpTrash,
	ANI_PL_Travis_EVE_PickingUpTrash_Bottom,
	ANI_PL_Travis_MOV_Run,
	ANI_PL_Travis_MOV_Run_Out,
	ANI_PL_Travis_IDL_Neutral,

	ANIMLIST_END
};