#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <format>
#include <Windows.h>
#include <consoleapi2.h>
#include <filesystem>
#include <d3d11.h>
#include <dxgi.h>
#include <stack>
#include <d3dcompiler.h>
#include <map>
#define deprecated(x)
#define __attribute__(x)
namespace fs = std::filesystem;
using namespace std::chrono;
using namespace std::chrono_literals;
// types
typedef unsigned long long u64;
typedef long long s64;
typedef unsigned long ul32;
typedef long sl32;
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;
typedef double f64;
typedef float f32;
struct data { u64 first, second; };
template <typename T>
using fptr = std::function<T>;
template <typename R, typename T, typename ...A>
using mfptr = R(T::*)(A...);
// globals
inline HMODULE g_main{ GetModuleHandleA(NULL) };
inline HMODULE g_entry{};
inline bool g_running{ true };