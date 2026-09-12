/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#pragma once

#include <string>

typedef signed char INT8;
typedef int INT, Ped, Vehicle, Entity, ScrHandle;
typedef unsigned long DWORD, Hash;
typedef float FLOAT;

class GTAped;
class GTAvehicle;
class GTAentity;

class GTAplayer
{
private:
	INT8 index;

public:
	GTAplayer();
	GTAplayer(const GTAplayer& obj);
	GTAplayer(INT8 handle);

	struct NetHandle13{ int _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12; };

	void SetIndex(INT8); /// set new pIndex for player

	INT8& Handle(); /// return index reference
	INT8 GetHandle() const noexcept; /// return index
	GTAped GetPed() const; /// return player's ped handle
	std::string GetName() const; /// return player's name
	bool IsActive() const; /// return if exists
	bool IsAlive() const; /// return true if alive 
	NetHandle13 GetNetHandle() const; /// return NETWORK_HANDLE
	ScrHandle GetClanDescription() const; /// return clan desc/handle
	bool IsTalking() const; /// return whether talking
	bool IsPressingHorn() const; /// return whether pressing horn
	bool IsFreeAiming() const; /// return whether free aiming
	bool IsFreeAimingAt(Entity) const; /// return whether free aiming at entity
	bool IsTargetingAnything() const; /// return whether targetting anything
	bool IsTargetingAt(Entity) const; /// return whether targetting entity
	GTAentity AimedEntity() const; /// returns entity handle for the entity the player is aiming at
	INT MaxArmour_get() const; /// return player max armour
	INT GetWantedLevel() const; /// return player wanted level

	void GiveRagdollControl(bool value);

	bool IsControlOn() const; /// return whether self control is on
	void SetControl(bool toggle, int flag); /// set control flags


	bool operator==(const GTAplayer& other) const;

};

