/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */



/* This Game Controller allows us to host a server which has a dodgeball gamemode
 * The existance of this controller provides the groundwork for a dodgeball game mode to be hosted. (Requirement 2).
 */

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

	// The survival game flag helps with Requirement 5: Players do not respawn until their killer is killed.
	m_GameFlags = GAMEFLAG_TEAMS | GAMEFLAG_SURVIVAL; // GAMEFLAG_TEAMS makes it a two-team gamemode and disallows automatic respawn.
}

void CGameControllerDBL::Tick()
{
	IGameController::Tick();
}

void CGameControllerDBL::OnCharacterSpawn(class CCharacter *pChr)
{	// When we spawn a character, we will limit their health and armor as well as prevent them from automatically respawning.
	// (Requirements 1, 5, 7, and 8)
	pChr->SetMaxHealth(1);
	pChr->SetMaxArmor(0);
	pChr->IncreaseHealth(1);

	// give default weapons
	pChr->GiveWeapon(WEAPON_GUN, 10);

	// prevent respawn
	pChr->GetPlayer()->m_RespawnDisabled = GetStartRespawnState();
}

int CGameControllerDBL::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{	// On a character's death, we will revive their victims and place them on their killer's list of victims. (Requirements 4, 5, and 6)
	IGameController::OnCharacterDeath(pVictim, pKiller, Weapon);

	if (pVictim->GetPlayer() != pKiller)
	pKiller->AddVictim(pVictim->GetPlayer());

	for (auto victim : pVictim->GetPlayer()->GetVictims())
	{
		victim->m_RespawnDisabled = false;
		victim->Respawn();
		victim->m_RespawnTick = Server()->Tick();
	}
	pVictim->GetPlayer()->ClearVictims();

	pVictim->GetPlayer()->m_RespawnTick = max(pVictim->GetPlayer()->m_RespawnTick, Server()->Tick()+Server()->TickSpeed()*GameServer()->Config()->m_SvRespawnDelayTDM);

	return 0;
}

void CGameControllerDBL::DoWincheckRound()
{
	// This function runs every server tick to check for a win condition. This will assist in completing requirement 3
	// and 9 to determine who won the round and when the round is finished.
	int Count[2] = {0}; // Array for keeping track of how many players are alive on each team.
	for(int i(0); i < MAX_CLIENTS; i++)
	{	// tally up how many team members are alive on each team.
		if(GameServer()->m_apPlayers[i] && // Does the player at this index exist?
		   GameServer()->m_apPlayers[i]->GetTeam() != TEAM_SPECTATORS && // Does the extant player belong to the spectator team?
		   (!GameServer()->m_apPlayers[i]->m_RespawnDisabled || (GameServer()->m_apPlayers[i]->GetCharacter() && GameServer()->m_apPlayers[i]->GetCharacter()->IsAlive())))
		 ++Count[GameServer()->m_apPlayers[i]->GetTeam()];	// Player exists, is not dead, and is not spectating.
	}

	if(Count[TEAM_RED]+Count[TEAM_BLUE] == 0 || (m_GameInfo.m_TimeLimit > 0 && (Server()->Tick()-m_GameStartTick) >= m_GameInfo.m_TimeLimit*Server()->TickSpeed()*60))
	{	// If the last member of both teams dies at the same time or if the time runs out, tie.
		++m_aTeamscore[TEAM_BLUE];
		++m_aTeamscore[TEAM_RED];
		EndRound();
	}
	else if(Count[TEAM_RED] == 0)
	{	// Red team is completely dead, blue wins! (Requirement 9 and 3)
		++m_aTeamscore[TEAM_BLUE];
		EndRound();
	}
	else if(Count[TEAM_BLUE] == 0)
	{	// Blue team is completely dead, red wins! (Requirement 9 and 3)
		++m_aTeamscore[TEAM_RED];
		EndRound();
	}
}
