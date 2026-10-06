#if defined _bottest_included
 #endinput
#endif
#define _bottest_included

/**
 * Возвращает количество ботов на сервере.
 *
 * @return  Число фейк-клиентов.
 */
native int BotTest_GetBotCount();

/**
 * Возвращает количество реальных игроков на сервере.
 *
 * @return  Число живых игроков.
 */
native int BotTest_GetHumanCount();

/**
 * Проверяет, является ли клиент ботом.
 *
 * @param client    Индекс игрока (1..MaxClients).
 * @return          True, если клиент — бот.
 * @error           Неверный индекс или клиент не подключён.
 */
native bool BotTest_IsFakeClient(int client);

/**
 * Не редактировать ниже этой строки!
 */
public Extension __ext_bottest =
{
	name = "BotTest",
	file = "bottest.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public void __ext_bottest_SetNTVOptional()
{
	MarkNativeAsOptional("BotTest_GetBotCount");
	MarkNativeAsOptional("BotTest_GetHumanCount");
	MarkNativeAsOptional("BotTest_IsFakeClient");
}
#endif
