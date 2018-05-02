#pragma once

#define COUNT(x) (sizeof(x) / sizeof(*(x)))
#define FORCEINLINE __forceinline

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long int i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long int u64;
typedef float f32;
typedef double f64;
typedef i32 b32;

typedef wchar_t WCHAR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef int BOOL;
