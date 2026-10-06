#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <bottest>

public Plugin myinfo =
{
	name        = "BotTest",
	author      = "css34 demo",
	description = "Test plugin for the BotTest extension (CS:S v34)",
	version     = "1.0.0",
	url         = "https://github.com/rusherr-c/cssv34-sdk"
};

public void OnPluginStart()
{
	RegConsoleCmd("sm_bots", Cmd_Bots, "Show number of bots and humans on the server");
	PrintToServer("[BotTest] plugin loaded, extension natives ready");
}

public Action Cmd_Bots(int client, int args)
{
	int bots   = BotTest_GetBotCount();
	int humans = BotTest_GetHumanCount();

	ReplyToCommand(client, "[BotTest] Bots: %d | Humans: %d | Total: %d",
		bots, humans, bots + humans);
	return Plugin_Handled;
}

public void OnClientPutInServer(int client)
{
	if (BotTest_IsFakeClient(client))
	{
		PrintToChatAll("[BotTest] a bot joined (bots now: %d)", BotTest_GetBotCount());
	}
}
