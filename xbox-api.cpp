#include "xbox-api.hpp"

#include <print>
#include <algorithm>
#include <set>
#include <regex>

#include "utils.hpp"

bool xbox_api::setup(const std::string& xbox_ip)
{
    if (!this->m_socket->connect_to_xbox(xbox_ip))
        return false;

    const auto welcome_msg = this->m_socket->recv_message();

    std::println("[+] Xbox Welcome: {}", welcome_msg);

    return true;
}

void xbox_api::send_raw_command_to_xbox(const std::string& cmd)
{
    const auto prepared_cmd = cmd + "\r\n";

    this->m_socket->send_message(prepared_cmd);
}

std::unique_ptr<xbox_systeminfo> xbox_api::get_system_info()
{
    auto ret = std::make_unique< xbox_systeminfo >();

    this->send_raw_command_to_xbox("systeminfo");

    auto resp = this->m_socket->recv_multiline_response();

    auto conc_resp = Utils::join_str_vec(resp);

    // 202- multiline response followsHDD=EnabledType=DevKitPlatform=Waternoose System=TrinityBaseKrnl=2.0.1888.0 Krnl=2.0.17559.0 XDK=2.0.21076.11

    const auto hdd_pos_1 = conc_resp.find("HDD=");
    const auto hdd_pos_2 = conc_resp.find("Type=", hdd_pos_1);
    const auto hdd = conc_resp.substr(hdd_pos_1 + 4, hdd_pos_2 - hdd_pos_1 - 4);

    const auto type_pos_1 = conc_resp.find("Platform=", hdd_pos_2);
    const auto type = conc_resp.substr(hdd_pos_2 + 5, type_pos_1 - hdd_pos_2 - 5);

    const auto platform_pos_1 = conc_resp.find("System=", type_pos_1);
    const auto platform = conc_resp.substr(type_pos_1 + 9, platform_pos_1 - type_pos_1 - 9);

    const auto system_pos_1 = conc_resp.find("BaseKrnl=", platform_pos_1);
    const auto system = conc_resp.substr(platform_pos_1 + 7, system_pos_1 - platform_pos_1 - 7);

    const auto basekrnl_pos_1 = conc_resp.find("Krnl=", system_pos_1 + 9);
    const auto basekrnl = conc_resp.substr(system_pos_1 + 9, basekrnl_pos_1 - system_pos_1 - 9);

    const auto krnl_pos_1 = conc_resp.find("XDK=", basekrnl_pos_1);
    const auto krnl = conc_resp.substr(basekrnl_pos_1 + 5, krnl_pos_1 - basekrnl_pos_1 - 5);

    const auto xdk = conc_resp.substr(krnl_pos_1 + 4);

    ret->HDD = hdd;
    ret->Type = type;
    ret->Platform = platform;
    ret->System = system;
    ret->BaseKrnl = basekrnl;
    ret->Krnl = krnl;
    ret->XDK = xdk;

    return ret;
}

std::unique_ptr<xbox_xbeinfo> xbox_api::get_xbeinfo()
{
    auto ret = std::make_unique< xbox_xbeinfo >();

    this->send_raw_command_to_xbox("xbeinfo running");

    auto resp = this->m_socket->recv_multiline_response();

    auto conc_resp = Utils::join_str_vec(resp);

    // 202- multiline response followstimestamp=0x00000000 checksum=0x00000000name=\"\\Device\\Mass1\\games\\halo3\\default.xex\"

    const auto timestamp_pos1 = conc_resp.find("timestamp=");
    const auto timestamp_pos2 = conc_resp.find(" ", timestamp_pos1);
    const auto timestamp = conc_resp.substr(timestamp_pos1 + 10, timestamp_pos2 - timestamp_pos1 - 10);

    const auto checksum_pos1 = conc_resp.find("checksum=", timestamp_pos2);
    const auto checksum_pos2 = conc_resp.find("name=", checksum_pos1);
    const auto checksum = conc_resp.substr(checksum_pos1 + 9, checksum_pos2 - checksum_pos1 - 9 );

    auto name = conc_resp.substr(checksum_pos2 + 5);
    name.erase(std::remove(name.begin(), name.end(), '"'));
    name.pop_back();

    ret->timestamp = Utils::str_to_ul( timestamp );
    ret->checksum = Utils::str_to_ul(checksum);
    ret->name = name;

    return ret;
}

