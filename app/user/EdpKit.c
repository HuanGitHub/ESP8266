#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "EdpKit.h"
#include "include.h"


/*---------------------------------------------------------------------------*/
Buffer* NewBuffer()
{
    Buffer* buf = (Buffer*)os_zalloc(sizeof(Buffer));
    buf->_data = (uint8*)os_zalloc(sizeof(uint8) * BUFFER_SIZE);
    buf->_write_pos = 0;
    buf->_read_pos = 0;
    buf->_capacity = BUFFER_SIZE;
    return buf;
}
void DeleteBuffer(Buffer** buf)
{
    uint8* pdata = (*buf)->_data;
    os_free(pdata);
    os_free(*buf);
    *buf = NULL;
}

/*---------------------------------------------------------------------------*/
int32 ReadByte(EdpPacket* pkg, uint8* val)
{
    if (pkg->_read_pos+1 > pkg->_write_pos) 
        return -1;
    *val = pkg->_data[pkg->_read_pos];
    pkg->_read_pos += 1;
    return 0;
}
int32 ReadBytes(EdpPacket* pkg, uint8** val, uint32 count)
{
    if (pkg->_read_pos+count > pkg->_write_pos) 
        return -1;
    *val = (uint8*)os_zalloc(sizeof(uint8) * count);
    memcpy(*val, pkg->_data + pkg->_read_pos, count);
    pkg->_read_pos += count;
    return 0;
}
int32 ReadUint16(EdpPacket* pkg, uint16* val)
{
    uint8 msb, lsb;
    if (pkg->_read_pos+2 > pkg->_write_pos) 
        return -1;
    msb = pkg->_data[pkg->_read_pos];
    pkg->_read_pos++;
    lsb = pkg->_data[pkg->_read_pos];
    pkg->_read_pos++;
    *val = (msb<<8) + lsb;
    return 0;
}
int32 ReadUint32(EdpPacket* pkg, uint32* val)
{
    int32 i = 0;
    uint32 tmpval = 0;
    if (pkg->_read_pos+4 > pkg->_write_pos) 
        return -1;
    while (i++ < 4) {
        tmpval = (tmpval << 8) | (pkg->_data[pkg->_read_pos]);
        pkg->_read_pos++;
    }
    *val = tmpval;
    return 0;
}

int32 ReadFloat(EdpPacket* pkg, float* val)
{
    *val = 0;
    if (pkg->_read_pos + 4 > pkg->_write_pos)
        return -1;

    /*此处假设机器为小端;
        若为大端，memcpy后需颠倒float的四个字节顺序*/
    memcpy(val, pkg->_data + pkg->_read_pos, sizeof(float));
    pkg->_read_pos += 4;
    return 0;
}

int32 ReadStr(EdpPacket* pkg, char** val)
{
    uint16 len = 0;
    int rc = 0;
    /* read str len */
    rc = ReadUint16(pkg, &len);
    if (rc) 
        return rc;
    if (pkg->_read_pos+len > pkg->_write_pos) 
        return -1;
    /* copy str val */
    *val = (char*)malloc(sizeof(char) * (len + 1));
    memset(*val, 0, len+1);
    strncpy(*val, (const char *)(pkg->_data + pkg->_read_pos), len);
    pkg->_read_pos += len;
    return 0;
}



int32 ReadRemainlen(EdpPacket* pkg, uint32* len_val)
{
    uint32 multiplier = 1;
    uint32 len_len = 0;
    uint8 onebyte = 0;
    int32 rc;
    *len_val = 0;
    do {
        rc = ReadByte(pkg, &onebyte);
        if (rc) 
            return rc;

        *len_val += (onebyte & 0x7f) * multiplier;
        multiplier *= 0x80;

        len_len++;
        if (len_len > 4) {
            return -1;/*len of len more than 4;*/
        }
    } while((onebyte & 0x80) != 0);
    return 0;
}
/*---------------------------------------------------------------------------*/
int32 WriteByte(Buffer* buf, uint8 byte)
{
//    assert(buf->_read_pos == 0);

    buf->_data[buf->_write_pos] = byte;
    buf->_write_pos++;
    return 0;
}
int32 WriteBytes(Buffer* buf, const void* bytes, uint32 count)
{
 
    os_memcpy(buf->_data + buf->_write_pos, bytes, count);
    buf->_write_pos += count;
    return 0;
}
int32 WriteUint16(Buffer* buf, uint16 val)
{

    return WriteByte(buf, MOSQ_MSB(val)) 
        || WriteByte(buf, MOSQ_LSB(val));
}
int32 WriteUint32(Buffer* buf, uint32 val)
{
    
    return WriteByte(buf, (val >> 24) & 0x00FF) 
        || WriteByte(buf, (val >> 16) & 0x00FF)
        || WriteByte(buf, (val >> 8) & 0x00FF) 
        || WriteByte(buf, (val) & 0x00FF);
}

int32 WriteFloat(Buffer* buf, float val)
{

   

   /*此处假设机器为小端;
        若为大端，需先颠倒float的四个字节顺序再writebytes*/

    return WriteBytes(buf, &val, sizeof(float));
}

