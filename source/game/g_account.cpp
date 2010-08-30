#include "g_local.h"
#include "g_account.h"
#include "string.h"
#include <algorithm>
#include "sqlite3/sqlite3.h"
#include "sqlite3/libsqlitewrapped.h"
#include "q_shared.h"


//====Account Functions====//

/*
=================

Cmd_AccountLogin_f

Ingame command : login <user> <password>
Account Login

=================
*/
void Cmd_AccountLogin_F( gentity_t * targetplayer )
{
	Database db(DATABASE_PATH);
	char userName[MAX_STRING_CHARS];
	char userPassword[MAX_STRING_CHARS];

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	//Make sure they entered both a user and a password
	if( trap_Argc() < 3 ){
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"Usage: login <user> <password>\n\"");
		return;
	}
	//Check if we're already logged in 
	if(isLoggedIn(targetplayer))
	{
		trap_SendServerCommand ( targetplayer->client->ps.clientNum, "print \"^1Statement: ^7Already logged in!\n\"");
		return;
	}
	//Get the username and password
	trap_Argv( 1, userName, MAX_STRING_CHARS );
	std::string userNameSTR = userName;
	trap_Argv( 2, userPassword, MAX_STRING_CHARS );

	//Check if this username exists
	Query q(db);
	std::transform(userNameSTR.begin(), userNameSTR.end(),userNameSTR.begin(),::tolower);
	std::string DBname = q.get_string(va("SELECT name FROM users WHERE name='%s'",userNameSTR.c_str()));
	if(DBname.empty())
	{
		trap_SendServerCommand ( targetplayer->client->ps.clientNum, va("print \"^1User %s has not been found\n\"",userName));
		return;
	}
	//Check password
	std::string DBpassword = q.get_string(va("SELECT password FROM users WHERE name='%s'",userNameSTR.c_str()));
	if(DBpassword.empty() || strcmp(DBpassword.c_str(),userPassword) != 0 )
	{
		trap_SendServerCommand ( targetplayer->client->ps.clientNum, va("print \"^1Password incorrect\n\"",DBpassword.c_str()));
		return;
	}
	//Log the user in
	int userID = q.get_num(va("SELECT ID FROM users WHERE name='%s'",userNameSTR.c_str()));
	targetplayer->client->sess.userID = userID;
	targetplayer->client->sess.loggedinAccount = qtrue;
	LoadUser(targetplayer);
	trap_SendServerCommand( targetplayer->client->ps.clientNum, va("print \"^1Success: ^7You are now logged in as %s!\n\"",userName));
	//Update the ui
	trap_SendServerCommand( targetplayer->client->ps.clientNum, va("lui_login"));
	return;
}

/*
=================

Cmd_AccountLogout_f

Ingame command : logout
Account Logout

=================
*/

void Cmd_AccountLogout_F(gentity_t * targetplayer)
{
	if(!isLoggedIn(targetplayer))
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: Not logged in fool.\n\"");
		return;
	}

	//Save the character
	if(targetplayer->client->sess.characterChosen == qtrue)
		SaveCharacter(targetplayer);

	//Account
	targetplayer->client->sess.loggedinAccount = qfalse;
	targetplayer->client->sess.userID = NULL;
	//Character
	targetplayer->client->sess.characterChosen = qfalse;
	targetplayer->client->sess.characterID = NULL;
	trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Success: ^7Your are now logged out\n\"");
	//Remove all feats
	for(int k = 0; k < NUM_FEATS-1; k++)
	{
		targetplayer->client->featLevel[k] = FORCE_LEVEL_0;
	}
	//Remove all character skills
	for(int i = 0; i < NUM_SKILLS-1; i++)
	{
		targetplayer->client->skillLevel[i] = FORCE_LEVEL_0;
	}
	//Remove all force powers
	targetplayer->client->ps.fd.forcePowersKnown = 0;
	for(int j = 0; j < NUM_FORCE_POWERS-1; j++)
	{
		targetplayer->client->ps.fd.forcePowerLevel[j] = FORCE_LEVEL_0;
	}
	//Respawn client
	targetplayer->flags &= ~FL_GODMODE;
	targetplayer->client->ps.stats[STAT_HEALTH] = targetplayer->health = -999;
	SetTeam(targetplayer,"s");
	//Update the ui
	trap_SendServerCommand( targetplayer->client->ps.clientNum, va("lui_logout"));
	return;
}

