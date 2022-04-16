/**
 * @author Forrest Jablonski
 */

/*
 * This file should be included in every file of the project. It provides
 * standard declarations to be shared throughout.
 */

/*
 * For pragma doc
 * https://docs.microsoft.com/en-us/cpp/preprocessor/warning?view=vs-2019
 */

#pragma once

#if defined(_WIN32)

#pragma warning(disable: 26812) // stop bugging me about unscoped enums
#pragma warning(error:    4062) // switch on all enum values
#pragma warning(error:    4227) // reference of const should be pointer to const

#if defined(_DEBUG)
#define YM_DBG
#endif // _DEBUG

#endif // _WIN32

#if !defined(YM_DBG)
//#define YM_DBG
#endif // !YM_DBG

#define YM_NO_DEFAULT(        ClassName ) ClassName              (void)              = delete;
#define YM_NO_COPY(           ClassName ) ClassName              (ClassName const &) = delete;
#define YM_NO_ASSIGN(         ClassName ) ClassName & operator = (ClassName const &) = delete;
#define YM_NO_MOVE_CONSTRUCT( ClassName ) ClassName              (ClassName &&)      = delete;
#define YM_NO_MOVE_ASSIGN(    ClassName ) ClassName & operator = (ClassName &&)      = delete;

namespace ym
{

typedef char const *       str;

typedef unsigned char      uchar;

typedef signed char        int8;
typedef signed short       int16;
typedef signed int         int32;
typedef signed long long   int64;

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

typedef float              float32;
typedef double             float64;

static_assert(sizeof(int8 ) == 1, "int8 not 1 byte"  );
static_assert(sizeof(int16) == 2, "int16 not 2 bytes");
static_assert(sizeof(int32) == 4, "int32 not 4 bytes");
static_assert(sizeof(int64) == 8, "int64 not 8 bytes");

static_assert(sizeof(uint8 ) == 1, "uint8 not 1 byte"  );
static_assert(sizeof(uint16) == 2, "uint16 not 2 bytes");
static_assert(sizeof(uint32) == 4, "uint32 not 4 bytes");
static_assert(sizeof(uint64) == 8, "uint64 not 8 bytes");

static_assert(sizeof(float32) == 4, "float32 not 4 bytes");
static_assert(sizeof(float64) == 8, "float64 not 8 bytes");

} // ym
