#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "EdpKit.h"



/*---------------------------------------------------------------------------*/
Buffer* NewBuffer()
{
    Buffer* buf = (Buffer*)malloc(sizeof(Buffer));
    buf->_data = (uint8*)malloc(sizeof(uint8) * BUFFER_SIZE);
    buf->_write_pos = 0;
    buf->_read_pos = 0;
    buf->_capacity = BUFFER_SIZE;
    return buf;
}
void DeleteBuffer(Buffer** buf)
{
    uint8* pdata = (*buf)->_data;
    free(pdata);
    free(*buf);
    *buf = 0;
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
    *val = (uint8*)malloc(sizeof(uint8) * count);
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
    assert(buf->_read_pos == 0);
    if (CheckCapacity(buf, 1))
        return -1;
    buf->_data[buf->_write_pos] = byte;
    buf->_write_pos++;
    return 0;
}
int32 WriteBytes(Buffer* buf, const void* bytes, uint32 count)
{
    assert(buf->_read_pos == 0);
    if (CheckCapacity(buf, count))
        return -1;
    memcpy(buf->_data + buf->_write_pos, bytes, count);
    buf->_write_pos += count;
    return 0;
}
int32 WriteUint16(Buffer* buf, uint16 val)
{
    assert(buf->_read_pos == 0);
    return WriteByte(buf, MOSQ_MSB(val)) 
        || WriteByte(buf, MOSQ_LSB(val));
}
int32 WriteUint32(Buffer* buf, uint32 val)
{
    assert(buf->_read_pos == 0);
    return WriteByte(buf, (val >> 24) & 0x00FF) 
        || WriteByte(buf, (val >> 16) & 0x00FF)
        || WriteByte(buf, (val >> 8) & 0x00FF) 
        || WriteByte(buf, (val) & 0x00FF);
}

int32 WriteFloat(Buffer* buf, float val)
{
    assert(buf->_read_pos == 0);
    if (CheckCapacity(buf, 4))
        return -1;

   /*此处假设机器为小端;
        若为大端，需先颠倒float的四个字节顺序再writebytes*/

    return WriteBytes(buf, &val, sizeof(float));
}

int32 WriteStr(Buffer* buf, const char *str)
{
    uint16 length = 0;
    assert(buf->_read_pos == 0);
    length = strlen(str);
    return WriteUint16(buf, length) 
        || WriteBytes(buf, str, length);
}
int32 WriteRemainlen(Buffer* buf, uint32 len_val)
{
    uint32 remaining_length = len_val;
    int32 remaining_count = 0;
    uint8 byte = 0;

    assert(buf->_read_pos == 0);

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
    assert(remaining_count != 5);
    return 0;
}
/*---------------------------------------------------------------------------*/
/* connect1 (C->S): devid + apikey */
EdpPacket* PacketConnect1(const char* devid, const char* auth_key)
{
  //  EdpPacket* pkg = NewBuffer();
 
}
void testfun()
{

}