std::vector<std::unique_ptr<xbox_module>> xbox_api::get_modules()
{
    std::vector< std::unique_ptr< xbox_module > > ret = {};

    this->send_raw_command_to_xbox("modules");

    auto resp = this->m_socket->recv_multiline_response();

    auto conc_resp = Utils::join_str_vec(resp);
   
    // parse now all modules out of concatenated string.. fml

    std::string mask = {};

    for( int32_t idx = 0; idx < conc_resp.size(); idx++ )
    {
        if (idx + 3 >= conc_resp.size())
            break;

        mask += conc_resp.at(idx);
        mask += conc_resp.at(idx + 1);
        mask += conc_resp.at(idx + 2);
        mask += conc_resp.at(idx + 3);

        // module entry found
        if (mask == "name")
        {
            // name="xboxkrnl.exe" base=0x80040000 size=0x001b0000 check=0x0016b651 timestamp=0x5daa916b pdata=0x8004fa00 psize=0x000063a0 thread=0x00000000 osize=0x001b0000

            const auto module_name_pos_1 = conc_resp.find('="', idx);
            const auto module_name_pos_2 = conc_resp.find('"', module_name_pos_1 + 2);
            const auto module_name = conc_resp.substr(module_name_pos_1 + 1, module_name_pos_2 - module_name_pos_1 - 1);       

            const auto base_pos_1 = conc_resp.find("base=", module_name_pos_2);
            const auto base_pos_2 = conc_resp.find(" ", base_pos_1);
            const auto base = conc_resp.substr(base_pos_1 + 5, base_pos_2 - base_pos_1 - 5);

            const auto size_pos_1 = conc_resp.find("size=", base_pos_2);
            const auto size_pos_2 = conc_resp.find(" ", size_pos_1);
            const auto size = conc_resp.substr(size_pos_1 + 5, size_pos_2 - size_pos_1 - 5);

            const auto check_pos_1 = conc_resp.find("check=", size_pos_2);
            const auto check_pos_2 = conc_resp.find(" ", check_pos_1);
            const auto check = conc_resp.substr(check_pos_1 + 6, check_pos_2 - check_pos_1 - 6);

            const auto timestamp_pos_1 = conc_resp.find("timestamp=", check_pos_2);
            const auto timestamp_pos_2 = conc_resp.find(" ", timestamp_pos_1);
            const auto timestamp = conc_resp.substr(timestamp_pos_1 + 10, timestamp_pos_2 - timestamp_pos_1 - 10);

            const auto pdata_pos_1 = conc_resp.find("pdata=", timestamp_pos_2);
            const auto pdata_pos_2 = conc_resp.find(" ", pdata_pos_1);
            const auto pdata = conc_resp.substr(pdata_pos_1 + 6, pdata_pos_2 - pdata_pos_1 - 6);

            const auto psize_pos_1 = conc_resp.find("psize=", pdata_pos_2);
            const auto psize_pos_2 = conc_resp.find(" ", psize_pos_1);
            const auto psize = conc_resp.substr(psize_pos_1 + 6, psize_pos_2 - psize_pos_1 - 6);

            size_t dllthread_pos_1 = -1;
            size_t dllthread_pos_2 = -1;
            std::string dllthread = {};

            size_t thread_pos_1 = -1;
            size_t thread_pos_2 = -1;
            std::string thread = {};

            if (module_name == "xboxkrnl.exe" || module_name == "default.xex")
            {
                thread_pos_1 = conc_resp.find("thread=", psize_pos_2);
                thread_pos_2 = conc_resp.find(" ", thread_pos_1);

                thread = conc_resp.substr(thread_pos_1 + 7, thread_pos_2 - thread_pos_1 - 7);
            }
            else
            {
                dllthread_pos_1 = conc_resp.find("dllthread=", psize_pos_2);
                dllthread_pos_2 = conc_resp.find(" ", dllthread_pos_1);

                dllthread = conc_resp.substr(dllthread_pos_1 + 10, dllthread_pos_2 - dllthread_pos_1 - 10);
            }
            
            const auto osize_pos_1 = conc_resp.find("osize=", dllthread_pos_2 != -1 ? dllthread_pos_2 : thread_pos_2);
            const auto osize_pos_2 = conc_resp.find("name=", osize_pos_1 + 6);
            const auto osize = conc_resp.substr(osize_pos_1 + 6, 9);


            auto temp_module = std::make_unique< xbox_module >();

            temp_module->name = module_name;
            temp_module->base = Utils::str_to_ul(base);
            temp_module->size = Utils::str_to_ul(size);
            temp_module->check = Utils::str_to_ul(check);
            temp_module->timestamp = Utils::str_to_ul(timestamp);
            temp_module->pdata = Utils::str_to_ul(pdata);
            temp_module->psize = Utils::str_to_ul(psize);
            temp_module->dllthread = !dllthread.empty() ? Utils::str_to_ul(dllthread) : 0x0;
            temp_module->thread = !thread.empty() ? Utils::str_to_ul(thread) : 0x0;
            temp_module->osize = Utils::str_to_ul(osize);

            ret.push_back(std::move(temp_module));
        }

        mask = "";
    }

    return ret;
}