/*
=================

Cmd_AccountCreate_f

Ingame command : register <user> <password>
Account Creation

=================
*/
void Cmd_AccountCreate_F(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);
	char userName[MAX_STRING_CHARS];
	char userPassword[MAX_STRING_CHARS];
	
	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}

	//Make sure they entered both a user and a password
	if( trap_Argc() < 3 ){
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"Usage: register <user> <password>\n\"");
		return;
	}
	
	//Get the user and pass
	trap_Argv( 1, userName, MAX_STRING_CHARS );
	std::string userNameSTR = userName;
	trap_Argv( 2, userPassword, MAX_STRING_CHARS );
	

	//Check if that user exists already
	Query q(db);
	std::transform(userNameSTR.begin(), userNameSTR.end(),userNameSTR.begin(),::tolower);
	std::string DBname = q.get_string(va("SELECT name FROM users WHERE name='%s'",userNameSTR.c_str()));
	if(!DBname.empty())
	{
		trap_SendServerCommand ( targetplayer->client->ps.clientNum, va("print \"^1User %s has already been used\n\"",DBname.c_str()));
		return;
	}
	//Create the account
	q.execute(va("INSERT INTO users(name,password) VALUES('%s','%s')",userNameSTR.c_str(),userPassword));
	//Tell the user the account has been created
	trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Success: ^7Account created. You may now login\n\"");
	
	
	return;
}

//====Character Functions====//

/*
=================

Cmd_AddFeat_F

Ingame Command : addFeat
Grants a level up in a skill

=================
*/
/*
void Cmd_AddFeat_F(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);
	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	//Make sure they entered a feat
	if( trap_Argc() < 2 ){
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"Usage: addFeat <featname>\n\"");
		return;
	}

	//Make sure they're logged in
	if(!isLoggedIn(targetplayer))
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You must be logged in to add skills\n\"");
		return;
	}
	//Make sure they have a character selected
	if(!isInCharacter(targetplayer))
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You have a character chosen to add skills\n\"");
		return;
	}
	//Get the feat name
	char featName[MAX_STRING_CHARS];
	trap_Argv( 1, featName, MAX_STRING_CHARS );
	for(int i = 0; i < NUM_FEATS; i++)
	{
		if(strcmp(featName,FeatTable[i].featname) == 0)
		{
			//Already has this feat
			if(HasFeat(targetplayer->client->sess.characterID,FeatTable[i].ID))
			{
				trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You already have this feat\n\"");
				return;
			}
			//Has the required feat
			else if(!HasFeat(targetplayer->client->sess.characterID,FeatTable[i].requiredFeat))
			{
				trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You don't have the required feat\n\"");
				return;
			}
			trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Success: ^7You have added the feat\n\"");
			AddFeat(targetplayer->client->sess.characterID,FeatTable[i].ID,FeatTable[i].skillLevel);
			return;
		}
	}
	trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7Invalid feat\n\"");
	return;

}
*/
/*
=================

Cmd_ListCharacters_F

Ingame Command : myCharacters
List all of the characters of an account

=================
*/
void Cmd_ListCharacters_F(gentity_t * targetplayer)
{
	StderrLog log;
	Database db(DATABASE_PATH, &log);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	//Make sure they're logged in
	if(!isLoggedIn(targetplayer))
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You must be logged in to ^3list ^7characters\n\"");
		return;
	}
	Query q(db);
	q.get_result(va("SELECT ID, name FROM characters WHERE userID='%i'",targetplayer->client->sess.userID));
	trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Characters:\n\"");
	while (q.fetch_row())
	{
		int ID = q.getval();
		std::string name = q.getstr();
		trap_SendServerCommand( targetplayer->client->ps.clientNum, va("print \"^3ID: ^7%i ^3Name: ^7%s\n\"",ID,name.c_str()));
	}
	q.free_result();
	return;
}

