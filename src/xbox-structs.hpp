#pragma once

#include <cstdint>
#include <string>

class xbox_module
{
public:
	std::string name;
	std::uintptr_t base;
	size_t size;
	std::uintptr_t check;
	std::uintptr_t timestamp;
	std::uintptr_t pdata;
	std::uintptr_t psize;
	std::uintptr_t dllthread;
	std::uintptr_t thread;
	size_t osize;
};

class xbox_module_section
{
public:
	std::string name;
	std::uintptr_t base;
	size_t size;
	uint32_t index;
	uint32_t flags;
};

class xbox_systeminfo
{
public:
	std::string HDD;
	std::string Type;
	std::string Platform;
	std::string System;
	std::string BaseKrnl;
	std::string Krnl;
	std::string XDK;
};

class xbox_xbeinfo
{
public:
	std::uintptr_t timestamp;
	std::uintptr_t checksum;
	std::string name;
};