std::vector < std::unique_ptr<xbox_module_section> > xbox_api::get_sections_of_module(const std::string& module_name)
{
    std::vector < std::unique_ptr< xbox_module_section > > ret = {};

    const auto cmd = std::format("modsections name=\"{}\"", module_name);

    this->send_raw_command_to_xbox( cmd );

    auto resp = this->m_socket->recv_multiline_response();

    const auto conc_resp = Utils::join_str_vec(resp);

    // 202- multiline response followsname=".rdata" base=0x82000400 size=0x0005d480 index=1 flags=2name=".pdata" base=0x8205da00 size=0x0001eeb8 index=2 flags=2name="BINKBSS" base=0x8207ca00 size=0x000028f8 index=3 flags=2name=".text" base=0x82080000 size=0x0068e554 index=4 flags=10name="BINK" base=0x8270e600 size=0x0001089c index=5 flags=10name=".data" base=0x82720000 size=0x00abb518 index=6 flags=6name=".XBMOVIE" base=0x831db600 size=0x0000000c index=7 flags=6name="BINKDATA" base=0x831db800 size=0x00003d88 index=8 flags=6name=".tls" base=0x831df600 size=0x000002e9 index=9 flags=6name=".idata" base=0x831e0000 size=0x00000526 index=10 flags=6name=".XBLD" base=0x831f0000 size=0x000000d0 index=11 flags=2name=".reloc" base=0x831f0200 size=0x00052474 index=12 flags=2

    // parse now the sections of the module from the response

    std::string mask = {};

    for (int32_t idx = 0; idx < conc_resp.size(); idx++)
    {
        if (idx + 3 >= conc_resp.size())
            break;

        mask += conc_resp.at(idx);
        mask += conc_resp.at(idx + 1);
        mask += conc_resp.at(idx + 2);
        mask += conc_resp.at(idx + 3);

        // section entry found
        if (mask == "name")
        {           
            const auto name_pos_1 = conc_resp.find('="', idx);
            const auto name_pos_2 = conc_resp.find('"', name_pos_1 + 2);
            const auto name = conc_resp.substr(name_pos_1 + 1, name_pos_2 - name_pos_1 - 1);

            const auto base_pos_1 = conc_resp.find("base=", name_pos_2);
            const auto base_pos_2 = conc_resp.find(" ", base_pos_1);
            const auto base = conc_resp.substr(base_pos_1 + 5, base_pos_2 - base_pos_1 - 5);

            const auto size_pos_1 = conc_resp.find("size=", base_pos_2);
            const auto size_pos_2 = conc_resp.find(" ", size_pos_1);
            const auto size = conc_resp.substr(size_pos_1 + 5, size_pos_2 - size_pos_1 - 5);

            const auto index_pos_1 = conc_resp.find("index=", size_pos_2);
            const auto index_pos_2 = conc_resp.find(" ", index_pos_1);
            const auto index = conc_resp.substr(index_pos_1 + 6, index_pos_2 - index_pos_1 - 6);

            const auto flags_pos_1 = conc_resp.find("flags=", index_pos_2);
            const auto flags_pos_2 = conc_resp.find(" ", flags_pos_1);
            const auto flags = conc_resp.substr(flags_pos_1 + 6, flags_pos_2 - flags_pos_1 - 6);

            auto temp_section = std::make_unique< xbox_module_section >();

            temp_section->name = name;
            temp_section->base = Utils::str_to_ul(base);
            temp_section->size = Utils::str_to_ul(size);
            temp_section->index = Utils::str_to_ul(index);
            temp_section->flags = Utils::str_to_ul(flags);

            ret.push_back(std::move(temp_section));
        }

        mask = "";
    }

    return ret;
}