/*
=================

Cmd_CreateCharacter_F

Ingame Command : createCharacter <name>
Creates a new character and binds it to a useraccount

=================
*/
void Cmd_CreateCharacter_F(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	//Make sure they're logged in
	if(!isLoggedIn(targetplayer))
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You must be logged in to ^3create ^7a character\n\"");
		return;
	}

	//Make sure they entered a character
	if( trap_Argc() < 2 ){
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"Usage: createCharacter <name>\n\"");
		return;
	}

	//Get the character name
	char charName[MAX_STRING_CHARS];
	trap_Argv( 1, charName, MAX_STRING_CHARS );
	std::string charNameSTR = charName;

	//Check if the character exists
	Query q(db);
	std::transform(charNameSTR.begin(), charNameSTR.end(),charNameSTR.begin(),::tolower);
	std::string DBname = q.get_string(va("SELECT name FROM characters WHERE userID='%i' AND name='%s'",targetplayer->client->sess.userID,charNameSTR.c_str()));
	/* No longer needed
	if(!DBname.empty())
	{
		trap_SendServerCommand ( targetplayer->client->ps.clientNum, va("print \"^1Character %s has already been used\n\"",DBname.c_str()));
		return;
	}*/
	//Create character
	q.execute(va("INSERT INTO characters(userID,name) VALUES('%i','%s')",targetplayer->client->sess.userID,charNameSTR.c_str()));
	trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Success: ^7Character created. You may select this character\n\"");
	return;
}

/*
=================

Cmd_SelectCharacter_F

Ingame Command : character <name>
Loads the character data and executes the keys effects

=================
*/
void Cmd_SelectCharacter_F(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}

	//Make sure they're logged in
	if(!isLoggedIn(targetplayer))
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You must be logged in to select a character\n\"");
		return;
	}

	//Make sure they entered a character
	if( trap_Argc() < 2 ){
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"Usage: character <name>\n\"");
		return;
	}

	//Get the character name
	char charName[MAX_STRING_CHARS];
	trap_Argv( 1, charName, MAX_STRING_CHARS );
	std::string charNameSTR = charName;

	//Check if the character exists
	Query q(db);
	std::transform(charNameSTR.begin(), charNameSTR.end(),charNameSTR.begin(),::tolower);
	int charID = q.get_num(va("SELECT ID FROM characters WHERE userID='%i' AND name='%s'",targetplayer->client->sess.userID,charNameSTR.c_str()));
	if(charID == 0)
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: Character does not exist\n\"");
		return;
	}
	
	//Update that we have a character selected
	targetplayer->client->sess.characterChosen = qtrue;
	targetplayer->client->sess.characterID = charID;
	SetTeam(targetplayer,"f");
	LoadCharacter(targetplayer);
	trap_SendServerCommand( targetplayer->client->ps.clientNum, va("print \"^1Success: ^7Your character is selected as: %s!\n\"",charName));
	targetplayer->flags &= ~FL_GODMODE;
	targetplayer->client->ps.stats[STAT_HEALTH] = targetplayer->health = -999;
	player_die (targetplayer, targetplayer, targetplayer, 100000, MOD_SUICIDE);

	return;
}

/*
=================

LoadCharacter

Loads the character data

=================
*/
void DetermineDodgeMax(gentity_t *ent);
void LoadCharacter(gentity_t * targetplayer)
{
	LoadSkills(targetplayer);
	LoadForcePowers(targetplayer);
	LoadFeats(targetplayer);
	LoadAttributes(targetplayer);

	
	//Create new power string
	std::string newForceString;
	newForceString.append(va("%i-%i-",FORCE_MASTERY_JEDI_KNIGHT,FORCE_LIGHTSIDE));
	int i;
	for(i = 0; i < NUM_FORCE_POWERS; i++)
	{
	char tempForce[2];
    itoa(targetplayer->client->ps.fd.forcePowerLevel[i],tempForce,10);
	newForceString.append(tempForce);
	}
	for(i = 0; i < NUM_SKILLS; i++)
	{
	char tempSkill[2];
    itoa(targetplayer->client->skillLevel[i],tempSkill,10);
	newForceString.append(tempSkill);
	}
	for(i = 0; i < NUM_FORCE_POWERS; i++)
	{
	char tempFeat[2];
    itoa(targetplayer->client->featLevel[i],tempFeat,10);
	newForceString.append(tempFeat);
	}
	trap_SendServerCommand( targetplayer->client->ps.clientNum, va("forcechanged x %s\n", newForceString.c_str()));
	
	DetermineDodgeMax(targetplayer);
	return;
}
/*
=================

LoadFeats

Loads the character feats

=================
*/
void LoadFeats(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	Query q(db);
	std::string feats = q.get_string(va("SELECT feats FROM characters WHERE ID='%i'",targetplayer->client->sess.characterID));
	int size = (feats.size() < NUM_FEATS) ? feats.size() : NUM_FEATS;
	for(int i = 0; i < size; i++)
	{
		char temp = feats[i];
		int level = temp - '0';
		targetplayer->client->featLevel[i] = level;
	}
	return;
}


