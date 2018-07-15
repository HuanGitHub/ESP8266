#ifndef __EDP_KIT_H__
#define __EDP_KIT_H__

#ifdef EDPKIT_EXPORTS
#define EDPKIT_DLL __declspec(dllexport)
#else
#define EDPKIT_DLL
#endif

#include "Common.h"
#include "cJSON.h"


/*---------------------------------------------------------------------------*/
#define MOSQ_MSB(A)         (uint8)((A & 0xFF00) >> 8)
#define MOSQ_LSB(A)         (uint8)(A & 0x00FF)
#define BUFFER_SIZE         (0x01<<10) 
#define PROTOCOL_NAME       "EDP"
#define PROTOCOL_VERSION    1

#define MAX_FLOAT_DPS_COUNT 1000
/*----------------------------错误码-----------------------------------------*/
#define ERR_UNPACK_CONNRESP_REMAIN              -1000
#define ERR_UNPACK_CONNRESP_FLAG                -1001
#define ERR_UNPACK_CONNRESP_RTN                 -1002
#define ERR_UNPACK_PUSHD_REMAIN                 -1010
#define ERR_UNPACK_PUSHD_DEVID                  -1011
#define ERR_UNPACK_PUSHD_DATA                   -1012
#define ERR_UNPACK_SAVED_REMAIN                 -1020
#define ERR_UNPACK_SAVED_TANSFLAG               -1021
#define ERR_UNPACK_SAVED_DEVID                  -1022
#define ERR_UNPACK_SAVED_DATAFLAG               -1023
#define ERR_UNPACK_SAVED_JSON                   -1024
#define ERR_UNPACK_SAVED_PARSEJSON              -1025
#define ERR_UNPACK_SAVED_BIN_DESC               -1026
#define ERR_UNPACK_SAVED_PARSEDESC              -1027
#define ERR_UNPACK_SAVED_BINLEN                 -1028
#define ERR_UNPACK_SAVED_BINDATA                -1029
#define ERR_UNPACK_PING_REMAIN                  -1030
#define ERR_UNPACK_CMDREQ                       -1031
#define ERR_UNPACK_ENCRYPT_RESP                 -1032
#define ERR_UNPACK_SAVEDATA_ACK                 -1033
#define ERR_UNPACK_SAVED_STR_WITH_TIME		    -1034
#define ERR_UNPACK_SAVED_FLOAT_WITH_TIME        -1035
#define ERR_UNPACK_UPDATE_RESP                  -1040
/*----------------------------消息类型---------------------------------------*/
/* 连接请求 */
#define CONNREQ             0x10
/* 连接响应 */
#define CONNRESP            0x20
/* 转发(透传)数据 */
#define PUSHDATA            0x30
/* 升级请求 */
#define UPDATEREQ           0x50
/* 升级响应 */
#define UPDATERESP          0x60
/* 存储(转发)数据 */
#define SAVEDATA            0x80
/* 存储确认 */
#define SAVEACK             0x90
/* 命令请求 */
#define CMDREQ              0xA0
/* 命令响应 */
#define CMDRESP             0xB0
/* 心跳请求 */
#define PINGREQ             0xC0
/* 心跳响应 */
#define PINGRESP            0xD0
/* 加密请求 */
#define ENCRYPTREQ          0xE0
/* 加密响应 */
#define ENCRYPTRESP         0xF0

#define UPDATE  0x50

#ifndef NULL
#define NULL (void*)0
#endif

/* SAVEDATA消息支持的格式类型 */
typedef enum {
    kTypeFullJson = 0x01,
    kTypeBin = 0x02,
    kTypeSimpleJsonWithoutTime = 0x03,
    kTypeSimpleJsonWithTime = 0x04,
    kTypeString = 0x05,
    kTypeStringWithTime = 0x06,
    kTypeFloatWithTime  = 0x07
}SaveDataType;




/*-------------发送buffer, 接收buffer, EDP包结构定义-------------------------*/
EDPKIT_DLL
typedef struct Buffer
{
uint8*  _data;          /* buffer数据 */
uint32  _write_pos;     /* buffer写入位置 */
uint32  _read_pos;      /* buffer读取位置 */
uint32  _capacity;      /* buffer容量 */
}Buffer, SendBuffer, RecvBuffer, EdpPacket;
/*-----------------------------操作Buffer的接口------------------------------*/
/* 
 * 函数名:  NewBuffer
 * 功能:    生成Buffer
 * 说明:    一般情况下, NewBuffer和DeleteBuffer应该成对出现
 * 参数:    无
 * 返回值:  类型 (Buffer*)
 *          返回值非空 生成Buffer成功, 返回这个Buffer的指针
 *          返回值为空 生成Buffer失败, 内存不够
 */
EDPKIT_DLL
Buffer* NewBuffer();
/* 
 * 函数名:  DeleteBuffer
 * 功能:    销毁Buffer
 * 说明:    一般情况下, NewBuffer和DeleteBuffer应该成对出现
 * 参数:    buf     一个Buffer的指针的指针
 * 返回值:  无
 */
EDPKIT_DLL
void DeleteBuffer(Buffer** buf);
/* 
 * 函数名:  CheckCapacity
 * 功能:    检查Buffer是否能够写入长度为len的字节流, 
 *          如果Buffer的容量不够, 自动成倍扩展Buffer的容量(不影响Buffer数据)
 * 参数:    buf     需要写入Buffer的指针
 *          len     期望写入的长度
 * 返回值:  类型 (int32)
 *          <0      失败, 内存不够
 *          =0      成功
 */