std::vector<uint8_t> xbox_api::read_memory_block(const std::uintptr_t address, const size_t size)
{
    std::vector< uint8_t > ret = {};

    const auto cmd = std::format( "getmem addr=0x{:08X} length=0x{:08X}", address, size );

    this->send_raw_command_to_xbox(cmd);

    auto resp = this->m_socket->recv_multiline_response();

    const auto conc_resp = Utils::join_str_vec(resp);

    // 202- memory data follows4D5A

    const auto bytes = conc_resp.substr(conc_resp.find("follows") + 7);

    ret = Utils::convert_array_str_to_bytes(bytes);

    return ret;
}

uint8_t xbox_api::read_byte(const std::uintptr_t address)
{
    const auto bytes = this->read_memory_block(address, 1);

    return bytes.front();
}

uint16_t xbox_api::read_word(const std::uintptr_t address)
{
    auto bytes = this->read_memory_block(address, 2);

    const auto ret = *reinterpret_cast<uint16_t*>(bytes.data());

    return ret;
}

uint32_t xbox_api::read_dword(const std::uintptr_t address)
{
    auto bytes = this->read_memory_block(address, 4);

    const uint32_t ret = bytes.at(0) << 24 | bytes.at(1) << 16 | bytes.at(2) << 8 | bytes.at(3);

    return ret;
}

bool xbox_api::write_memory_block(const std::uintptr_t address, const std::vector<uint8_t>& bytes)
{
    const auto bytes_str = Utils::convert_bytes_vector_to_str(bytes);

    const auto cmd = std::format("setmem addr=0x{:08X} data={}", address, bytes_str);

    this->send_raw_command_to_xbox(cmd);

    auto resp = this->m_socket->recv_message();

    return resp.contains("200-");
}

bool xbox_api::write_byte(const std::uintptr_t address, const uint8_t byte)
{
    return this->write_memory_block( 
        address, 
        { 
            0x0, 0x0, 0x0, byte 
        } 
    );
}

bool xbox_api::write_word(const std::uintptr_t address, const uint16_t word)
{
    const uint8_t b0 = (word & 0xFF00) >> 8;
    const uint8_t b1 = (word & 0x00FF);
   
    return this->write_memory_block(
        address,
        {
            0x0, 0x0, b0, b1
        }
    );
}

bool xbox_api::write_dword(const std::uintptr_t address, const uint32_t dword)
{
    const uint8_t b0 = ( dword & 0xFF000000) >> 24;
    const uint8_t b1 = ( dword & 0x00FF0000) >> 16;
    const uint8_t b2 = ( dword & 0x0000FF00) >> 8;
    const uint8_t b3 = ( dword & 0x000000FF);

    return this->write_memory_block(
        address,
        {
            b0, b1, b2, b3
        }
    );
}

void xbox_api::hexdump_memory(const std::uintptr_t& address, const size_t size)
{
    auto buffer = this->read_memory_block(address, size);

    if (buffer.size() != size)
    {
        std::println("[!] Failed to dump memory buffer correctly!");

        return;
    }

    uint32_t counter = 0;

    auto temp_buffer = std::vector< uint8_t >();

    for (const auto& bt : buffer )
    {
        counter++;

        temp_buffer.push_back(bt);

        if (counter % 8 == 0 && counter > 0)
        {
            const auto b0 = temp_buffer.at(0);
            const auto b1 = temp_buffer.at(1);
            const auto b2 = temp_buffer.at(2);
            const auto b3 = temp_buffer.at(3);
            const auto b4 = temp_buffer.at(4);
            const auto b5 = temp_buffer.at(5);
            const auto b6 = temp_buffer.at(6);
            const auto b7 = temp_buffer.at(7);

            const auto dump = std::format(
                "[{:X}] {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {}{}{}{}{}{}{}{}",
                    address + counter - temp_buffer.size(),
                    b0,
                    b1,
                    b2,
                    b3,
                    b4,
                    b5,
                    b6,
                    b7,
                    std::isprint(b0) ? static_cast<char>(b0) : '.',
                    std::isprint(b1) ? static_cast<char>(b1) : '.',
                    std::isprint(b2) ? static_cast<char>(b2) : '.',
                    std::isprint(b3) ? static_cast<char>(b3) : '.',
                    std::isprint(b4) ? static_cast<char>(b4) : '.',
                    std::isprint(b5) ? static_cast<char>(b5) : '.',
                    std::isprint(b6) ? static_cast<char>(b6) : '.',
                    std::isprint(b7) ? static_cast<char>(b7) : '.'
                );

            std::println("{}", dump);

            temp_buffer.clear();
        }
        
    }

    std::println("");
}
