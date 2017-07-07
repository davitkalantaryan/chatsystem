/*****************************************************************************
 * File			: ttypess.h
 * Created on	: 2017 Jun 15
 *****************************************************************************
 * Created by	: D.Kalantaryan, Tel:+49(0)33762/77552 kalantar
 * Email		: davit.kalantaryan@desy.de
 * Mail			: DESY, Platanenallee 6, 15738 Zeuthen
 *****************************************************************************
 * Description
 *   Types with exact size defined here
 ****************************************************************************/
#ifndef __ttypess_h__
#define __ttypess_h__


#ifndef uint8_ttt_defined
typedef unsigned char uint8_ttt;
#define uint8_ttt_defined
#endif // #ifndef uint8_ttt_defined

#ifndef int8_ttt_defined
typedef char int8_ttt;
#define int8_ttt_defined
#endif // #ifndef int8_ttt_defined

#ifndef uint16_ttt_defined
typedef unsigned short int uint16_ttt;
#define uint16_ttt_defined
#endif // #ifndef uint16_ttt_defined

#ifndef int16_ttt_defined
typedef short int int16_ttt;
#define int16_ttt_defined
#endif // #ifndef int16_ttt_defined

#ifndef uint32_ttt_defined
typedef unsigned int uint32_ttt;
#define uint32_ttt_defined
#endif // #ifndef uint32_ttt_defined

#ifndef int32_ttt_defined
typedef int int32_ttt;
#define int32_ttt_defined
#endif // #ifndef int32_ttt_defined

#ifndef uint64_ttt_defined
#ifdef _MSC_VER
typedef unsigned __int64 uint64_ttt;
#else
typedef unsigned long long int uint64_ttt;
#endif
#define uint64_ttt_defined
#endif // #ifndef uint64_ttt_defined

#ifndef int64_ttt_defined
#ifdef _MSC_VER
typedef __int64 int64_ttt;
#else
typedef long long int int64_ttt;
#endif
#define int64_ttt_defined
#endif // #ifndef int64_ttt_defined

#endif  // #ifndef __ttypess_h__
