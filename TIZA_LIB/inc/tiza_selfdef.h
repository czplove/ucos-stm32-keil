#ifndef __TIZA_SELFDEF_H
#define __TIZA_SELFDEF_H

#include "tiza_include.h"

uint8 U8SumCheck(uint8 data[],uint16 len);
uint8 XorCheck(uint8 data[],uint16 len);
void RamClear(uint8 *ram_addr,uint16 len);
void RamZero(uint8 *ram_addr,uint16 len);
u16 CrcCheck(u8 *p_data,u16 len);
u16 Crc16Verify(u16 check, u8 *str, u16 len);
uint16 Crc16Check(uint8 const *str, uint16 len);
uint8 StrLen(const uint8 *str,uint8 max_count);
void LongTimeDly(uint32 dly);
uint16 SubMatch(uint8 sub[],uint8 sub_len,uint8 tar[],uint16 tar_len);
uint8 IsValidChar(uint8 data[],uint8 len);
uint8 IsValidAscii(uint8 data[],uint8 len);
uint8 IsValidNum(uint8 data[],uint8 len);
uint8 IsValidCharFloatNum(uint8 data[],uint8 len);
uint8 IsValidNumOrChar(uint8 data[],uint8 len);
uint8 IsValidNumOrCharOrDot(uint8 data[],uint8 len);
uint8 IsValidSimCardNum(uint8 data[]);
uint32 LittleBigSwap(uint32 val);
void MemCpy(uint8 dst[],uint8 src[],uint16 len);
uint8 MemCmp(uint8 dst[],uint8 src[],uint16 len);
uint8 AsciiToHexVal(uint8 h_b,uint8 l_b);
uint16 AsciiToHex(uint8 *p_src,uint16 len,uint8 *p_dst);
uint16 HexToAscii(uint8 *p_src,uint16 len,uint8 *p_dst);
uint32 U8ToUint32(uint8 data[]);
void Uint32ToU8(uint8 data[],uint32 val);
float U8ToFloat(uint8 data[]);
void FloatToU8(uint8 data[],float val);

#endif
