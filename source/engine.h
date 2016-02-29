// engine.h: include file for my not so awesome hacking-engine.

#pragma once

#include "stdafx.h"
#include "weapon.h"

class Engine
{
public:
	Engine();
	~Engine();

	bool Initialize(HMODULE cshell);
	void HookFlipScreen(uint32_t routine, uint32_t* original);
	void Backup();
	void Run();
	void DetourWeaponCheck();

	static Weapon* GetOldWeaponByIndex(int32_t index)
	{
		return Engine::old_weapons_[index];
	}

private:
	static Weapon *old_weapons_[2000];
	
	uint32_t cshell_base_;
	uint32_t flipscreen_;
	uint32_t call_to_flipscreen_;
	uint32_t address_of_weapon_array_;
	uint32_t weaponcheck_;
	Weapon **weapons_;
	time_t last_run_;
	bool initialized_ = false;
};