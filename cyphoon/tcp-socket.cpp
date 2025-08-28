#include "tcp-socket.hpp"

#include <format>
#include <iostream>
#include <ws2tcpip.h>
#include <print>

#include "xbox-status-codes.hpp"
#include "utils.hpp"

bool tcp_socket::connect_to_xbox(const std::string& xbox_ip)
{
	if (xbox_ip.empty())
	{
		std::println("[!] Xbox IP cannot be empty!");

		return false;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::println("[!] Could not initialize Winsock!\n");
		return false;
	}

	this->m_client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->m_client_fd == INVALID_SOCKET)
	{
		std::println("[!] Could not create the socket!\n");
		WSACleanup();
		return false;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( this->m_port );
	InetPtonA(AF_INET, xbox_ip.c_str(), &serverAddr.sin_addr.s_addr);

	if (connect(this->m_client_fd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::println("[!] Failed to connect to the xbox!");
		WSACleanup();
		return false;
	}

	this->m_ip = xbox_ip;

	return true;
}

std::string tcp_socket::recv_message() const
{
	char       buffer[1024];
	const auto bytesReceived = recv(this->m_client_fd, buffer, sizeof(buffer), 0);

	if (bytesReceived < 0)
	{
		std::println("[!] Failed to receive message from xbox!");

		return {};
	}

	if (bytesReceived < 1024)
		buffer[bytesReceived] = '\0';
	else
		buffer[1023] = '\0';
		

	auto ret = std::string("");

	ret.append(buffer);

	return ret;
}

std::vector<std::string> tcp_socket::recv_multiline_response()
{
	std::string temp = {};

	bool finished = false;

	std::vector< std::string > ret;

	while (!finished)
	{
		temp = this->recv_message();

		// if last line sequence was found, I can stop receiving messages from the xbox
		if (temp.find("\r\n.") != std::string::npos || temp.find(".\r\n") != std::string::npos)
			finished = true;

		// remove the line ending sequences from the received messages
		if (temp.find("\r\n") != std::string::npos)
			temp.erase(std::remove_if(temp.begin(), temp.end(),
				[&](char ch)
				{ return std::iscntrl(static_cast<unsigned char>(ch)); }),
				temp.end());

		ret.push_back(temp);

		temp = {};
	}

	// remove last dot from multiline finish sequence
	ret.back().pop_back();

	return ret;
}

void tcp_socket::send_message(const std::string& msg) const
{
	send(this->m_client_fd, msg.c_str(), msg.size(), 0);
}

bool tcp_socket::release_socket()
{
	closesocket(this->m_client_fd);
	WSACleanup();

	return true;
}
