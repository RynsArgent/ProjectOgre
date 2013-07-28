## CS134 Winter 2013: Video Game Design
## Lab 1 Makefile
#######################################

LIBS_LINUX = -lglut -lGLU -lGL
LIBS_WINDOWS = -lglut32 -lglu32 -lopengl32
LIBS_MAC = -framework OpenGL -framework GLUT
CC = g++

## Global header files
INCLUDE = pch.h

## Object files and executables
MAIN_OUT = BattleSimulator.exe
ABILITY_OUT = ability.o
BATTLE_OUT = battle.o
CHARACTER_OUT = character.o
DAMAGE_OUT = damage.o
FORMATION_OUT = formation.o
GROUP_OUT = group.o
STATUS_OUT = status.o
UNIT_OUT = unit.o

## Requirements for each command
MAIN_REQS = main.cpp $(ABILITY_OUT) $(BATTLE_OUT) $(GROUP_OUT) $(UNIT_OUT) $(FORMATION_OUT) $(CHARACTER_OUT) $(STATUS_OUT)
ABILITY_REQS = ability.h ability.cpp
BATTLE_REQS = battle.h battle.cpp
CHARACTER_REQS = character.h character.cpp
DAMAGE_REQS = damage.h damage.cpp
FORMATION_REQS = formation.h formation.cpp
GROUP_REQS = group.h group.cpp
STATUS_REQS = status.h status.cpp
UNIT_REQS = unit.h unit.cpp

## Targets to compile for each command
MAIN_TARGETS = main.cpp $(ABILITY_OUT) $(BATTLE_OUT) $(GROUP_OUT) $(UNIT_OUT) $(FORMATION_OUT) $(CHARACTER_OUT) $(STATUS_OUT)
ABILITY_TARGETS = ability.cpp
BATTLE_TARGETS = battle.cpp
CHARACTER_TARGETS = character.cpp
DAMAGE_TARGETS = damage.cpp
FORMATION_TARGETS = formation.cpp
GROUP_TARGETS = group.cpp
STATUS_TARGETS = status.cpp
UNIT_TARGETS = unit.cpp

all:
	echo Usage: make mainwindows
	echo Usage: make mainlinux
	echo Usage: make mainmac
	echo Usage: make cleanwindows
	echo Usage: make cleanlinux
	echo Usage: make cleanmac

## Main 
mainwindows: $(MAIN_REQS) $(INCLUDE)
	$(CC) -o $(MAIN_OUT) $(MAIN_TARGETS) $(LIBS_WINDOWS) -D IN_WINDOWS
mainlinux: $(MAIN_REQS) $(INCLUDE)
	$(CC) -o $(MAIN_OUT) $(MAIN_TARGETS) $(LIBS_LINUX) -D IN_LINUX
mainmac: $(MAIN_REQS) $(INCLUDE)
	$(CC) -o $(MAIN_OUT) $(MAIN_TARGETS) $(LIBS_MAC) -D IN_MAC
## Object class
$(BATTLE_OUT): $(BATTLE_REQS) $(INCLUDE)
	$(CC) -c $(BATTLE_TARGETS)
$(ABILITY_OUT): $(ABILITY_REQS) $(INCLUDE)
	$(CC) -c $(ABILITY_TARGETS)
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

cleanwindows:
	del *~ *.o *.exe
cleanlinux:
	rm -f *~ *.o *.exe
cleanmac:
	rm -f *~ *.o *.exe
