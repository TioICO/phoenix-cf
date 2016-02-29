// memory.h : include file for basic memory-editing functions.

#pragma once

#include "stdafx.h"

bool PlaceBytes(void* dst, uint8_t* src, size_t len);
uint32_t FindPattern(uint32_t address, size_t len, uint8_t *bMask, char *szMask);
bool ComparePattern(const uint8_t* pData, const uint8_t* bMask, const char* szMask);