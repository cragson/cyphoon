cmake_minimum_required (VERSION 3.8)
project(cyphoon)

# MSVC Hot Reload
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_executable(cyphoon src/main.cpp src/tcp-socket.cpp src/xbox-api.cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cyphoon PROPERTY CXX_STANDARD 23)
endif()
