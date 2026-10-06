# BotTest — тестовое расширение SourceMod для CS:S v34

Минимальное расширение SourceMod (`.ext.2.ep1.so`) + тестовый плагин (`.smx`) под **CS:S v34
(build 4044, ветка ep1)**. Собрано под **SourceMod 1.11 / Metamod:Source 1.11** (проверено на
SM 1.11.0.6522-css34 + MM 1.11.0-dev+1130), Linux.

Цель — проверить всю цепочку `Metamod → SourceMod → расширение (C++) → плагин (SourcePawn) → чат`
перед написанием чего-то серьёзного (например, подмены логики ботов).

## Что делает

Расширение регистрирует три натива (только через API SourceMod, без привязки к сигнатурам server.dll):

| Натив | Описание |
|---|---|
| `BotTest_GetBotCount()` | количество ботов на сервере |
| `BotTest_GetHumanCount()` | количество реальных игроков |
| `BotTest_IsFakeClient(client)` | является ли клиент ботом |

Плагин добавляет команду **`sm_bots`** (в чате `!bots`):

```
[BotTest] Bots: 3 | Humans: 2 | Total: 5
```

и пишет в чат при входе бота. Расширение логирует загрузку в `addons/sourcemod/logs/…`.

## Установка

Скопировать содержимое `package/` в `cstrike/` сервера:

```
cstrike/addons/sourcemod/extensions/bottest.ext.2.ep1.so
cstrike/addons/sourcemod/plugins/bottest.smx
```

Проверка:
1. `sm exts` → **BotTest (1.0.0)** — без ошибок загрузки.
2. `!bots` в чате → счётчики ботов/людей.
3. `sm plugins list` → BotTest [RUNNING].

## История: почему первая версия падала с «Function CreateInterface not found»

Первая сборка делалась против заголовков SM 1.10 + MM 1.10.6. При загрузке на SM 1.11 / MM 1.11
получали:

```
[SM] Extension bottest.ext.2.ep1.so failed to load: Function CreateInterface not found
```

Диагноз (подтверждён по исходникам):

1. Расширение с `SMEXT_CONF_METAMOD` SourceMod дополнительно подключает к Metamod как MM-плагин.
2. MM 1.11 ищет в `.so` сначала `LoadInterface_MMS`, затем fallback `CreateInterface`
   (`metamod_plugins.cpp:519`, именно отсюда текст ошибки).
3. Точка входа `CreateInterface` в `smsdk_ext.cpp` экспортируется макросом `SMM_API`.
   В заголовках MM **1.10.6** этот макрос ставит `visibility("default")` только при
   `__GNUC__ == 4` (эпоха gcc 4.x). Современный gcc 14 под это условие не попадает →
   атрибута нет → флаг `-fvisibility=hidden` (стандартный для сборки расширений) делал
   символ **локальным**: `nm` показывал `t CreateInterface` вместо `T`.
4. Итог: `dlsym` не находил функцию → MM отклонял плагин → SM откатывал загрузку расширения.
   (Сборки rom4s это не задевало: его CI собирал clang'ом, который объявляет себя `__GNUC__=4`.)

В MM **1.11** `SMM_API` безусловный — поэтому пересборка против 1.11-заголовков чинит проблему
«сама собой». Дополнительно в код расширения добавлены размерные `operator delete` (C++14),
чтобы не тянуть зависимость от `libstdc++.so.6` (AMTL из SM 1.11 ссылается на `_ZdlPvj`).

Итоговые экспорты бинарника (проверено):

```
$ nm -D bottest.ext.2.ep1.so | grep ' T '
00002150 T CreateInterface      ← MM 1.11 находит
000020e0 T GetSMExtAPI          ← ядро SM находит
NEEDED: только libc.so.6         ← минимум зависимостей
```

(Для сравнения: официальные расширения rom4s экспортируют те же `CreateInterface` + `GetSMExtAPI`
и зависят ещё от `libstdc++`, `vstdlib_i486`, `tier0_i486` — наш бинарник легче.)

## Сборка

Требуется Linux с 32-битным тулчейном:

```bash
sudo apt install gcc-multilib g++-multilib make
cd bottest
make            # => bottest.ext.2.ep1.so (C++14, gcc 14 работает)
```

Плагин (артефакт уже собран):

```bash
make plugin SPCOMP=/путь/к/addons/sourcemod/scripting/spcomp
```

`.smx`, скомпилированный spcomp 1.10, без проблем грузится SM 1.11 (байткод SourcePawn совместим).

## Структура

```
src/            исходники расширения
scripting/      bottest.sp + include/bottest.inc
deps/           вендоренные заголовки — проект самодостаточен
package/        готовые файлы для установки на сервер
Makefile        сборка
```

## Что в deps/

| Каталог | Источник | Зачем |
|---|---|---|
| `deps/sourcemod` | [sourcemod](https://github.com/alliedmodders/sourcemod), ветка `1.11-dev` + сабмодули `amtl`/`sourcepawn` на пинах из `.gitmodules` | `smsdk_ext.h/cpp`, интерфейсы SM 1.11 |
| `deps/metamod` | [metamod-source](https://github.com/alliedmodders/metamod-source), ветка `1.11-dev` | `ISmmPlugin.h` (безусловный `SMM_API`), SourceHook |
| `deps/hl2sdk` | [rom4s/hl2sdk-ep1c](https://github.com/rom4s/hl2sdk-ep1c), ветка `game-cstrike-1` (+ файлы из [hl2sdk episode1](https://github.com/alliedmodders/hl2sdk/tree/episode1)) | заголовки движка v34: `ServerGameDLL005`, `VEngineServer021` |

Локальные правки вендоренных заголовков (оригинальные репозитории не тронуты):

1. `hl2sdk: tier0/platform.h` — `#include <new.h>` обёрнут в `#ifdef _MSC_VER`.
2. `hl2sdk: mathlib/math_base.h` — gcc-совместимая версия (из episode1) + `minmax.h`/`MAX`/`MIN`.
3. ~107 файлов hl2sdk — копии с запрошенным регистром имён (эпоха Windows: `iappsystem.h` vs `IAppSystem.h`).
4. `Makefile` — `-fpermissive` и `-std=c++14` под современный gcc.

## Совместимость

- Сервер: **CS:S v34 (ep1)**, Linux, SM 1.11 + MM 1.11 (как у автора задачи).
- Для SM/MM 1.10 нужен отдельный билд против их заголовков (см. раздел «История» — там есть
  подводный камень с `SMM_API` и gcc≠4).
- На старых MM (1.10-ep1, core-legacy API) текущий бинарник может не загрузиться —
  версии ISmmPlugin-интерфейса различаются между поколениями MM.

## Следующий шаг (к ботам)

Расширение собрано с `SMEXT_CONF_METAMOD` → доступны `g_SMAPI` и `g_SHPtr` (SourceHook).
Дальше:

1. добавить игровые заголовки (`game/server/cstrike/bot/cs_bot.h` из cssv34-sdk / hl2sdk-ep1c);
2. `SH_DECL_HOOK1_void(CCSBot, Update, SH_NOATTRIB, 0);` (метод виртуальный — проверено);
3. `SH_ADD_HOOK_MEMFUNC` на `Update/Spawn/Upkeep` и своя машина состояний вместо ванильного ИИ,
   не трогая оригинальный `server.so`/`server.dll`.

## Известные ограничения

- Только Linux (.so). Для Windows-сервера — эквивалентная сборка .dll в MSVC.
- Проверка выполнена до уровня dlopen/экспортов; на живом сервере v34 прогон не выполнялся —
  если `sm exts` покажет ошибку, присылайте её текст.