EDPKIT_DLL
int32 ReadByte(EdpPacket* pkg, uint8* val);
/* 
 * 函数名:  ReadBytes
 * 功能:    按EDP协议, 从Buffer(包)中读取count个字节数据
 * 说明:    val是malloc出来的, 需要客户端自己free 
 * 参数:    pkg     EDP包
 *          val     数据(count个字节)
 *          count   字节数
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 ReadBytes(EdpPacket* pkg, uint8** val, uint32 count);
/* 
 * 函数名:  ReadUint16
 * 功能:    按EDP协议, 从Buffer(包)中读取uint16值
 * 参数:    pkg     EDP包
 *          val     uint16值
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 ReadUint16(EdpPacket* pkg, uint16* val);
/* 
 * 函数名:  ReadUint32
 * 功能:    按EDP协议, 从Buffer(包)中读取uint32值
 * 参数:    pkg     EDP包
 *          val     uint32值
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 ReadUint32(EdpPacket* pkg, uint32* val);

/* 
 * 函数名:  ReadFloat
 * 功能:    按EDP协议, 从Buffer(包)中读取float值
 * 参数:    pkg     EDP包
 *                 val     float值,此处假定机器为小端(高字节在前，低字节在后)
 *                          若为大端机器，必须将读出的float四字节颠倒顺序后再使用
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 ReadFloat(EdpPacket* pkg, float* val);
/* 
 * 函数名:  ReadStr
 * 功能:    按EDP协议, 从Buffer(包)中读取字符串, 以\0结尾
 * 参数:    pkg     EDP包
 *          val     字符串
 * 说明:    val是malloc出来的, 需要客户端自己free 
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 ReadStr(EdpPacket* pkg, char** val);


/* 
 * 函数名:  ReadRemainlen
 * 功能:    按EDP协议, 从Buffer(包)中remainlen
 * 说明:    remainlen是EDP协议中的概念, 是一个EDP包身的长度
 * 参数:    pkg     EDP包
 *          len_val remainlen
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 ReadRemainlen(EdpPacket* pkg, uint32* len_val);

/*------------------------数据写入EDP包的接口-------------------------------*/
/*
 * 说明:    目前不支持一个包即在写入又在读取, 因此, 只有对于_read_pos为0的包才能被写入
 */
/* 
 * 函数名:  WriteByte
 * 功能:    按EDP协议, 将一个字节数据写入Buffer(包)中
 * 参数:    pkg     EDP包
 *          byte    数据(一个字节)
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 WriteByte(Buffer* buf, uint8 byte);
/* 
 * 函数名:  WriteBytes
 * 功能:    按EDP协议, 将count个字节数据写入Buffer(包)中
 * 参数:    pkg     EDP包
 *          bytes   数据
 *          count   字节数
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 WriteBytes(Buffer* buf, const void* bytes, uint32 count);
/* 
 * 函数名:  WriteUint16
 * 功能:    按EDP协议, 将uint16写入Buffer(包)中
 * 参数:    pkg     EDP包
 *          val     uint16数据
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 WriteUint16(Buffer* buf, uint16 val);
/* 
 * 函数名:  WriteUint32
 * 功能:    按EDP协议, 将uint32写入Buffer(包)中
 * 参数:    pkg     EDP包
 *          val     uint32数据
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 WriteUint32(Buffer* buf, uint32 val);

/* 
 * 函数名:  WriteFloat
 * 功能:    按EDP协议, 将一个float值写入Buffer中
 * 参数:    pkg     EDP包
 *                 val     float值,此处假定机器为小端(高字节在前，低字节在后)
 *                          若为大端机器，必许float四字节颠倒顺序后再存储
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 WriteFloat(EdpPacket* pkg, float val);


/* 
 * 函数名:  WriteStr
 * 功能:    按EDP协议, 将字符串写入Buffer(包)中
 * 参数:    pkg     EDP包
 *          val     字符串
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 WriteStr(Buffer* buf, const char *str);
/* 
 * 函数名:  WriteRemainlen 
 * 功能:    按EDP协议, 将remainlen写入Buffer(包)中
 * 说明:    remainlen是EDP协议中的概念, 是一个EDP包身的长度
 * 参数:    pkg     EDP包
 *          len_val remainlen
 * 返回值:  类型 (int32)
 *          <0      失败, pkg中无数据
 *          =0      成功
 */
EDPKIT_DLL
int32 WriteRemainlen(Buffer* buf, uint32 len_val);




/* 
 * 函数名:  PacketConnect1 
 * 功能:    打包 由设备到设备云的EDP协议包, 连接设备云的请求(登录认证方式1)
 * 说明:    返回的EDP包发送给设备云后, 需要客户程序删除该包
 *          设备云会回复连接响应给设备
 * 相关函数:UnpackConnectResp
 * 参数:    devid       设备ID, 申请设备时平台返回的ID
 *          auth_key    鉴权信息(api-key), 在平台申请的可以操作该设备的api-key字符串
 * 返回值:  类型 (EdpPacket*) 
 *          非空        EDP协议包
 *          为空        EDP协议包生成失败 
 */
EDPKIT_DLL
EdpPacket* PacketConnect1(const char* devid, const char* auth_key);


int32 CheckCapacity(Buffer* buf, uint32 len);



#endif /* __EDP_KIT_H__ */

