# cyphoon

Hi!

This is an API wrapper for the Xbox Debug Monitor (XBDM), written in modern C++.

It connects to your Xbox via a TCP socket and allows you to:
- Parse loaded modules
- Read and write memory
- Dump memory

And much more. This is achieved by wrapping up different commands of the XBDM. 

This is the current platform support:
- Windows (VS2022, works)
- Linux (GCC/Clang, compiles but is untested)

I want to give credits to the following projects and resources that helped me to learn about the XBDM and gain an overall understanding about Xbox 360 internals:
- [AngryCarrot789/MemoryEngine360](https://github.com/AngryCarrot789/MemoryEngine360)
- [XboxDevWiki](https://xboxdevwiki.net/Xbox_Debug_Monitor)
- [Experiment5X/XBDM](https://github.com/Experiment5X/XBDM)

Also make sure to check out my other projects on my [GitHub profile](https://github.com/cragson)!
Just a few examples for you:
 - [osmium - A Framework for Windows external cheats, written in modern C++](https://github.com/cragson/osmium)
 - [A wrapper for the mGBA Scripting API, which is written in C++ and Lua.](https://github.com/cragson/mgba-api)
 - [A C++ hacking framework for the Playstation 5 (PS5)](https://github.com/cragson/nihonium)
 - [A programming language made for Reverse Engineering and with Assembler in my heart.](https://github.com/cragson/TangoAway)


I appreciate every PR or constructive feedback, don't hesitate to reach out to me!

## Features

- Implemented commands of XBDM
    - `systeminfo`
    - `xbeinfo`
    - `modules`
    - `modsections`
    - `getmem`
    - `setmem`

- Wrapper API
    - Read memory (byte, word, dword)
    - Write memory (byte, word, dword)
    - Read memory block (n-bytes)
    - Write memory block (n-bytes)
    - Get list of all loaded modules (e.g. xboxkrnl.exe, default.xex)
    - Get list of all sections in loaded module (e.g. .text, .data)
    - Hexdump memory at address with n-bytes
    - Get system info (HDD, Type, Platform, System, BaseKrnl, Krnl, XDK)
    - Retreive info about running application (xbeinfo running)
    - Send custom command to Xbox
	
    ---
![showcase1](/res/showcase-1.png)

## Quick Start
1. Clone the repository
    - `git clone https://github.com/cragson/cyphoon`
2. Note IP of Xbox 360 running on BadUpdate / RGH
3. Use the following snippet as a template:
```cpp
void quickstart()
{
    // MAKE SURE TO USE THE IP OF YOUR XBOX HERE
    constexpr auto XBOX_IP = "192.168.4.179"; 

    auto api = std::make_unique< xbox_api >();

    while (!api->setup(XBOX_IP))
        Sleep(420);

    std::println("[*] Succcessfully connected!\n");

    const auto info = api->get_system_info();
    std::println("[*] System: {}\n[*] Kernel: {}", info->System, info->Krnl);
}
```
4. You're **done**, have fun experimenting!

Also have a look at the code examples below.

## Code Examples
Here are some code examples that should give you a quick orientation on how to do things with this project.
They should be up to date, but please always check for yourself.

### How to connect to your Xbox 360
```cpp
auto api = std::make_unique< xbox_api >();

constexpr auto XBOX_IP = "192.168.4.179";

while (!api->setup(XBOX_IP))
    Sleep(420);

std::println("[*] Succcessfully connected!\n");
```

### How to retreive `systeminfo`
```cpp
const auto info = api->get_system_info();

std::println("[*] System: {}\n[*] Kernel: {}", info->System, info->Krnl);
```

### How to retreive `xbeinfo`
```cpp
const auto xbe = api->get_xbeinfo();

std::println("\n[*] Running: {}\n", xbe->name);
```

### How to read from memory
```cpp
const auto waw_points = api->read_dword(0x82A9B900);

std::println("[*] CoD5 zombie points: {}", waw_points);
```

### How to write to memory
```cpp
if (api->write_dword(0x82A9B900, 133337))
    std::println("[*] Set points to 133337");
```

### How to hexdump a memory region
```cpp
api->hexdump_memory(0x82000000);
```

### How to retreive all loaded modules
```cpp
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
```

### How to retreive all sections in a loaded module
```cpp
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
```
