/**
 * BotTest — минимальное тестовое расширение SourceMod для CS:S v34.
 *
 * Что делает:
 *   - при загрузке пишет сообщение в лог SourceMod;
 *   - регистрирует три натива для SourcePawn:
 *       BotTest_GetBotCount()      — сколько ботов на сервере
 *       BotTest_GetHumanCount()    — сколько реальных игроков
 *       BotTest_IsFakeClient(int)  — является ли клиент ботом
 *
 * Нативы используют только собственный API SourceMod (IPlayerManager),
 * поэтому расширение не зависит от сигнатур/оффсетов server.dll.
 */

#include "extension.h"

/** Синглтон расширения */
static BotTest g_BotTest;

/** Экспортирует точку входа GetSMExtAPI */
SMEXT_LINK(&g_BotTest);

static int CountPlayers(bool wantBots)
{
	int count = 0;
	int maxClients = playerhelpers->GetMaxClients();

	for (int i = 1; i <= maxClients; i++)
	{
		IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(i);
		if (pPlayer == NULL || !pPlayer->IsConnected())
			continue;
		if (pPlayer->IsFakeClient() == wantBots)
			count++;
	}

	return count;
}

// native int BotTest_GetBotCount();
static cell_t Native_GetBotCount(IPluginContext *pContext, const cell_t *params)
{
	return CountPlayers(true);
}

// native int BotTest_GetHumanCount();
static cell_t Native_GetHumanCount(IPluginContext *pContext, const cell_t *params)
{
	return CountPlayers(false);
}

// native bool BotTest_IsFakeClient(int client);
static cell_t Native_IsFakeClient(IPluginContext *pContext, const cell_t *params)
{
	IGamePlayer *pPlayer = playerhelpers->GetGamePlayer(params[1]);
	if (pPlayer == NULL)
	{
		return pContext->ThrowNativeError("Client index %d is invalid", params[1]);
	}
	if (!pPlayer->IsConnected())
	{
		return pContext->ThrowNativeError("Client %d is not connected", params[1]);
	}
	return pPlayer->IsFakeClient() ? 1 : 0;
}

static const sp_nativeinfo_t g_BotTestNatives[] =
{
	{ "BotTest_GetBotCount",   Native_GetBotCount   },
	{ "BotTest_GetHumanCount", Native_GetHumanCount },
	{ "BotTest_IsFakeClient",  Native_IsFakeClient  },
	{ NULL,                    NULL                 }
};

// Размерные operator delete (C++14) — их использует AMTL из SM 1.11.
// Без этих определений появится зависимость от libstdc++.so.6
// (smsdk_ext.cpp переопределяет только безразмерные варианты).
void operator delete(void *ptr, size_t)
{
	free(ptr);
}

void operator delete[](void *ptr, size_t)
{
	free(ptr);
}

bool BotTest::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	g_pSM->LogMessage(myself, "BotTest v%s loaded (%s, SourceHook: %s)",
		GetExtensionVerString(),
		late ? "late load" : "normal load",
		(g_SHPtr != NULL) ? "available" : "NOT available");
	return true;
}

void BotTest::SDK_OnAllLoaded()
{
	sharesys->AddNatives(myself, g_BotTestNatives);
}