int32 WriteStr(Buffer* buf, const char *str)
{
    uint16 length = 0;

    length = os_strlen(str);
    return WriteUint16(buf, length) 
        || WriteBytes(buf, str, length);
}
int32 WriteRemainlen(Buffer* buf, uint32 len_val)
{
    uint32 remaining_length = len_val;
    int32 remaining_count = 0;
    uint8 byte = 0;

    
    do {
        byte = remaining_length % 128;
        remaining_length = remaining_length / 128;
        /* If there are more digits to encode, set the top bit of this digit */
        if (remaining_length > 0) {
            byte = byte | 0x80;
        }
        buf->_data[buf->_write_pos++] = byte;
        remaining_count++;
    }while(remaining_length > 0 && remaining_count < 5);
    
    return 0;
}
/*---------------------------------------------------------------------------*/
/* connect1 (C->S): devid + apikey */
EdpPacket* PacketConnect1(const char* devid, const char* auth_key)
{
    EdpPacket* pkg = NewBuffer();
    uint32 remainlen;
	/* msg type */
    WriteByte(pkg, CONNREQ);
	remainlen = (2+3)+1+1+2+(2+strlen(devid))+(2+strlen(auth_key));
	WriteRemainlen(pkg, remainlen);
	/* protocol desc */
	WriteStr(pkg, PROTOCOL_NAME);
	/* protocol version */
	WriteByte(pkg, PROTOCOL_VERSION);
	/* connect flag */
	WriteByte(pkg, 0x40);
	/* keep time */
    WriteUint16(pkg, 300);
	/* DEVID */
	WriteStr(pkg, devid);
	/* auth key */
	WriteStr(pkg, auth_key);
    return pkg;
}

int32 UnpackCmdReq(EdpPacket* pkg, char** cmdid, uint16* cmdid_len, 
		   char** req, uint32* req_len)
{
//	os_printf("begin unpackCmdreq");
    uint32 remainlen;
    int rc;
    if (ReadRemainlen(pkg, &remainlen))
	return ERR_UNPACK_CMDREQ;

    rc = ReadUint16(pkg, cmdid_len);
    if (rc) 
        return rc;
    if (ReadBytes(pkg, (uint8**)cmdid, *cmdid_len))
        return ERR_UNPACK_CMDREQ;

    rc = ReadUint32(pkg, req_len);
    if (rc) 
        return rc;
    if (ReadBytes(pkg, (uint8**)req, *req_len))
        return ERR_UNPACK_CMDREQ;
        

  
//    os_printf("finsh unpackCmdreq");
    return 0;
}

EdpPacket* PacketCmdResp(const char* cmdid, uint16 cmdid_len,
			 const char* resp, uint32 resp_len)
{
    EdpPacket* send_pkg = NULL;
    unsigned remainlen = 0;

    send_pkg = NewBuffer();
    /* 6 = 2 + 4 = len(cmdid_len) + len(resp_len) */
    remainlen = cmdid_len + resp_len + (resp_len ? 6 : 2);
    WriteByte(send_pkg, CMDRESP);
    WriteRemainlen(send_pkg, remainlen);
    WriteUint16(send_pkg, cmdid_len);
    WriteBytes(send_pkg, cmdid, cmdid_len);
    if (resp_len){
	WriteUint32(send_pkg, resp_len);
	WriteBytes(send_pkg, resp, resp_len);
    }
    return send_pkg;
}

EdpPacket* PacketPing(void)
{
    EdpPacket* pkg = NULL;

    pkg = NewBuffer();
    /* msg type */
    WriteByte(pkg, PINGREQ);
    /* remain len */
    WriteRemainlen(pkg, 0);
    return pkg;
}

uint8 EdpPacketType(EdpPacket* pkg)
{
    uint8 mtype = 0x00;
    ReadByte(pkg, &mtype);
    return mtype;
}
EdpPacket* GetEdpPacket(RecvBuffer* buf)
{
    EdpPacket* pkg = NULL;
    int32 flag = 0;

 
    flag = IsPkgComplete(buf);  
    if (flag <= 0)
 		return pkg;
    pkg = NewBuffer();
    WriteBytes(pkg, buf->_data, flag);
    /* shrink buffer */
    memmove(buf->_data, buf->_data + flag, buf->_write_pos - flag);
    buf->_write_pos -= flag;
    return pkg;
}
int32 IsPkgComplete(RecvBuffer* buf)
{
    uint8* data = NULL;
    uint32 data_len = 0;
    uint32 multiplier = 1;
    uint32 len_val = 0;
    uint32 len_len = 1;
    uint8* pdigit = NULL;
    uint32 pkg_total_len = 0;
    
    data = buf->_data;
    data_len = buf->_write_pos;

    if (data_len <= 1) {
	return 0;   /* continue receive */
    }
    /* recevie remaining len */
    pdigit = data;

    do {
	if (len_len > 4) {
	    return -1;  /* protocol error; */
	}
	if (len_len > data_len - 1) {
	    return 0;   /* continue receive */
	}
	len_len++;
	pdigit++;
	len_val += ((*pdigit) & 0x7f) * multiplier;
	multiplier *= 0x80;
    }while(((*pdigit) & 0x80) != 0);

    pkg_total_len = len_len + len_val;
    /* receive payload */
    if (pkg_total_len <= (uint32)data_len){

	return pkg_total_len;   /* all data for this pkg is read */
    }else{
	return 0;   /* continue receive */
    }
}

int32 UnpackConnectResp(EdpPacket* pkg)
{
    uint8 flag, rtn;
    uint32 remainlen;
    if (ReadRemainlen(pkg, &remainlen))
        return ERR_UNPACK_CONNRESP_REMAIN;
    if (ReadByte(pkg, &flag))
        return ERR_UNPACK_CONNRESP_FLAG;
    if (ReadByte(pkg, &rtn))
        return ERR_UNPACK_CONNRESP_RTN;
//    assert(pkg->_read_pos == pkg->_write_pos);
    return (int32)rtn;
}