/*
=================

LoadSkills

Loads the character skills

=================
*/
void LoadSkills(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	Query q(db);
	std::string skills = q.get_string(va("SELECT skills FROM characters WHERE ID='%i'",targetplayer->client->sess.characterID));
	int size = (skills.size() < NUM_SKILLS) ? skills.size() : NUM_SKILLS;
	for(int i = 0; i < size; i++)
	{
		char temp = skills[i];
		int level = temp - '0';
		targetplayer->client->skillLevel[i] = level;
	}
	return;
}


/*
=================

LoadForcePowers

Loads the character force powers

=================
*/
void LoadForcePowers(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	Query q(db);
	std::string powers = q.get_string(va("SELECT force FROM characters WHERE ID='%i'",targetplayer->client->sess.characterID));
	int size = (powers.size() < NUM_FORCE_POWERS) ? powers.size() : NUM_FORCE_POWERS;
	for(int i = 0; i < size; i++)
	{
		char temp = powers[i];
		int level = temp - '0';
		targetplayer->client->ps.fd.forcePowerLevel[i] = level;
		if(level > 0)
			targetplayer->client->ps.fd.forcePowersKnown |= (1 << i);
	}
	return;
}
/*
=================

GetForceLevel

Takes the level stored in the database and returns force power value

=================
*/
int GetForceLevel(int level)
{
	switch(level)
	{
		case 1:
			return FORCE_LEVEL_1;
		case 2:
			return FORCE_LEVEL_2;
		case 3:
			return FORCE_LEVEL_3;
		default:
			return FORCE_LEVEL_0;
	}
}


