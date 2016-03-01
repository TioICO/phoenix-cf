// engine.cpp: Contains code for the super not so awesome hacking-engine.

#include "engine.h"
#include "memory.h"

Weapon* Engine::old_weapons_[2000];

Engine::Engine(Log* log)
{
	this->file_log_ = log;
}

Engine::~Engine()
{
}

bool Engine::Initialize(HMODULE cshell)
{
	this->file_log_->Write("Initializing hacking engine.");

	// assuming cshell is a valid handle
	this->cshell_base_ = reinterpret_cast<uint32_t>(cshell);
	this->file_log_->Writef("Client Shell base address: 0x%X", this->cshell_base_);

	// weapons
	this->address_of_weapon_array_ = *reinterpret_cast<uint32_t*>(*reinterpret_cast<uint32_t*>(FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x8B\x4F\x04\xA1\x00\x00\x00\x00\x85\xC0\x8B\x51\x04\x8B\x6A\x04"), "xxxx????xxxxxxxx") + 4));
	this->weaponcheck_ = FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\xE8\x00\x00\x00\x00\x8B\xF0\x83\xC4\x04\x85\xF6\x0F\x84\x00\x00\x00\x00\xD9\x86\x00\x00\x00\x00"), "x????xxxxxxxxx????xx????");
	if (!this->address_of_weapon_array_ || !this->weaponcheck_)
		return false;

	this->weapons_ = reinterpret_cast<Weapon**>(this->address_of_weapon_array_);

	// model nodes
	this->address_of_modelnode_array_ = *reinterpret_cast<uint32_t*>(*reinterpret_cast<uint32_t*>(FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x8B\x0D\x00\x00\x00\x00\x83\xC4\x04\x89\x44\x0F\x54\x8B\x15\x00\x00\x00\x00\x8B\x04\x17\x3B\xC3\x7C\x0C\x83\xF8\x64\x7D\x07"), "xx????xxxxxxxxx????xxxxxxxxxxxx") + 2));
	if (!this->address_of_modelnode_array_)
		return false;

	this->model_nodes_ = reinterpret_cast<ModelNode*>(this->address_of_modelnode_array_);

	// FlipScreen()
	this->flipscreen_ = FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\x56\x8B\xF1\x80\x7E\x2C\x00\x74\x7D\x8B\x0D\x00\x00\x00\x00"), "xxxxxxxxxxx????");
	this->call_to_flipscreen_ = FindPattern(this->cshell_base_, 0xFFFFFF, reinterpret_cast<uint8_t*>("\xE8\x00\x00\x00\x00\xC6\x46\x00\x00\x5F\x5E\x33\xC0\x5B\x83\xC4\x00"), "x????xx??xxxxxxx?");
	if (!this->flipscreen_ || !this->call_to_flipscreen_)
		return false;

	this->file_log_->Write("Hacking engine successfully initialized.");
	this->initialized_ = true;
	return true;
}

void Engine::HookFlipScreen(uint32_t routine, uint32_t* original)
{
	this->file_log_->Write("Setting up game hook: FlipScreen().");

	// let the caller know what the address of the original flipscreen function is
	*original = this->flipscreen_;

	uint32_t relative_address = routine - this->call_to_flipscreen_ - 5;
	*reinterpret_cast<uint32_t*>(this->call_to_flipscreen_ + 1) = relative_address;
}

void Engine::Backup()
{
	for (int32_t i = 0; i < 2000; i++)
	{
		if (this->weapons_[i] && this->weapons_[i]->index == i)
		{
			Engine::old_weapons_[i] = new Weapon;
			memcpy(reinterpret_cast<void*>(Engine::old_weapons_[i]), reinterpret_cast<void*>(this->weapons_[i]), 0x4000);
		}
	}
}

void Engine::Run()
{
	// trying to not drop the fps on slow machines, this might result in a more or less
	// noticeable delay when toggling features
	/*if (difftime(time(0), this->last_run_) < 0.2)
		return;*/

	if (this->address_of_modelnode_array_)
	{
		for (int32_t i = 0; i < 300; i++)
		{
			this->model_nodes_[i].type = 1;
		}
	}

	if (this->address_of_weapon_array_)
	{
		// loop through all weapons
		for (int32_t i = 0; i < 2000; i++)
		{
			// check if it's actually a valid weapon
			if (this->weapons_[i] && this->weapons_[i]->index == i)
			{
				// full (and perfect) no recoil, will give client error 28_3 if not bypassed
				for (int32_t y = 0; y < 10; y++)
				{
					this->weapons_[i]->perturb_max[y] = 0.0f;
					this->weapons_[i]->perturb_min[y] = 0.0f;
					this->weapons_[i]->detail_perturb_shot[y] = 0.0f;
					this->weapons_[i]->detail_react_pitch_shot[y] = 0.0f;
					this->weapons_[i]->detail_react_yaw_shot[y] = 0.0f;
					this->weapons_[i]->shot_react_yaw[y] = 0.0f;
					this->weapons_[i]->shot_react_pitch[y] = 0.0f;
				}

				for (int32_t y = 0; y < 30; y++)
				{
					this->weapons_[i]->damageratio_per_node[y] = 1.48f;
				}
			}
		}
	}

	//this->last_run_ = time(0);
}

void Engine::DetourWeaponCheck()
{
	uint32_t relative_address = reinterpret_cast<uint32_t>(&Engine::GetOldWeaponByIndex) - this->weaponcheck_ - 5;
	*reinterpret_cast<uint32_t*>(this->weaponcheck_ + 1) = relative_address;
}
