// weapon.h : contains the structure for weapons used in the game.

#pragma once

#include "stdafx.h"

struct Weapon
{
public:
	int16_t index;						// 0x0
	int16_t weapon_class;				// 0x2
	int8_t ___spacer01[2074];
	float perturb_min[10];				// 0x81C, 0x28
	int8_t ___spacer02[320];
	float perturb_max[10];				// 0x984, 0x28
	int8_t ___spacer03[336];
	float ammo_damage;					// 0xAFC
	int8_t ___spacer04[756];
	float movespeed_penalty;			// 0xDF4
	float damageratio_per_node;			// 0xDF8
	int8_t ___spacer06[1032];
	float detail_perturb_shot[10];		// 0x1204, 0x28
	int8_t ___spacer07[320];
	float detail_react_pitch_shot[10];	// 0x136C, 0x28
	int8_t ___spacer08[320];
	float detail_react_yaw_shot[10];	// 0x14D4, 0x28
	int8_t ___spacer09[9040];
	float shot_react_yaw[10];			// 0x384C, 0x28
	int8_t ___spacer10[428];
	float shot_react_pitch[10];			// 0x3A20, 0x28
	int8_t ___spacer11[1464];
};