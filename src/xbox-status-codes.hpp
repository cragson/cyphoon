#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

// https://xboxdevwiki.net/Xbox_Debug_Monitor#Status_codes

enum eXboxStatusCodes : int32_t
{
	OK = 200,
	CONNECTED = 201,
	MULTILINE_RESPONSE_FOLLOWS = 202,
	BINARY_RESPONSE_FOLLOWS = 203,
	SEND_BINARY_DATA = 204,
	CONNECTION_DISABLED = 205,
	UNEXPECTED_ERROR = 400,
};

inline std::unordered_map< int32_t, std::string > xbox_status_codes = {
	{ 200, "OK" }, { 201, "Connected" }, { 202, "Multiline response follows" },
	{ 400, "Unexpected error" }
};