#include <memory>
#include <print>
#include <iostream>
#include <string>

#include "utils.hpp"
#include "xbox-api.hpp"

void lab()
{
    SetConsoleTitleA("cragson: cyphoon");

    constexpr auto XBOX_IP = "192.168.4.179";

    auto api = std::make_unique< xbox_api >();

    std::println("[*] Looking for {}:{}", XBOX_IP, api->get_xbdm_port());

    while (!api->setup(XBOX_IP))
        Sleep(420);

    std::println("[*] Succcessfully connected!\n");

    const auto info = api->get_system_info();
    std::println("[*] System: {}\n[*] Kernel: {}", info->System, info->Krnl);


    const auto xbe = api->get_xbeinfo();
    std::println("\n[*] Running: {}\n", xbe->name);


    const auto modules = api->get_modules();

    for( const auto& module : modules )
        if(module->name == "default.xex")
            std::println(
                "{}\n\tbase 0x{:08X}\n\tsize 0x{:08X}\n\tcheck: 0x{:08X}\n\ttimestamp: 0x{:08X}\n\tpdata: 0x{:08X}\n\tpsize: 0x{:08X}\n\tdllthread: 0x{:08X}\n\tthread: 0x{:08X}\n\tosize: 0x{:08X}\n",
                module->name,
                module->base,
                module->size,
                module->check,
                module->timestamp,
                module->pdata,
                module->psize,
                module->dllthread,
                module->thread,
                module->osize
            );


    const auto sections = api->get_sections_of_module("default.xex");

    for (const auto& section : sections)
        if(section->name == ".text")
            std::println("{}\n\tbase: 0x{:08X}\n\tsize: 0x{:08X}\n\tindex: {}\n\tflags: {}\n",
                section->name,
                section->base,
                section->size,
                section->index,
                section->flags
            );


    const auto bin_sig = api->read_word(0x82000000);
    std::println("[*] default.xex sig: {:04X}", bin_sig);

    const auto waw_points = api->read_dword(0x82A9B900);
    std::println("[*] CoD5 zombie points: {}", waw_points);

    if (api->write_dword(0x82A9B900, 133337))
        std::println("[*] Set points to 133337");

    api->hexdump_memory(0x82000000);

}

int main()
{
    lab();

    return 0;
}

