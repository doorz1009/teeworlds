/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "dbl.h"

#include <engine/shared/config.h>

#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/entities/character.h>

CGameControllerDBL::CGameControllerDBL(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	m_pGameType = "DBL";

	m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

void CGameControllerDBL::Tick()
{
	IGameController::Tick();
}

void CGameControllerDBL::OnCharacterSpawn(CCharacter *pChr)
{
	pChr->SetMaxHealth(1);
	pChr->SetMaxArmor(0);
	pChr->IncreaseHealth(1);

	// give default weapons
	pChr->GiveWeapon(WEAPON_HAMMER, -1);
	pChr->GiveWeapon(WEAPON_GUN, 10);
}

int CGameControllerDBL::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	IGameController::OnCharacterDeath(pVictim, pKiller, Weapon);

	if (pVictim->GetPlayer() != pKiller)
	pKiller->AddVictim(pVictim->GetPlayer());

	if(pKiller && Weapon != WEAPON_GAME)
	{
		// do team scoring
		if(pKiller == pVictim->GetPlayer() || pKiller->GetTeam() == pVictim->GetPlayer()->GetTeam())
			m_aTeamscore[pKiller->GetTeam()&1]--; // klant arschel
		else
			m_aTeamscore[pKiller->GetTeam()&1]++; // good shit
	}

	pVictim->GetPlayer()->m_RespawnTick = max(pVictim->GetPlayer()->m_RespawnTick, Server()->Tick()+Server()->TickSpeed()*GameServer()->Config()->m_SvRespawnDelayTDM);

	return 0;
}

