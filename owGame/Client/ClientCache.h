#pragma once

#include "WoWGameObject.h"
#include "WoWUnit.h"

#pragma region GameObject

struct ZN_API SGameObjectQueryResult
{
	SGameObjectQueryResult()
		: entryID(0)
	{}

	uint32          entryID;
	GameobjectTypes type;
	uint32          displayId;
	std::string     Name;
	std::string     Name2;
	std::string     Name3;
	std::string     Name4;

	union                                                   // different GO types have different data field
	{
		//0 GAMEOBJECT_TYPE_DOOR
		struct
		{
			uint32 startOpen;                               //0 used client side to determine GO_ACTIVATED means open/closed
			uint32 lockId;                                  //1 -> Lock.dbc
			uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
			uint32 noDamageImmune;                          //3 break opening whenever you recieve damage?
			uint32 openTextID;                              //4 can be used to replace castBarCaption?
			uint32 closeTextID;                             //5
		} door;
		//1 GAMEOBJECT_TYPE_BUTTON
		struct
		{
			uint32 startOpen;                               //0
			uint32 lockId;                                  //1 -> Lock.dbc
			uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
			uint32 linkedTrap;                              //3
			uint32 noDamageImmune;                          //4 isBattlegroundObject
			uint32 large;                                   //5
			uint32 openTextID;                              //6 can be used to replace castBarCaption?
			uint32 closeTextID;                             //7
			uint32 losOK;                                   //8
		} button;
		//2 GAMEOBJECT_TYPE_QUESTGIVER
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 questList;                               //1
			uint32 pageMaterial;                            //2
			uint32 gossipID;                                //3
			uint32 customAnim;                              //4
			uint32 noDamageImmune;                          //5
			uint32 openTextID;                              //6 can be used to replace castBarCaption?
			uint32 losOK;                                   //7
			uint32 allowMounted;                            //8
			uint32 large;                                   //9
		} questgiver;
		//3 GAMEOBJECT_TYPE_CHEST
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 lootId;                                  //1
			uint32 chestRestockTime;                        //2
			uint32 consumable;                              //3
			uint32 minSuccessOpens;                         //4
			uint32 maxSuccessOpens;                         //5
			uint32 eventId;                                 //6 lootedEvent
			uint32 linkedTrapId;                            //7
			uint32 questId;                                 //8 not used currently but store quest required for GO activation for player
			uint32 level;                                   //9
			uint32 losOK;                                   //10
			uint32 leaveLoot;                               //11
			uint32 notInCombat;                             //12
			uint32 logLoot;                                 //13
			uint32 openTextID;                              //14 can be used to replace castBarCaption?
			uint32 groupLootRules;                          //15
		} chest;
		//5 GAMEOBJECT_TYPE_GENERIC
		struct
		{
			uint32 floatingTooltip;                         //0
			uint32 highlight;                               //1
			uint32 serverOnly;                              //2
			uint32 large;                                   //3
			uint32 floatOnWater;                            //4
			uint32 questID;                                 //5
		} _generic;
		//6 GAMEOBJECT_TYPE_TRAP
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 level;                                   //1
			uint32 radius;                                  //2 radius for trap activation
			uint32 spellId;                                 //3
			uint32 charges;                                 //4 need respawn (if > 0)
			uint32 cooldown;                                //5 time in secs
			uint32 autoCloseTime;                           //6
			uint32 startDelay;                              //7
			uint32 serverOnly;                              //8
			uint32 stealthed;                               //9
			uint32 large;                                   //10
			uint32 stealthAffected;                         //11
			uint32 openTextID;                              //12 can be used to replace castBarCaption?
			uint32 closeTextID;                             //13
		} trap;
		//7 GAMEOBJECT_TYPE_CHAIR
		struct
		{
			uint32 slots;                                   //0
			uint32 height;                                  //1
			uint32 onlyCreatorUse;                          //2
		} chair;
		//8 GAMEOBJECT_TYPE_SPELL_FOCUS
		struct
		{
			uint32 focusId;                                 //0
			uint32 dist;                                    //1
			uint32 linkedTrapId;                            //2
			uint32 serverOnly;                              //3
			uint32 questID;                                 //4
			uint32 large;                                   //5
		} spellFocus;
		//9 GAMEOBJECT_TYPE_TEXT
		struct
		{
			uint32 pageID;                                  //0
			uint32 language;                                //1
			uint32 pageMaterial;                            //2
			uint32 allowMounted;                            //3
		} text;
		//10 GAMEOBJECT_TYPE_GOOBER
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 questId;                                 //1
			uint32 eventId;                                 //2
			uint32 autoCloseTime;                           //3
			uint32 customAnim;                              //4
			uint32 consumable;                              //5
			uint32 cooldown;                                //6
			uint32 pageId;                                  //7
			uint32 language;                                //8
			uint32 pageMaterial;                            //9
			uint32 spellId;                                 //10
			uint32 noDamageImmune;                          //11
			uint32 linkedTrapId;                            //12
			uint32 large;                                   //13
			uint32 openTextID;                              //14 can be used to replace castBarCaption?
			uint32 closeTextID;                             //15
			uint32 losOK;                                   //16 isBattlegroundObject
			uint32 allowMounted;                            //17
		} goober;
		//11 GAMEOBJECT_TYPE_TRANSPORT
		struct
		{
			uint32 pause;                                   //0
			uint32 startOpen;                               //1
			uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
		} transport;
		//12 GAMEOBJECT_TYPE_AREADAMAGE
		struct
		{
			uint32 lockId;                                  //0
			uint32 radius;                                  //1
			uint32 damageMin;                               //2
			uint32 damageMax;                               //3
			uint32 damageSchool;                            //4
			uint32 autoCloseTime;                           //5 secs till autoclose = autoCloseTime / 0x10000
			uint32 openTextID;                              //6
			uint32 closeTextID;                             //7
		} areadamage;
		//13 GAMEOBJECT_TYPE_CAMERA
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 cinematicId;                             //1
			uint32 eventID;                                 //2
			uint32 openTextID;                              //3 can be used to replace castBarCaption?
		} camera;
		//15 GAMEOBJECT_TYPE_MO_TRANSPORT
		struct
		{
			uint32 taxiPathId;                              //0
			uint32 moveSpeed;                               //1
			uint32 accelRate;                               //2
			uint32 startEventID;                            //3
			uint32 stopEventID;                             //4
			uint32 transportPhysics;                        //5
			uint32 mapID;                                   //6
		} moTransport;
		//17 GAMEOBJECT_TYPE_FISHINGNODE
		struct
		{
			uint32 _data0;                                  //0
			uint32 lootId;                                  //1
		} fishnode;
		//18 GAMEOBJECT_TYPE_SUMMONING_RITUAL
		struct
		{
			uint32 reqParticipants;                         //0
			uint32 spellId;                                 //1
			uint32 animSpell;                               //2
			uint32 ritualPersistent;                        //3
			uint32 casterTargetSpell;                       //4
			uint32 casterTargetSpellTargets;                //5
			uint32 castersGrouped;                          //6
			uint32 ritualNoTargetCheck;                     //7
		} summoningRitual;
		//20 GAMEOBJECT_TYPE_AUCTIONHOUSE
		struct
		{
			uint32 actionHouseID;                           //0
		} auctionhouse;
		//21 GAMEOBJECT_TYPE_GUARDPOST
		struct
		{
			uint32 creatureID;                              //0
			uint32 charges;                                 //1
		} guardpost;
		//22 GAMEOBJECT_TYPE_SPELLCASTER
		struct
		{
			uint32 spellId;                                 //0
			uint32 charges;                                 //1
			uint32 partyOnly;                               //2
		} spellcaster;
		//23 GAMEOBJECT_TYPE_MEETINGSTONE
		struct
		{
			uint32 minLevel;                                //0
			uint32 maxLevel;                                //1
			uint32 areaID;                                  //2
		} meetingstone;
		//24 GAMEOBJECT_TYPE_FLAGSTAND
		struct
		{
			uint32 lockId;                                  //0
			uint32 pickupSpell;                             //1
			uint32 radius;                                  //2
			uint32 returnAura;                              //3
			uint32 returnSpell;                             //4
			uint32 noDamageImmune;                          //5
			uint32 openTextID;                              //6
			uint32 losOK;                                   //7
		} flagstand;
		//25 GAMEOBJECT_TYPE_FISHINGHOLE                    // not implemented yet
		struct
		{
			uint32 radius;                                  //0 how close bobber must land for sending loot
			uint32 lootId;                                  //1
			uint32 minSuccessOpens;                         //2
			uint32 maxSuccessOpens;                         //3
			uint32 lockId;                                  //4 -> Lock.dbc; possibly 1628 for all?
		} fishinghole;
		//26 GAMEOBJECT_TYPE_FLAGDROP
		struct
		{
			uint32 lockId;                                  //0
			uint32 eventID;                                 //1
			uint32 pickupSpell;                             //2
			uint32 noDamageImmune;                          //3
			uint32 openTextID;                              //4
		} flagdrop;
		//27 GAMEOBJECT_TYPE_MINI_GAME
		struct
		{
			uint32 gameType;                                //0
		} miniGame;
		//29 GAMEOBJECT_TYPE_CAPTURE_POINT
		struct
		{
			uint32 radius;                                  //0
			uint32 spell;                                   //1
			uint32 worldState1;                             //2
			uint32 worldstate2;                             //3
			uint32 winEventID1;                             //4
			uint32 winEventID2;                             //5
			uint32 contestedEventID1;                       //6
			uint32 contestedEventID2;                       //7
			uint32 progressEventID1;                        //8
			uint32 progressEventID2;                        //9
			uint32 neutralEventID1;                         //10
			uint32 neutralEventID2;                         //11
			uint32 neutralPercent;                          //12
			uint32 worldstate3;                             //13
			uint32 minSuperiority;                          //14
			uint32 maxSuperiority;                          //15
			uint32 minTime;                                 //16
			uint32 maxTime;                                 //17
			uint32 large;                                   //18
			uint32 highlight;                               //19
		} capturePoint;
		//30 GAMEOBJECT_TYPE_AURA_GENERATOR
		struct
		{
			uint32 startOpen;                               //0
			uint32 radius;                                  //1
			uint32 auraID1;                                 //2
			uint32 conditionID1;                            //3
			uint32 auraID2;                                 //4
			uint32 conditionID2;                            //5
			uint32 serverOnly;                              //6
		} auraGenerator;

		// not use for specific field access (only for output with loop by all filed), also this determinate max union size
		struct                                              // GAMEOBJECT_TYPE_SPELLCASTER
		{
			uint32 data[24];
		} raw;
	};

	void Fill(CByteBuffer& Bytes)
	{
		Bytes >> entryID;
		Bytes.read(&type);
		Bytes >> displayId;
		Bytes >> &Name;
		Bytes >> &Name2;
		Bytes >> &Name3;
		Bytes >> &Name4;
		Bytes.readBytes(&raw.data, sizeof(raw.data));
	}

	void Print()
	{
		Log::Info("GameObject query: ID '%d', Type '%d', Name '%s'", entryID, type, Name.c_str());
	}
};

