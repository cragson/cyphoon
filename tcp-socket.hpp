#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

class tcp_socket
{
public:

	tcp_socket()
	{
		this->m_ip = {};

		this->m_port = 730;

		this->m_client_fd = {};

	}

	~tcp_socket()
	{
		closesocket(this->m_client_fd);

		WSACleanup();
	}

	[[nodiscard]] bool connect_to_xbox(const std::string& xbox_ip);

	[[nodiscard]] std::string recv_message() const;

	[[nodiscard]] std::vector< std::string > recv_multiline_response();

	void send_message(const std::string& msg) const;

	[[nodiscard]] bool release_socket();

	[[nodiscard]] auto get_ip() const noexcept
	{
		return this->m_ip;
	}

	[[nodiscard]] auto get_port() const noexcept
	{
		return this->m_port;
	}

	[[nodiscard]] auto get_client_fd() const noexcept
	{
		return this->m_client_fd;
	}

private:

	std::string m_ip;

	uint32_t m_port;

	SOCKET m_client_fd;
};