/*
=================

AddFeat

Processes the feat into database and adds the approriate skills/levels
=====
*/
/*
void AddFeat(int charID, int featID, int level)
{
	Database db(DATABASE_PATH);
	Query q(db);
	switch(featID)
	{
	case FT_BLASTERS1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_PISTOL);
			GrantSkill(charID,SK_BLASTER);
			return;
		}
	case FT_BLASTERS2:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_PISTOL,level);
			UpdateSkill(charID,SK_BLASTER,level);
			return;
		}
	case FT_BLASTERS3:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_PISTOL,level);
			UpdateSkill(charID,SK_BLASTER,level);
			return;
		}
	case FT_RIFLES1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_DISRUPTOR);
			GrantSkill(charID,SK_TUSKEN_RIFLE);
			return;
		}
	case FT_RIFLES2:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_DISRUPTOR,level);
			UpdateSkill(charID,SK_TUSKEN_RIFLE,level);
			return;
		}
	case FT_RIFLES3:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_DISRUPTOR,level);
			UpdateSkill(charID,SK_TUSKEN_RIFLE,level);
			return;
		}
	case FT_BLASTERUPGRADE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_BLASTERRATEOFFIREUPGRADE);
			return;
		}
	case FT_HEAVYWP1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_REPEATER);
			GrantSkill(charID,SK_FLECHETTE);
			return;
		}
	case FT_HEAVYWP2:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_REPEATER,level);
			UpdateSkill(charID,SK_FLECHETTE,level);
			return;
		}
	case FT_HEAVYWP3:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_REPEATER,level);
			UpdateSkill(charID,SK_FLECHETTE,level);
			return;
		}
	case FT_REPEATERUPGRADE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_REPEATERUPGRADE);
			return;
		}
	case FT_SPEC1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_BOWCASTER);
			return;
		}
	case FT_SPEC2:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_BOWCASTER,level);
			return;
		}
	case FT_SPEC3:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_BOWCASTER,level);
			return;
		}
	case FT_DEMO1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_DETPACK);
			GrantSkill(charID,SK_ROCKET);
			return;
		}
	case FT_EXPLODE1: //Needs work
		{
			//InsertFeat(charID,featID);
			return;
		}
	case FT_BACTA1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_BACTA);
			return;
		}
	case FT_BACTA2:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_BACTA,level);
			return;
		}
	case FT_FLAMETHROWER1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_FLAMETHROWER);
			return;
		}
	case FT_JETPACK1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_JETPACK);
			return;
		}
	case FT_CLOAK1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_CLOAK);
			return;
		}
	case FT_SHIELD1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_SHIELD);
			return;
		}
	case FT_SHIELD2:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_SHIELD,level);
			return;
		}
	case FT_SHIELD3:
		{
			InsertFeat(charID,featID);
			UpdateSkill(charID,SK_SHIELD,level);
			return;
		}
	case FT_ENGINEER1:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_FORCEFIELD);
			GrantSkill(charID,SK_SEEKER);
			GrantSkill(charID,SK_SENTRY);
			return;
		}//Force powers
	case FT_TELEKINESIS1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_PUSH);
			GrantFP(charID,FP_PULL);
			GrantFP(charID,FP_LIFT);
			return;
		}
	case FT_TELEKINESIS2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_PUSH,level);
			UpdateFP(charID,FP_PULL,level);
			UpdateFP(charID,FP_LIFT,level);
			return;
		}
	case FT_TELEKINESIS3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_PUSH,level);
			UpdateFP(charID,FP_PULL,level);
			UpdateFP(charID,FP_LIFT,level);
			return;
		}
	case FT_LEVITATION1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_LEVITATION);
			return;
		}
	case FT_LEVITATION2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_LEVITATION,level);
			return;
		}
	case FT_LEVITATION3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_LEVITATION,level);
			return;
		}
	case FT_HEAL1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_HEAL);
			return;
		}
	case FT_HEAL2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_HEAL,level);
			return;
		}
	case FT_HEAL3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_HEAL,level);
			return;
		}
	case FT_TEAM_HEAL1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_TEAM_HEAL);
			return;
		}
	case FT_TEAM_HEAL2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_TEAM_HEAL,level);
			return;
		}
	case FT_TEAM_HEAL3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_TEAM_HEAL,level);
			return;
		}
	case FT_MINDTRICK1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_MINDTRICK);
			return;
		}
	case FT_MINDTRICK2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_MINDTRICK,level);
			return;
		}
	case FT_MINDTRICK3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_MINDTRICK,level);
			return;
		}
	case FT_GRIP1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_GRIP);
			return;
		}
	case FT_GRIP2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_GRIP,level);
			return;
		}
	case FT_GRIP3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_GRIP,level);
			return;
		}
	case FT_LIGHTNING1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_LIGHTNING);
			return;
		}
	case FT_LIGHTNING2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_LIGHTNING,level);
			return;
		}
	case FT_LIGHTNING3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_LIGHTNING,level);
			return;
		}
	case FT_RAGE1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_RAGE);
			return;
		}
	case FT_RAGE2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_RAGE,level);
			return;
		}
	case FT_RAGE3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_RAGE,level);
			return;
		}
	case FT_MANIPULATE1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_MANIPULATE);
			return;
		}
	case FT_MANIPULATE2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_MANIPULATE,level);
			return;
		}
	case FT_MANIPULATE3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_MANIPULATE,level);
			return;
		}
	case FT_ABSORB1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_ABSORB);
			return;
		}
	case FT_ABSORB2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_ABSORB,level);
			return;
		}
	case FT_ABSORB3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_ABSORB,level);
			return;
		}
	case FT_SPEED1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_SPEED);
			return;
		}
	case FT_SPEED2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SPEED,level);
			return;
		}
	case FT_SPEED3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SPEED,level);
			return;
		}
	case FT_SEE1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_SEE);
			return;
		}
	case FT_SEE2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SEE,level);
			return;
		}
	case FT_SEE3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SEE,level);
			return;
		}
	case FT_SABER_OFFENSE1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_SABER_OFFENSE);
			return;
		}
	case FT_SABER_OFFENSE2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SABER_OFFENSE,level);
			return;
		}
	case FT_SABER_OFFENSE3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SABER_OFFENSE,level);
			return;
		}
	case FT_SABER_DEFENSE1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_SABER_DEFENSE);
			return;
		}
	case FT_SABER_DEFENSE2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SABER_DEFENSE,level);
			return;
		}
	case FT_SABER_DEFENSE3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SABER_DEFENSE,level);
			return;
		}
	case FT_SABERTHROW1:
		{
			InsertFeat(charID,featID);
			GrantFP(charID,FP_SABERTHROW);
			return;
		}
	case FT_SABERTHROW2:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SABERTHROW,level);
			return;
		}
	case FT_SABERTHROW3:
		{
			InsertFeat(charID,featID);
			UpdateFP(charID,FP_SABERTHROW,level);
			return;
		}
	case FT_BLUESTYLE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_BLUESTYLE);
			return;
		}
	case FT_REDSTYLE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_REDSTYLE);
			return;
		}
	case FT_PURPLESTYLE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_PURPLESTYLE);
			return;
		}
	case FT_GREENSTYLE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_GREENSTYLE);
			return;
		}
	case FT_DUALSTYLE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_DUALSTYLE);
			return;
		}
	case FT_STAFFSTYLE:
		{
			InsertFeat(charID,featID);
			GrantSkill(charID,SK_STAFFSTYLE);
			return;
		}
	}
	return;
}
*/
/*
=================

GrantSkill

Grants the appropriate skill with default level to the player
=====
*/
void GrantSkill(int charID, int skill)
{
	Database db(DATABASE_PATH);
	Query q(db);

	q.execute(va("INSERT INTO skills(charID,skill,level) VALUES('%i','%i','%i')",charID,skill,1));
	return;
}

