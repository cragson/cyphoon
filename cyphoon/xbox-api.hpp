#pragma once

#include <memory>

#include "tcp-socket.hpp"
#include "xbox-structs.hpp"

class xbox_api
{
public:

	xbox_api()
	{
		this->m_socket = std::make_unique< tcp_socket >();
	}

	~xbox_api()
	{
		this->send_raw_command_to_xbox("bye");

		const auto resp = this->m_socket->recv_message();
	}

	[[nodiscard]] bool setup( const std::string& xbox_ip );

	void send_raw_command_to_xbox(const std::string& cmd);

	[[nodiscard]] std::unique_ptr< xbox_systeminfo > get_system_info();

	[[nodiscard]] std::unique_ptr< xbox_xbeinfo > get_xbeinfo();

	[[nodiscard]] std::vector< std::unique_ptr< xbox_module > > get_modules();

	[[nodiscard]] std::vector < std::unique_ptr< xbox_module_section > > get_sections_of_module(const std::string& module_name);

	[[nodiscard]] std::vector< uint8_t > read_memory_block(const std::uintptr_t address, const size_t size);
	[[nodiscard]] uint8_t read_byte(const std::uintptr_t address);
	[[nodiscard]] uint16_t read_word(const std::uintptr_t address);
	[[nodiscard]] uint32_t read_dword(const std::uintptr_t address);

	bool write_memory_block(const std::uintptr_t address, const std::vector< uint8_t >& bytes);
	bool write_byte(const std::uintptr_t address, const uint8_t byte);
	bool write_word(const std::uintptr_t address, const uint16_t word);
	bool write_dword(const std::uintptr_t address, const uint32_t dword);

	void hexdump_memory(const std::uintptr_t& address, const size_t size = 256);

	[[nodiscard]] auto get_xbdm_ip() const noexcept
	{
		return this->m_socket->get_ip();
	}

	[[nodiscard]] auto get_xbdm_port() const noexcept
	{
		return this->m_socket->get_port();
	}

private:
	std::unique_ptr< tcp_socket > m_socket;
};