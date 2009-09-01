// Copyright (C) 1999-2000 Id Software, Inc.
//

// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"
#include "g_admin.h"
#include "g_adminshared.h"

/*
==============================================================================

PACKET FILTERING
 

You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and any unspecified digits will match any value, so you can specify an entire class C network with "addip 192.246.40".

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

g_filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.


==============================================================================
*/

// extern	vmCvar_t	g_banIPs;
// extern	vmCvar_t	g_filterBan;


typedef struct ipFilter_s
{
	unsigned	mask;
	unsigned	compare;
} ipFilter_t;

// VVFIXME - We don't need this at all, but this is the quick way.
#define	MAX_IPFILTERS	1024

static ipFilter_t	ipFilters[MAX_IPFILTERS];
static int			numIPFilters;

/*
=================
StringToFilter
=================
*/
static qboolean StringToFilter (char *s, ipFilter_t *f)
{
	char	num[128];
	int		i, j;
	byte	b[4];
	byte	m[4];
	
	for (i=0 ; i<4 ; i++)
	{
		b[i] = 0;
		m[i] = 0;
	}
	
	for (i=0 ; i<4 ; i++)
	{
		if (*s < '0' || *s > '9')
		{
			G_Printf( "Bad filter address: %s\n", s );
			return qfalse;
		}
		
		j = 0;
		while (*s >= '0' && *s <= '9')
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi(num);
		if (b[i] != 0)
			m[i] = 255;

		if (!*s)
			break;
		s++;
	}
	
	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;
	
	return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
static void UpdateIPBans (void)
{
	byte	b[4];
	int		i;
	char	iplist[MAX_INFO_STRING];

	*iplist = 0;
	for (i = 0 ; i < numIPFilters ; i++)
	{
		if (ipFilters[i].compare == 0xffffffff)
			continue;

		*(unsigned *)b = ipFilters[i].compare;
		Com_sprintf( iplist + strlen(iplist), sizeof(iplist) - strlen(iplist), 
			"%i.%i.%i.%i ", b[0], b[1], b[2], b[3]);
	}

	trap_Cvar_Set( "g_banIPs", iplist );
}

/*
=================
G_FilterPacket
=================
*/
qboolean G_FilterPacket (char *from)
{
	byte			m[4];// = {'\0','\0','\0','\0'};
	int				i = 0;
	unsigned int	in;
	char			*p;

	while (i < 4)
	{
		m[i] = 0;
		i++;
	}

	i = 0;
	p = from;
	while (*p && i < 4) {
		while (*p >= '0' && *p <= '9') {
			m[i] = m[i]*10 + (*p - '0');
			p++;
		}
		if (!*p || *p == ':')
			break;
		i++, p++;
	}
	
	in = *(unsigned int *)m;

	for (i=0 ; i<numIPFilters ; i++)
		if ( (in & ipFilters[i].mask) == ipFilters[i].compare)
			return (qboolean)(g_filterBan.integer != 0);

	return (qboolean)(g_filterBan.integer == 0);
}

/*
=================
AddIP
=================
*/
//[AdminCommands]
void AddIP( char *str )
//[/AdminCommands]
{
	int		i;

	for (i = 0 ; i < numIPFilters ; i++)
		if (ipFilters[i].compare == 0xffffffff)
			break;		// free spot
	if (i == numIPFilters)
	{
		if (numIPFilters == MAX_IPFILTERS)
		{
			G_Printf ("IP filter list is full\n");
			return;
		}
		numIPFilters++;
	}
	
	if (!StringToFilter (str, &ipFilters[i]))
		ipFilters[i].compare = 0xffffffffu;

	UpdateIPBans();
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans(void) 
{
	char *s, *t;
	char		str[MAX_TOKEN_CHARS];

	Q_strncpyz( str, g_banIPs.string, sizeof(str) );

	for (t = s = g_banIPs.string; *t; /* */ ) {
		s = strchr(s, ' ');
		if (!s)
			break;
		while (*s == ' ')
			*s++ = 0;
		if (*t)
			AddIP( t );
		t = s;
	}
}


/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f (void)
{
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf("Usage:  addip <ip-mask>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	AddIP( str );

}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f (void)
{
	ipFilter_t	f;
	int			i;
	char		str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf("Usage:  sv removeip <ip-mask>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	if (!StringToFilter (str, &f))
		return;

	for (i=0 ; i<numIPFilters ; i++) {
		if (ipFilters[i].mask == f.mask	&&
			ipFilters[i].compare == f.compare) {
			ipFilters[i].compare = 0xffffffffu;
			G_Printf ("Removed.\n");

			UpdateIPBans();
			return;
		}
	}

	G_Printf ( "Didn't find %s.\n", str );
}


//[AdminCommands]
void Svcmd_ListIPs_f( void )
{
	int		i;
	char	*str;
	byte	b[4];

	G_Printf ( "%d IP slots used.\n", numIPFilters );
	for ( i = 0 ; i < numIPFilters ; i++ ) 
	{
		G_Printf ( "%d: ", i );
		if (ipFilters[i].compare == 0xffffffff)
		{
			G_Printf ( "unused\n" );
		}
		else
		{
			*(unsigned *)b = ipFilters[i].compare;
			str = va("%i.%i.%i.%i \n", b[0], b[1], b[2], b[3]);
			G_Printf ( "%s\n", str );
		}
	}
}

void G_SaveBanIP( void )
{//save out all the banned IPs
	int		i;
	char	*str;
	fileHandle_t fh;
	byte	b[4];

	trap_FS_FOpenFile("banip.txt", &fh, FS_WRITE);
	if ( !fh )
	{
		G_Printf ( "G_SaveBanIP - ERROR: can't open banip.txt\n" );
		return;
	}
	
	str = va("%d \n", numIPFilters);
	trap_FS_Write(str, strlen(str), fh);
	for ( i = 0 ; i < numIPFilters ; i++ ) 
	{
		if (ipFilters[i].compare == 0xffffffff)
		{
			str = "unused \n";
			trap_FS_Write(str, strlen(str), fh);
		}
		else
		{
			*(unsigned *)b = ipFilters[i].compare;
			str = va("%i.%i.%i.%i \n", b[0], b[1], b[2], b[3]);
			trap_FS_Write(str, strlen(str), fh);
		}
	}

	trap_FS_FCloseFile(fh);
}

void G_LoadIPBans( void )
{//load in all the banned IPs
	int		i, len;
	char	*p, *token;
	fileHandle_t fh;
	char	banIPBuffer[MAX_IPFILTERS*32];			//	The list of file names read in
	char	banIPFile[MAX_QPATH];

	len = trap_FS_FOpenFile("banip.txt", &fh, FS_READ);
	if ( !fh )
	{
		G_Printf ( "G_LoadBanIP - ERROR: can't open banip.txt\n" );
		return;
	}
	
	trap_FS_Read(banIPBuffer, len, fh);
	banIPBuffer[len] = 0;
	trap_FS_FCloseFile(fh);
	p = banIPBuffer;
	COM_BeginParseSession(banIPFile);

	//had to change this to compile linux
	token = COM_ParseExt( (const char **) &p, qtrue );
	if ( token )
	{
		numIPFilters = atoi(token);

		for ( i = 0 ; i < numIPFilters ; i++ ) 
		{
			//had to change this to compile linux
			token = COM_ParseExt( (const char **) &p, qtrue );
			if ( token )
			{//have an ip
				if ( !Q_stricmp("unused",token) )
				{
					ipFilters[i].compare = 0xffffffffu;
				}
				else
				{
					StringToFilter(token,&ipFilters[i]);
				}
			}
			else
			{
				break;
			}
		}
	}
}
//[/AdminCommands]


/*
===================
Svcmd_EntityList_f
===================
*/
void	Svcmd_EntityList_f (void) {
	int			e;
	gentity_t		*check;

	check = g_entities+1;
	for (e = 1; e < level.num_entities ; e++, check++) {
		if ( !check->inuse ) {
			continue;
		}
		G_Printf("%3i:", e);
		switch ( check->s.eType ) {
		case ET_GENERAL:
			G_Printf("ET_GENERAL          ");
			break;
		case ET_PLAYER:
			G_Printf("ET_PLAYER           ");
			break;
		case ET_ITEM:
			G_Printf("ET_ITEM             ");
			break;
		case ET_MISSILE:
			G_Printf("ET_MISSILE          ");
			break;
		case ET_MOVER:
			G_Printf("ET_MOVER            ");
			break;
		case ET_BEAM:
			G_Printf("ET_BEAM             ");
			break;
		case ET_PORTAL:
			G_Printf("ET_PORTAL           ");
			break;
		case ET_SPEAKER:
			G_Printf("ET_SPEAKER          ");
			break;
		case ET_PUSH_TRIGGER:
			G_Printf("ET_PUSH_TRIGGER     ");
			break;
		case ET_TELEPORT_TRIGGER:
			G_Printf("ET_TELEPORT_TRIGGER ");
			break;
		case ET_INVISIBLE:
			G_Printf("ET_INVISIBLE        ");
			break;
		case ET_NPC:
			G_Printf("ET_NPC              ");
			break;
		default:
			G_Printf("%3i                 ", check->s.eType);
			break;
		}

		if ( check->classname ) {
			G_Printf("%s", check->classname);
		}
		G_Printf("\n");
	}
}

gclient_t	*ClientForString( const char *s ) {
	gclient_t	*cl;
	int			i;
	int			idnum;

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			Com_Printf( "Bad client slot: %i\n", idnum );
			return NULL;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			G_Printf( "Client %i is not connected\n", idnum );
			return NULL;
		}
		return cl;
	}

	// check for a name match
	for ( i=0 ; i < level.maxclients ; i++ ) {
		cl = &level.clients[i];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( !Q_stricmp( cl->pers.netname, s ) ) {
			return cl;
		}
	}

	G_Printf( "User %s is not on the server\n", s );

	return NULL;
}

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void	Svcmd_ForceTeam_f( void ) {
	gclient_t	*cl;
	char		str[MAX_TOKEN_CHARS];

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );
	if ( !cl ) {
		return;
	}

	// set the team
	trap_Argv( 2, str, sizeof( str ) );
	SetTeam( &g_entities[cl - level.clients], str );
}

char	*ConcatArgs( int start );

/*
=================
ConsoleCommand

=================
*/
//[AdminSys]
qboolean OJP_AllPlayersHaveClientPlugin(void);
//[/AdminSys]
qboolean	ConsoleCommand( void ) {
	char	cmd[MAX_TOKEN_CHARS];

	trap_Argv( 0, cmd, sizeof( cmd ) );

	// MJN - Info
	if ( Q_stricmp (cmd, "minfo") == 0 ){
		M_Svcmd_Info_f();
		return qtrue;
	}
	// MJN - Kick
	if ( Q_stricmp (cmd, "mkick") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Kick_f, "Usage: mkick <playerid> <reason>\n", 2,
			NULL, NULL, NULL);
		return qtrue;
	}
	// MJN - Kick Ban
	if ( Q_stricmp (cmd, "mkickban") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_KickBan_f, "Usage: mkickban <playerid> <reason>\n", 3,
			NULL, NULL, NULL);
		return qtrue;
	}
	// MJN - Ban Mask
	if ( Q_stricmp (cmd, "mbanrange") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_BanRange_f, "Usage: mbanrange <playerid> <security> <reason>\n", 4,
			NULL, NULL, NULL);
		return qtrue;
	}
	// MJN - Silence
	if ( Q_stricmp (cmd, "msilence") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Silence_f, "Usage: msilence <playerid> <mode>\n", 3,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Silence
	if ( Q_stricmp (cmd, "munsilence") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_UnSilence_f, "Usage: munsilence <playerid> <mode>\n", 3,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Empower
	if ( Q_stricmp (cmd, "mempower") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Empower_f, "Usage: mempower <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - UnEmpower
	if ( Q_stricmp (cmd, "munempower") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_UnEmpower_f, "Usage: munempower <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Terminator
	if ( Q_stricmp (cmd, "mterminator") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Terminator_f, "Usage: mterminator <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - UnTerminator
	if ( Q_stricmp (cmd, "munterminator") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_UnTerminator_f, "Usage: munterminator <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Protect
	if ( Q_stricmp (cmd, "mprotect") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Protect_f, "Usage: mprotect <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - UnProtect
	if ( Q_stricmp (cmd, "munprotect") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_UnProtect_f, "Usage: munprotect <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - No Target
	if ( Q_stricmp (cmd, "mnotarget") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_NoTarget_f, "Usage: mnotarget <playerid>\n", 2,
			NULL, NULL, NULL);
		return qtrue;
	}
	// MJN - Sleep
	if ( Q_stricmp (cmd, "msleep") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Sleep_f, "Usage: msleep <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Wake
	if ( Q_stricmp (cmd, "mwake") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Wake_f, "Usage: mwake <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Rename
	if ( Q_stricmp (cmd, "mrename") == 0 ){
		M_Svcmd_Rename_f();
		return qtrue;
	}
	// MJN - Allows access for admin commands
	if ( Q_stricmp (cmd, "madminaccess") == 0 ) {
		M_Svcmd_AdminAccess_f();
		return qtrue;
	}
	// MJN - Cancel access to admin commands
	if ( Q_stricmp (cmd, "mdenyadminaccess") == 0 ) {
		M_Svcmd_UnAdminAccess_f();
		return qtrue;
	}
	// MJN - Allows access to NPC's
	if (Q_stricmp (cmd, "mnpcaccess") == 0) {
		M_Svcmd_CheatAccess_f();
		return qtrue;
	}
	// MJN - Admin Origin
	if (Q_stricmp (cmd, "morigin") == 0 ){
		ExecCommandOnPlayers( M_Svcmd_Origin_f, "Usage: morigin <playerid>\n", 2,
			NULL, NULL, NULL);
		return qtrue;
	}
	// MJN - Deny Vote
	if (Q_stricmp (cmd, "mdenyvote") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_VoteDeny_f, "Usage: mdenyvote <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Allow Vote
	if (Q_stricmp (cmd, "mallowvote") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_VoteAllow_f, "Usage: mallowvote <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	
	// MJN - Teleporting
	if ( Q_stricmp (cmd, "mtele") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Teleport_f, "Usage: mtele <playerid> <x> <y> <z> <yaw>\n", 3, 
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Changes map
	if ( Q_stricmp (cmd, "mgametype") == 0 ){
		M_Svcmd_ChangeMap_f();
		return qtrue;
	}
	// MJN - Changes next map in rotation
	if ( Q_stricmp (cmd, "mnextmap") == 0 ){
		M_Svcmd_NextMap_f();
		return qtrue;
	}
	// MJN - Select which map in rotation
	if ( Q_stricmp (cmd, "mvstr") == 0 ){
		M_Svcmd_Vstr_f();
		return qtrue;
	}
	// MJN - Random Teams
	if (Q_stricmp (cmd, "mrandteams") == 0 ){
		M_Svcmd_RandTeams_f();
		return qtrue;
	}
	// MJN - Private Say
	if ( Q_stricmp (cmd, "mpsay") == 0 ){
		ExecCommandOnPlayers ( M_Svcmd_PSay_f, "Usage: mpsay <playerid> <message>\nYou can use \\n for enters.\n", 3, NULL, NULL, NULL );
		return qtrue;
	}
	//MJN - Force Client to certain team
	if ( Q_stricmp (cmd, "mforceteam") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_ForceTeam_f, "Usage: mforceteam <playerid> <team>\n", 3,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Locks teams for CTF
	if ( Q_stricmp (cmd, "mlockteam") == 0 ){
		M_Svcmd_LockTeam_f();
		return qtrue;
	}
	// MJN - Unlocks teams for CTF
	if ( Q_stricmp (cmd, "munlockteam") == 0 ){
		M_Svcmd_UnLockTeam_f();
		return qtrue;
	}
	// MJN - Slap
	if ( Q_stricmp (cmd, "mslap") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_Slap_f, "Usage: mslap <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Timescale
	if ( Q_stricmp (cmd, "mtimescale") == 0 ){
		M_Svcmd_TimeScale_f();
		return qtrue;
	}
	// MJN - Warn
	if ( Q_stricmp (cmd, "mwarn") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_IssueWarning_f, "Usage: mwarn <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}
	// MJN - Unwarn / Forgive
	if ( Q_stricmp (cmd, "mforgive") == 0 ){
		ExecCommandOnPlayers(M_Svcmd_UnIssueWarning_f, "Usage: mforgive <playerid>\n", 2,
			targetString, broadcastString, everyoneString);
		return qtrue;
	}

	if ( Q_stricmp (cmd, "entitylist") == 0 ) {
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( Q_stricmp (cmd, "forceteam") == 0 ) {
		Svcmd_ForceTeam_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "game_memory") == 0) {
		Svcmd_GameMem_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "addbot") == 0) {
		Svcmd_AddBot_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "botlist") == 0) {
		Svcmd_BotList_f();
		return qtrue;
	}

/*	if (Q_stricmp (cmd, "abort_podium") == 0) {
		Svcmd_AbortPodium_f();
		return qtrue;
	}
*/
	if (Q_stricmp (cmd, "addip") == 0) {
		Svcmd_AddIP_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "removeip") == 0) {
		Svcmd_RemoveIP_f();
		return qtrue;
	}

	if (Q_stricmp (cmd, "listip") == 0) {
		//[AdminCommands]
		Svcmd_ListIPs_f();
		//trap_SendConsoleCommand( EXEC_NOW, "g_banIPs\n" );
		//[/AdminCommands]
		return qtrue;
	}

	//[AdminSys]
	//added bigsay command to be able to print things to the center of the screen as the server admin.
	if (Q_stricmp (cmd, "centersay") == 0)
	{
		if(OJP_AllPlayersHaveClientPlugin())
		{//just fire off the text without editting it for line breaks.
			trap_SendServerCommand( -1, va("cp \"%s\n\"", ConcatArgs(1) ) );
		}
		else
		{//since someone is running basejka, we need to add line breaks to make up for the 
			//50 chars per line limit of basejka's cp code.
			char temp[1024];  //MAX_STRINGED_SV_STRING
			char output[1024];
			
			//copy the print text.
			strcpy(temp, ConcatArgs(1));
			
			TextWrapCenterPrint(temp, output);

			trap_SendServerCommand( -1, va("cp \"%s\n\"", output ) );
		}
		
		//also duplicate the message in the chat buffer and in the game log. (so the admin can follow admin<->player
		//conversations
		trap_SendServerCommand( -1, va("print \"server: %s\n\"", ConcatArgs(1) ) );
		G_LogPrintf( "server: %s\n", ConcatArgs(1) );
		return qtrue;
	}
	//[/AdminSys]

	if (g_dedicated.integer) {
		if (Q_stricmp (cmd, "say") == 0) {
			trap_SendServerCommand( -1, va("print \"server: %s\n\"", ConcatArgs(1) ) );
			return qtrue;
		}
		// everything else will also be printed as a say command
		trap_SendServerCommand( -1, va("print \"server: %s\n\"", ConcatArgs(0) ) );
		return qtrue;
	}

	return qfalse;
}

