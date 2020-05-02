# Feature: Dodgeball Mode

## Feature 1: One-Hit Kill:
### User Story:
As a teeworlds user, I would like for the enemy hit by the ball to be out of the game after one hit so that the game is similar to the way that dodgeball really works.
* Reduce starting health so that enemies are killed by one hit.

## Feature 2: Respawn on Killer's Death:
### User Story:
As a teeworlds user, I would like to respawn when the enemy that killed me is killed so that this game mode will be similar to the way that dodgeball is really played.
* Keep track of who killed a player
* When the killer dies, resurrect the player

## Feature 3: Round End on No Enemies Remaining:
### User Story:
As a tee worlds user, I would like be able to win the game when all enemy players have been eliminated so that this game mode works like dodgeball really does.
* Detect number of enemies on each death. If there is one team with nobody left, then the other team wins.


## Requirements:
1. All players shall start with one hit point.
1. A new gamemode shall be available named DBL.
1. The gamemode shall end when any one team has no players alive.
1. Killers shall keep a list of their victims.
1. Players shall not respawn until their killer is killed.
1. All victims on a player's list shall respawn upon the player's death.
1. Hit points shall not exceed 1 hit point.
1. Armor points shall not exceed 0.
1. The gamemode shall award the match win to the team with players still alive at the end of the round.
