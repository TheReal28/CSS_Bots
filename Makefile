# BotTest — минимальное расширение SourceMod для CS:S v34 (ep1, Linux x86)
#
#   make            — собрать bottest.ext.2.ep1.so
#   make clean      — почистить
#   make plugin SPCOMP=/путь/к/spcomp — собрать bottest.smx
#                     (по умолчанию spcomp ищется в PATH)
#
# Требуется 32-битный тулчейн:  sudo apt install gcc-multilib g++-multilib

CXX      ?= g++
PROJECT  := bottest
BINARY   := $(PROJECT).ext.2.ep1.so

# Пути к вендоренным зависимостям
#   sourcemod — заголовки SourceMod 1.10 (ветка 1.10-dev)
#   metamod   — заголовки Metamod:Source 1.10.6
#   hl2sdk    — заголовки игры (rom4s/hl2sdk-ep1c, ветка game-cstrike-1,
#               с небольшим патчем под Linux: deps/patches)
SM     := deps/sourcemod
MMS    := deps/metamod
HL2SDK := deps/hl2sdk

# Компилятор и флаги — по образцу public/sample_ext из SourceMod 1.10
CXXFLAGS += -m32 -std=c++14 -O2 -pipe -fno-strict-aliasing \
	-fvisibility=hidden -fvisibility-inlines-hidden \
	-fno-exceptions -fno-rtti -msse -mfpmath=sse \
	-DNDEBUG -D_LINUX -DPOSIX -DCOMPILER_GCC -DSOURCEMOD_BUILD -DHAVE_STDINT_H \
	-Dstricmp=strcasecmp -D_stricmp=strcasecmp \
	-Dstrnicmp=strncasecmp -D_strnicmp=strncasecmp \
	-D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca \
	-DSOURCE_ENGINE=1 -DSE_EPISODEONE=1 \
	-Wall -fpermissive -Wno-overloaded-virtual -Wno-switch -Wno-unused \
	-Wno-non-virtual-dtor -Wno-delete-non-virtual-dtor -MMD -MP

INCLUDES := -Isrc \
	-I$(SM)/public -I$(SM)/public/amtl -I$(SM)/public/amtl/amtl -I$(SM)/sourcepawn/include \
	-I$(MMS)/core -I$(MMS)/core/sourcehook \
	-I$(HL2SDK)/public -I$(HL2SDK)/public/tier0 -I$(HL2SDK)/public/tier1 \
	-I$(HL2SDK)/public/engine

LDFLAGS += -m32 -shared -static-libgcc
LDLIBS  += -lm -ldl

OBJ := obj/extension.o obj/smsdk_ext.o

all: $(BINARY)

obj:
	@mkdir -p obj

obj/extension.o: src/extension.cpp src/extension.h src/smsdk_config.h | obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

obj/smsdk_ext.o: $(SM)/public/smsdk_ext.cpp $(SM)/public/smsdk_ext.h src/smsdk_config.h | obj
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BINARY): $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o $@ $(LDLIBS)
	@echo "==> $(BINARY) готов"
	@file $(BINARY) || true

# Сборка SourcePawn-плагина (нужен spcomp из пакета SourceMod css34)
SPCOMP ?= spcomp
SM_INCLUDE ?= scripting/include

plugin:
	cd scripting && $(SPCOMP) -i$(abspath $(SM_INCLUDE)) -iinclude -O2 -v2 \
		$(PROJECT).sp -o ../$(PROJECT).smx

clean:
	rm -rf obj $(BINARY) $(PROJECT).smx

.PHONY: all clean plugin

-include $(OBJ:.o=.d)