#pragma endregion


#pragma region Creatures

struct ZN_API SCreatureQueryResult
{
	SCreatureQueryResult()
		: entry(0)
	{}

	uint32 entry;                          // creature entry
	std::string Name;
	std::string Name2;
	std::string Name3;
	std::string Name4;
	std::string SubName;
	CreatureTypeFlags type_flags;          // flags          wdbFeild7=wad flags1
	CreatureType type;
	CreatureFamily family;                 // family         wdbFeild9
	uint32 rank;                           // rank           wdbFeild10
	uint32 unk0;                           // unknown        wdbFeild11
	uint32 PetSpellDataId;                 // Id from CreatureSpellData.dbc    wdbField12
	uint32 Modelid_A1;                     // Modelid_A1
	uint32 Modelid_A2;                     // Modelid_A2
	uint32 Modelid_H1;                     // Modelid_H1
	uint32 Modelid_H2;                     // Modelid_H2
	float unk1;                            // unk
	float unk2;                            // unk
	uint8 RacialLeader;

	void Fill(CByteBuffer& Bytes)
	{
		Bytes >> entry;
		Bytes >> &Name;
		Bytes >> &Name2;
		Bytes >> &Name3;
		Bytes >> &Name4;
		Bytes >> &SubName;
		Bytes.read(&type_flags);
		Bytes.read(&type);
		Bytes.read(&family);
		Bytes >> rank;
		Bytes >> unk0;
		Bytes >> PetSpellDataId;
		Bytes >> Modelid_A1;
		Bytes >> Modelid_A2;
		Bytes >> Modelid_H1;
		Bytes >> Modelid_H2;
		Bytes >> unk1;
		Bytes >> unk2;
		Bytes >> RacialLeader;
	}

	void Print()
	{
		Log::Info("Creature query: ID '%d', Name '%s', SubName '%s'", entry, Name.c_str(), SubName.c_str());
	}
};

#pragma endregion
