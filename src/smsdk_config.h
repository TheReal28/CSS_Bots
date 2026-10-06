/**
 * BotTest — минимальное тестовое расширение SourceMod для CS:S v34 (ветка ep1).
 *
 * Здесь настраивается публичная информация (видна в `sm exts`),
 * включается интеграция с Metamod и нужные интерфейсы SourceMod.
 */

#ifndef _INCLUDE_BOTTEST_CONFIG_H_
#define _INCLUDE_BOTTEST_CONFIG_H_

/* Публичная информация о расширении */
#define SMEXT_CONF_NAME         "BotTest"
#define SMEXT_CONF_DESCRIPTION  "Test extension for CSS v34 (bot counting via SourceMod API)"
#define SMEXT_CONF_VERSION      "1.0.0"
#define SMEXT_CONF_AUTHOR       "css34 demo"
#define SMEXT_CONF_URL          "https://github.com/rusherr-c/cssv34-sdk"
#define SMEXT_CONF_LOGTAG       "BOTTEST"
#define SMEXT_CONF_LICENSE      "GPLv3"
#define SMEXT_CONF_DATESTRING   __DATE__

/**
 * Экспортирует главный интерфейс расширения (точка входа GetSMExtAPI).
 */
#define SMEXT_LINK(name) SDKExtension *g_pExtensionIface = name;

/**
 * Включает интеграцию с Metamod:Source.
 * Расширение получает g_SMAPI и g_SHPtr (SourceHook) — это задел
 * под будущие хуки игровых классов (например, CCSBot::Update).
 */
#define SMEXT_CONF_METAMOD

/* Используемые интерфейсы SourceMod */
#define SMEXT_ENABLE_PLAYERHELPERS   /* playerhelpers: IGamePlayer, IsFakeClient, ... */

#endif // _INCLUDE_BOTTEST_CONFIG_H_
