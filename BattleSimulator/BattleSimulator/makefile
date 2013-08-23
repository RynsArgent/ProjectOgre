LIBS_LINUX = -lglut -lGLU -lGL
LIBS_WINDOWS = -lglut32 -lglu32 -lopengl32
LIBS_MAC = -framework OpenGL -framework GLUT
CC = g++

## Global header files
INCLUDE = 

## Object files and executables
MAIN_OUT = BattleSimulator.exe
PCH_OUT = pch.o
ACTION_OUT = action.o
ABILITY_OUT = ability.o
EVENT_OUT = event.o
BATTLE_OUT = battle.o
CHARACTER_OUT = character.o
DAMAGE_OUT = damage.o
GRIDPOINT_OUT = gridpoint.o
FORMATION_OUT = formation.o
GROUP_OUT = group.o
STATUS_OUT = status.o
UNIT_OUT = unit.o
TARGET_OUT = target.o
RENDERER_WINDOWS_OUT = rendererwindows.o
RENDERER_LINUX_OUT = rendererlinux.o
RENDERER_MAC_OUT = renderermac.o

## Requirements for each command
MAIN_REQS = main.cpp $(PCH_OUT) $(ACTION_OUT) $(ABILITY_OUT) $(EVENT_OUT) $(GRIDPOINT_OUT) $(BATTLE_OUT) $(GROUP_OUT) $(UNIT_OUT) $(FORMATION_OUT) $(CHARACTER_OUT) $(STATUS_OUT) $(DAMAGE_OUT) $(TARGET_OUT) 
PCH_REQS = pch.h pch.cpp
ACTION_REQS = action.h action.cpp
ABILITY_REQS = ability.h ability.cpp
EVENT_REQS = event.h event.cpp
BATTLE_REQS = battle.h battle.cpp
CHARACTER_REQS = character.h character.cpp
DAMAGE_REQS = damage.h damage.cpp
GRIDPOINT_REQS = gridpoint.h gridpoint.cpp
FORMATION_REQS = formation.h formation.cpp
GROUP_REQS = group.h group.cpp
STATUS_REQS = status.h status.cpp
UNIT_REQS = unit.h unit.cpp
TARGET_REQS = target.h target.cpp
RENDERER_REQS = renderer.h renderer.cpp

## Targets to compile for each command
MAIN_TARGETS = main.cpp $(PCH_OUT) $(ACTION_OUT) $(ABILITY_OUT) $(EVENT_OUT) $(GRIDPOINT_OUT) $(BATTLE_OUT) $(GROUP_OUT) $(UNIT_OUT) $(FORMATION_OUT) $(CHARACTER_OUT) $(STATUS_OUT) $(DAMAGE_OUT) $(TARGET_OUT)
PCH_TARGETS = pch.cpp
ACTION_TARGETS = action.cpp
ABILITY_TARGETS = ability.cpp
EVENT_TARGETS = event.cpp
BATTLE_TARGETS = battle.cpp
CHARACTER_TARGETS = character.cpp
DAMAGE_TARGETS = damage.cpp
GRIDPOINT_TARGETS = gridpoint.cpp
FORMATION_TARGETS = formation.cpp
GROUP_TARGETS = group.cpp
STATUS_TARGETS = status.cpp
UNIT_TARGETS = unit.cpp
TARGET_TARGETS = target.cpp
RENDERER_TARGETS = renderer.cpp

all:
	echo Usage: make mainwindows
	echo Usage: make mainlinux
	echo Usage: make mainmac
	echo Usage: make cleanwindows
	echo Usage: make cleanlinux
	echo Usage: make cleanmac

## Main 
mainwindows: $(MAIN_REQS) $(RENDERER_WINDOWS_OUT) $(INCLUDE)
	$(CC) -o $(MAIN_OUT) $(MAIN_TARGETS) $(RENDERER_WINDOWS_OUT) $(LIBS_WINDOWS) -D IN_WINDOWS
mainlinux: $(MAIN_REQS) $(RENDERER_LINUX_OUT) $(INCLUDE)
	$(CC) -o $(MAIN_OUT) $(MAIN_TARGETS) $(RENDERER_LINUX_OUT) $(LIBS_LINUX) -D IN_LINUX
mainmac: $(MAIN_REQS) $(RENDERER_MAC_OUT) $(INCLUDE)
	$(CC) -o $(MAIN_OUT) $(MAIN_TARGETS) $(RENDERER_MAC_OUT) $(LIBS_MAC) -D IN_MAC
## Object class
$(BATTLE_OUT): $(BATTLE_REQS) $(INCLUDE)
	$(CC) -c $(BATTLE_TARGETS)
$(ACTION_OUT): $(ACTION_REQS) $(INCLUDE)
	$(CC) -c $(ACTION_TARGETS)
$(ABILITY_OUT): $(ABILITY_REQS) $(INCLUDE)
	$(CC) -c $(ABILITY_TARGETS)
$(EVENT_OUT): $(EVENT_REQS) $(INCLUDE)
	$(CC) -c $(EVENT_TARGETS)
$(GRIDPOINT_OUT): $(GRIDPOINT_REQS) $(INCLUDE)
	$(CC) -c $(GRIDPOINT_TARGETS)
$(GROUP_OUT): $(GROUP_REQS) $(INCLUDE)
	$(CC) -c $(GROUP_TARGETS)
$(FORMATION_OUT): $(FORMATION_REQS) $(INCLUDE)
	$(CC) -c $(FORMATION_TARGETS)
$(UNIT_OUT): $(UNIT_REQS) $(INCLUDE)
	$(CC) -c $(UNIT_TARGETS)
$(CHARACTER_OUT): $(CHARACTER_REQS) $(INCLUDE)
	$(CC) -c $(CHARACTER_TARGETS)
$(STATUS_OUT): $(STATUS_REQS) $(INCLUDE)
	$(CC) -c $(STATUS_TARGETS)
$(DAMAGE_OUT) : $(DAMAGE_REQS) $(INCLUDE)
	$(CC) -c $(DAMAGE_TARGETS)
$(TARGET_OUT) : $(TARGET_REQS) $(INCLUDE)
	$(CC) -c $(TARGET_TARGETS)
$(PCH_OUT) : $(PCH_REQS) $(INCLUDE)
	$(CC) -c $(PCH_TARGETS)
$(RENDERER_WINDOWS_OUT) : $(RENDERER_REQS) $(INCLUDE)
	$(CC) -c $(RENDERER_TARGETS) -o $(RENDERER_WINDOWS_OUT) -D IN_WINDOWS
$(RENDERER_LINUX_OUT) : $(RENDERER_REQS) $(INCLUDE)
	$(CC) -c $(RENDERER_TARGETS) -o $(RENDERER_LINUX_OUT) -D IN_LINUX
$(RENDERER_MAC_OUT) : $(RENDERER_REQS) $(INCLUDE)
	$(CC) -c $(RENDERER_TARGETS) -o $(RENDERER_MAC_OUT) -D IN_MAC
cleanwindows:
	del *~ *.o *.exe
cleanlinux:
	rm -f *~ *.o *.exe
cleanmac:
	rm -f *~ *.o *.exe