/*
=================

UpdateSkill

Updates the appropriate skill with new level
=====
*/
void UpdateSkill(int charid, int skill, int level)
{
	Database db(DATABASE_PATH);
	Query q(db);
	
	int skillID = q.get_num(va("SELECT ID FROM skills WHERE charID='%i' AND skill='%i'",charid,skill));
	q.execute(va("UPDATE skills set level='%i' WHERE ID='%i'",level,skillID));
	return;
}
/*
=================

GrantFP

Grants the appropriate forcepower with default level to the player
=====
*/
void GrantFP(int charID, int forcepower)
{
	Database db(DATABASE_PATH);
	Query q(db);

	q.execute(va("INSERT INTO forcepowers(charID,forcepower,level) VALUES('%i','%i','%i')",charID,forcepower,1));
	return;
}

/*
=================

UpdateFP

Updates the appropriate forcepower with new level
=====
*/
void UpdateFP(int charid, int forcepower, int level)
{
	Database db(DATABASE_PATH);
	Query q(db);
	
	int fpID = q.get_num(va("SELECT ID FROM forcepowers WHERE charID='%i' AND forcepower='%i'",charid,forcepower));
	q.execute(va("UPDATE forcepowers set level='%i' WHERE ID='%i'",level,fpID));
	return;
}
/*
=================

HasForcePower

Checks if the character has this forcepower
=====
*/
qboolean HasForcePower(int charid, int power)
{
	Database db(DATABASE_PATH);
	Query q(db);

	q.get_result(va("SELECT * FROM forcepower WHERE charID='%i' AND forcepower='%i'",charid,power));
	int forcepower = q.num_rows();
	if(forcepower)
	{
		return qtrue;
	}
	else
	{
		return qfalse;
	}
}
/*
=================

HasSkill

Checks if the character has this skill
=====
*/
qboolean HasSkill(int charid, int skill)
{
	Database db(DATABASE_PATH);
	Query q(db);

	q.get_result(va("SELECT * FROM skills WHERE charID='%i' AND skill='%i'",charid,skill));
	int valid = q.num_rows();
	if(valid)
	{
		return qtrue;
	}
	else
	{
		return qfalse;
	}
}
/*
=================

HasFeat

Checks if the character has this feat
=====
*/
qboolean HasFeat(int charid, int featID)
{
	Database db(DATABASE_PATH);
	Query q(db);
	/*if(featID == FT_NONE)
	{
		return qtrue;
	}*/
	q.get_result(va("SELECT * FROM feats WHERE charID='%i' AND featID='%i'",charid,featID));
	int feat = q.num_rows();
	if(feat)
	{
		return qtrue;
	}
	else
	{
		return qfalse;
	}
}
/*
=================

InsertFeat

Inserts feat into the database
=====
*/
void InsertFeat(int charID,int featID)
{
	Database db(DATABASE_PATH);
	Query q(db);
	q.execute(va("INSERT INTO feats(charID,featID) VALUES('%i','%i')",charID,featID));
	return;
}
/*
=================

Cmd_ListFeats_F

Lists all the feats available

Ingame command: feats
=====
*/
/*
void Cmd_ListFeats_F(gentity_t * targetplayer)
{
	
	//Make sure they're logged in
	if(!isLoggedIn(targetplayer) || !isInCharacter(targetplayer))
	{
		trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^1Error: ^7You must be logged in and have a character chosen to list feats\n\"");
		return;
	}
	std::string featOutput;
	std::string newLine = "\n";
	trap_SendServerCommand( targetplayer->client->ps.clientNum, "print \"^3Available Feats\n\"");
	for(int i = 0; i < NUM_FEATS; i++)
	{
		if(FeatTable[i].requiredFeat == FT_NONE)
		{
			featOutput.append(newLine);
		}
			featOutput.append(va("^7%s>",FeatTable[i].featname));
	}
	trap_SendServerCommand( targetplayer->client->ps.clientNum, va("print \"%s\n\"",featOutput.c_str()));


	return;
}*/
/*
=================

Cmd_Grenade_F

Gives you the chosen grenade

Ingame command: grenade <emp cryoban smoke>
=====
*/
void Cmd_Grenade_F(gentity_t * targetplayer)
{
	
	return;
}
/*
=================

Cmd_CharacterInfo_F

Spits out the character information

Ingame command: characterInfo
=====
*/
void Cmd_CharacterInfo_F(gentity_t * targetplayer)
{
	return;
}


//Returns whether we're logged in or not
qboolean isLoggedIn(gentity_t* targetplayer){
	if(targetplayer->client->sess.loggedinAccount)
		return qtrue;
	else
		return qfalse;
}
//Returns whether we're in character or not
qboolean isInCharacter(gentity_t* targetplayer){
	if(targetplayer->client->sess.characterChosen)
		return qtrue;
	else
		return qfalse;
}

/*
=================

LoadUser

Loads the user properties

=====
*/
void LoadUser(gentity_t * targetplayer){
	Database db(DATABASE_PATH);
	Query q(db);
	
	int userID = targetplayer->client->sess.userID;

	//Checks if the user is admin
	int isAdmin = q.get_num(va("SELECT admin FROM users WHERE ID='%i'",userID));
	if(isAdmin)
	{//We are give them admin
		targetplayer->client->sess.loggedinadmin = qtrue;
	}
	return;
}


/*
=================

SaveCharacter

Saves the character information to the database

=====
*/
void SaveCharacter(gentity_t * targetplayer) 
{
        Database db(DATABASE_PATH);
        Query q(db);
		std::string featString;
		std::string skillString;
		std::string forceString;

        //Create feat string
        for(int i = 0; i < NUM_FEATS; i++)
        {
		 char tempFeat[2];
		 itoa(targetplayer->client->featLevel[i],tempFeat,10);
		 featString.append(tempFeat);
        }
        //Create skill string
        for(int j = 0; j < NUM_SKILLS; j++)
        {
		 char tempSkill[2];
         itoa(targetplayer->client->skillLevel[j],tempSkill,10);
		 skillString.append(tempSkill);
        }
        //Create force string
        for(int k = 0; k < NUM_FORCE_POWERS-1; k++)
        {
		 char tempForce[2];
         itoa(targetplayer->client->ps.fd.forcePowerLevel[k],tempForce,10);
		 forceString.append(tempForce);
        }
      
      //Update feats in database
	  q.execute(va("UPDATE characters set feats='%s' WHERE ID='%i'",featString.c_str(),targetplayer->client->sess.characterID));
      //Update skills in database
      q.execute(va("UPDATE characters set skills='%s' WHERE ID='%i'",skillString.c_str(),targetplayer->client->sess.characterID));
      //Update force in database
      q.execute(va("UPDATE characters set force='%s' WHERE ID='%i'",forceString.c_str(),targetplayer->client->sess.characterID));
	  SaveAttributes(targetplayer);

        return;
}

void Cmd_GrantAdmin_F( gentity_t * ent )
{
	Database db(DATABASE_PATH);
	Query q(db);
	char accountName[MAX_TOKEN_CHARS];

	if(ent->client->sess.loggedinadmin == qfalse)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"You are not allowed to use this command.\n\"");
		return;
	}
	if( trap_Argc() < 2 ){
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"Usage: GrantAdmin <accountname>\n\"");
		return;
	}
	trap_Argv( 1, accountName, MAX_STRING_CHARS );
	
	int valid = q.get_num(va("SELECT * FROM users WHERE name='%s'",accountName));
	if(!valid)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"This user does not exist\n\"");
		return;
	}

	q.execute(va("UPDATE users set admin='1' WHERE name='%s'",accountName));

	trap_SendServerCommand( ent->client->ps.clientNum, "print \"Admin granted\n\"");
	return;
}

void Cmd_SVGrantAdmin_F()
{
	Database db(DATABASE_PATH);
	Query q(db);
	char accountName[MAX_TOKEN_CHARS];

	if( trap_Argc() < 2 ){
		G_Printf("Usage: GrantAdmin <accountname>\n");
		return;
	}
	trap_Argv( 1, accountName, MAX_STRING_CHARS );
	
	int valid = q.get_num(va("SELECT * FROM users WHERE name='%s'",accountName));
	if(!valid)
	{
		G_Printf("This user does not exist\n");
		return;
	}

	q.execute(va("UPDATE users set admin='1' WHERE name='%s'",accountName));

	G_Printf("Admin granted\n");
	return;
}

void Cmd_RemoveAdmin_F( gentity_t * ent )
{
	Database db(DATABASE_PATH);
	Query q(db);
	char accountName[MAX_TOKEN_CHARS];

	if(ent->client->sess.loggedinadmin == qfalse)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"You are not allowed to use this command.\n\"");
		return;
	}
	if( trap_Argc() < 2 ){
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"Usage: RemoveAdmin <accountname>\n\"");
		return;
	}
	trap_Argv( 1, accountName, MAX_STRING_CHARS );
	
	int valid = q.get_num(va("SELECT * FROM users WHERE name='%s'",accountName));
	if(!valid)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, "print \"This user does not exist\n\"");
		return;
	}

	q.execute(va("UPDATE users set admin='0' WHERE name='%s'",accountName));

	trap_SendServerCommand( ent->client->ps.clientNum, "print \"Admin removed\n\"");
	return;
}

void Cmd_SVRemoveAdmin_F()
{
	Database db(DATABASE_PATH);
	Query q(db);
	char accountName[MAX_TOKEN_CHARS];

	if( trap_Argc() < 2 ){
		G_Printf("Usage: RemoveAdmin <accountname>\n");
		return;
	}
	trap_Argv( 1, accountName, MAX_STRING_CHARS );
	
	int valid = q.get_num(va("SELECT * FROM users WHERE name='%s'",accountName));
	if(!valid)
	{
		G_Printf("This user does not exist\n");
		return;
	}

	q.execute(va("UPDATE users set admin='0' WHERE name='%s'",accountName));

	G_Printf("Admin removed\n");
	return;
}
void LoadAttributes(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	Query q(db);
	
	char userinfo[MAX_INFO_STRING];
	trap_GetUserinfo( targetplayer->client->ps.clientNum, userinfo, MAX_INFO_STRING );


	//Name
	std::string name = q.get_string(va("SELECT igname FROM characters WHERE ID='%i'",targetplayer->client->sess.characterID));
	Info_SetValueForKey( userinfo, "name", name.c_str() );
    //Model
	std::string model = q.get_string(va("SELECT model FROM characters WHERE ID='%i'",targetplayer->client->sess.characterID));
	Info_SetValueForKey( userinfo, "model", model.c_str() );
	trap_SetUserinfo( targetplayer->client->ps.clientNum, userinfo );
	ClientUserinfoChanged( targetplayer->client->ps.clientNum );

	//Model scale
	int modelScale = q.get_num(va("SELECT modelscale FROM characters WHERE ID='%i'",targetplayer->client->sess.characterID));
	targetplayer->client->ps.iModelScale= modelScale;
	targetplayer->client->sess.modelScale= modelScale;
	return;
}

void SaveAttributes(gentity_t * targetplayer)
{
	Database db(DATABASE_PATH);

	if (!db.Connected())
	{
		G_Printf("Database not connected, %s\n",DATABASE_PATH);
		return;
	}
	Query q(db);
	
	char userinfo[MAX_INFO_STRING];
	trap_GetUserinfo( targetplayer->client->ps.clientNum, userinfo, MAX_INFO_STRING );


	//Name
	std::string name = Info_ValueForKey( userinfo, "name");
	q.execute(va("UPDATE characters set igname='%s' WHERE ID='%i'",name.c_str(),targetplayer->client->sess.characterID));
    //Model
	std::string model = Info_ValueForKey( userinfo, "model");
	q.execute(va("UPDATE characters set model='%s' WHERE ID='%i'",model.c_str(),targetplayer->client->sess.characterID));
	//Model scale
	q.execute(va("UPDATE characters set modelscale='%i' WHERE ID='%i'",targetplayer->client->sess.modelScale,targetplayer->client->sess.characterID));
	return;
}