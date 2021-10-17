#include "bitbuf.h"
#include "../../Utils/Utils.h"

int bf_write::StartWriting(void* pData, int nBytes, int iStartBit, int nBits)
{
	using fn = int(__thiscall *)(bf_write *, void *, int, int, int);
	static fn FN = reinterpret_cast<fn>(g_Pattern.Find(XorStr(L"engine.dll").c_str(), XorStr(L"55 8B EC 8B 45 08 8B 55 0C 83 E2 FC 89 01 8B 45 14").c_str()));
	return FN(this, pData, nBytes, iStartBit, nBits);
}

// (1 << i) - 1
unsigned long g_ExtraMasks[32];

const uint32 CBitBuffer::s_nMaskTable[33] = {
    0,
    (1 << 1) - 1,
    (1 << 2) - 1,
    (1 << 3) - 1,
    (1 << 4) - 1,
    (1 << 5) - 1,
    (1 << 6) - 1,
    (1 << 7) - 1,
    (1 << 8) - 1,
    (1 << 9) - 1,
    (1 << 10) - 1,
    (1 << 11) - 1,
    (1 << 12) - 1,
    (1 << 13) - 1,
    (1 << 14) - 1,
    (1 << 15) - 1,
    (1 << 16) - 1,
    (1 << 17) - 1,
    (1 << 18) - 1,
    (1 << 19) - 1,
    (1 << 20) - 1,
    (1 << 21) - 1,
    (1 << 22) - 1,
    (1 << 23) - 1,
    (1 << 24) - 1,
    (1 << 25) - 1,
    (1 << 26) - 1,
    (1 << 27) - 1,
    (1 << 28) - 1,
    (1 << 29) - 1,
    (1 << 30) - 1,
    0x7fffffff,
    0xffffffff,
};

inline int GetBitForBitnum(int bitNum)
{
    static int bitsForBitnum[] =
    {
        (1 << 0),
        (1 << 1),
        (1 << 2),
        (1 << 3),
        (1 << 4),
        (1 << 5),
        (1 << 6),
        (1 << 7),
        (1 << 8),
        (1 << 9),
        (1 << 10),
        (1 << 11),
        (1 << 12),
        (1 << 13),
        (1 << 14),
        (1 << 15),
        (1 << 16),
        (1 << 17),
        (1 << 18),
        (1 << 19),
        (1 << 20),
        (1 << 21),
        (1 << 22),
        (1 << 23),
        (1 << 24),
        (1 << 25),
        (1 << 26),
        (1 << 27),
        (1 << 28),
        (1 << 29),
        (1 << 30),
        (1 << 31),
    };

    return bitsForBitnum[(bitNum) & (BITS_PER_INT - 1)];
}

static BitBufErrorHandler g_BitBufErrorHandler = 0;

inline int BitForBitnum(int bitnum)
{
    return GetBitForBitnum(bitnum);
}

void InternalBitBufErrorHandler(BitBufErrorType errorType, const char* pDebugName)
{
    if (g_BitBufErrorHandler)
        g_BitBufErrorHandler(errorType, pDebugName);
}


void SetBitBufErrorHandler(BitBufErrorHandler fn)
{
    g_BitBufErrorHandler = fn;
}

// ---------------------------------------------------------------------------------------- //
// old_bf_read
// ---------------------------------------------------------------------------------------- //

old_bf_read::old_bf_read()
{
    m_pData = NULL;
    m_nDataBytes = 0;
    m_nDataBits = -1; // set to -1 so we overflow on any operation
    m_iCurBit = 0;
    m_bOverflow = false;
    m_bAssertOnOverflow = true;
    m_pDebugName = NULL;
}

old_bf_read::old_bf_read(const void* pData, int nBytes, int nBits)
{
    m_bAssertOnOverflow = true;
    StartReading(pData, nBytes, 0, nBits);
}

old_bf_read::old_bf_read(const char* pDebugName, const void* pData, int nBytes, int nBits)
{
    m_bAssertOnOverflow = true;
    m_pDebugName = pDebugName;
    StartReading(pData, nBytes, 0, nBits);
}

void old_bf_read::StartReading(const void* pData, int nBytes, int iStartBit, int nBits)
{
    // Make sure we're dword aligned.
    Assert(((unsigned long)pData & 3) == 0);

    m_pData = (unsigned char*)pData;
    m_nDataBytes = nBytes;

    if (nBits == -1)
    {
        m_nDataBits = m_nDataBytes << 3;
    }
    else
    {
        Assert(nBits <= nBytes * 8);
        m_nDataBits = nBits;
    }

    m_iCurBit = iStartBit;
    m_bOverflow = false;
}

void old_bf_read::Reset()
{
    m_iCurBit = 0;
    m_bOverflow = false;
}

void old_bf_read::SetAssertOnOverflow(bool bAssert)
{
    m_bAssertOnOverflow = bAssert;
}

const char* old_bf_read::GetDebugName()
{
    return m_pDebugName;
}

void old_bf_read::SetDebugName(const char* pName)
{
    m_pDebugName = pName;
}

unsigned int old_bf_read::CheckReadUBitLong(int numbits)
{
    // Ok, just read bits out.
    int i, nBitValue;
    unsigned int r = 0;

    for (i = 0; i < numbits; i++)
    {
        nBitValue = ReadOneBitNoCheck();
        r |= nBitValue << i;
    }
    m_iCurBit -= numbits;

    return r;
}

void old_bf_read::ReadBits(void* pOutData, int nBits)
{
#if defined( BB_PROFILING )
    VPROF("old_bf_write::ReadBits");
#endif

    unsigned char* pOut = (unsigned char*)pOutData;
    int nBitsLeft = nBits;


    // align output to dword boundary
    while (((unsigned long)pOut & 3) != 0 && nBitsLeft >= 8)
    {
        *pOut = (unsigned char)ReadUBitLong(8);
        ++pOut;
        nBitsLeft -= 8;
    }

    // X360TBD: Can't read dwords in ReadBits because they'll get swapped
    if (IsPC())
    {
        // read dwords
        while (nBitsLeft >= 32)
        {
            *((unsigned long*)pOut) = ReadUBitLong(32);
            pOut += sizeof(unsigned long);
            nBitsLeft -= 32;
        }
    }

    // read remaining bytes
    while (nBitsLeft >= 8)
    {
        *pOut = ReadUBitLong(8);
        ++pOut;
        nBitsLeft -= 8;
    }

    // read remaining bits
    if (nBitsLeft)
    {
        *pOut = ReadUBitLong(nBitsLeft);
    }

}

float old_bf_read::ReadBitAngle(int numbits)
{
    float fReturn;
    int i;
    float shift;

    shift = (float)(BitForBitnum(numbits));

    i = ReadUBitLong(numbits);
    fReturn = (float)i * (360.0 / shift);

    return fReturn;
}

unsigned int old_bf_read::PeekUBitLong(int numbits)
{
    unsigned int r;
    int i, nBitValue;
#ifdef BIT_VERBOSE
    int nShifts = numbits;
#endif

    old_bf_read savebf;

    savebf = *this;  // Save current state info

    r = 0;
    for (i = 0; i < numbits; i++)
    {
        nBitValue = ReadOneBit();

        // Append to current stream
        if (nBitValue)
        {
            r |= BitForBitnum(i);
        }
    }

    *this = savebf;

#ifdef BIT_VERBOSE
    Con_Printf("PeekBitLong:  %i %i\n", nShifts, (unsigned int)r);
#endif

    return r;
}

// Append numbits least significant bits from data to the current bit stream
int old_bf_read::ReadSBitLong(int numbits)
{
    int r, sign;

    r = ReadUBitLong(numbits - 1);

    // Note: it does this wierdness here so it's bit-compatible with regular integer data in the buffer.
    // (Some old code writes direct integers right into the buffer).
    sign = ReadOneBit();
    if (sign)
        r = -((BitForBitnum(numbits - 1)) - r);

    return r;
}

const byte g_BitMask[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
const byte g_TrailingMask[8] = { 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80 };

inline int old_bf_read::CountRunOfZeros()
{
    int bits = 0;
    if (m_iCurBit + 32 < m_nDataBits)
    {
#if !FAST_BIT_SCAN
        while (true)
        {
            int value = (m_pData[m_iCurBit >> 3] & g_BitMask[m_iCurBit & 7]);
            ++m_iCurBit;
            if (value)
                return bits;
            ++bits;
        }
#else
        while (true)
        {
            int value = (m_pData[m_iCurBit >> 3] & g_TrailingMask[m_iCurBit & 7]);
            if (!value)
            {
                int zeros = (8 - (m_iCurBit & 7));
                bits += zeros;
                m_iCurBit += zeros;
            }
            else
            {
                int zeros = CountTrailingZeros(value) - (m_iCurBit & 7);
                m_iCurBit += zeros + 1;
                bits += zeros;
                return bits;
            }
        }
#endif
    }
    else
    {
        while (ReadOneBit() == 0)
            bits++;
    }
    return bits;
}

unsigned int old_bf_read::ReadUBitVar()
{
    switch (ReadUBitLong(2))
    {
    case 0:
        return ReadUBitLong(4);

    case 1:
        return ReadUBitLong(8);

    case 2:
        return ReadUBitLong(12);

    default:
    case 3:
        return ReadUBitLong(32);
    }
#if 0
    int bits = CountRunOfZeros();

    unsigned int data = BitForBitnum(bits) - 1;

    // read the value
    if (bits > 0)
        data += ReadUBitLong(bits);

    return data;
#endif
}


unsigned int old_bf_read::ReadBitLong(int numbits, bool bSigned)
{
    if (bSigned)
        return (unsigned int)ReadSBitLong(numbits);
    else
        return ReadUBitLong(numbits);
}


// Basic Coordinate Routines (these contain bit-field size AND fixed point scaling constants)
float old_bf_read::ReadBitCoord(void)
{
#if defined( BB_PROFILING )
    VPROF("old_bf_write::ReadBitCoord");
#endif
    int		intval = 0, fractval = 0, signbit = 0;
    float	value = 0.0;


    // Read the required integer and fraction flags
    intval = ReadOneBit();
    fractval = ReadOneBit();

    // If we got either parse them, otherwise it's a zero.
    if (intval || fractval)
    {
        // Read the sign bit
        signbit = ReadOneBit();

        // If there's an integer, read it in
        if (intval)
        {
            // Adjust the integers from [0..MAX_COORD_VALUE-1] to [1..MAX_COORD_VALUE]
            intval = ReadUBitLong(COORD_INTEGER_BITS) + 1;
        }

        // If there's a fraction, read it in
        if (fractval)
        {
            fractval = ReadUBitLong(COORD_FRACTIONAL_BITS);
        }

        // Calculate the correct floating point value
        value = intval + ((float)fractval * COORD_RESOLUTION);

        // Fixup the sign if negative.
        if (signbit)
            value = -value;
    }

    return value;
}

float old_bf_read::ReadBitCoordMP(EBitCoordType coordType)
{
#if defined( BB_PROFILING )
    VPROF("old_bf_write::ReadBitCoordMP");
#endif
    int		intval = 0, fractval = 0, signbit = 0;
    float	value = 0.0;


    bool bInBounds = ReadOneBit() ? true : false;

    if (coordType == kCW_Integral)
    {
        // Read the required integer and fraction flags
        intval = ReadOneBit();
        // If we got either parse them, otherwise it's a zero.
        if (intval)
        {
            // Read the sign bit
            signbit = ReadOneBit();

            // If there's an integer, read it in
            // Adjust the integers from [0..MAX_COORD_VALUE-1] to [1..MAX_COORD_VALUE]
            if (bInBounds)
            {
                value = ReadUBitLong(COORD_INTEGER_BITS_MP) + 1;
            }
            else
            {
                value = ReadUBitLong(COORD_INTEGER_BITS) + 1;
            }
        }
    }
    else
    {
        // Read the required integer and fraction flags
        intval = ReadOneBit();

        // Read the sign bit
        signbit = ReadOneBit();

        // If we got either parse them, otherwise it's a zero.
        if (intval)
        {
            if (bInBounds)
            {
                intval = ReadUBitLong(COORD_INTEGER_BITS_MP) + 1;
            }
            else
            {
                intval = ReadUBitLong(COORD_INTEGER_BITS) + 1;
            }
        }

        // If there's a fraction, read it in
        fractval = ReadUBitLong(coordType == kCW_LowPrecision ? COORD_FRACTIONAL_BITS_MP_LOWPRECISION : COORD_FRACTIONAL_BITS);

        // Calculate the correct floating point value
        value = intval + ((float)fractval * (coordType == kCW_LowPrecision ? COORD_RESOLUTION_LOWPRECISION : COORD_RESOLUTION));
    }

    // Fixup the sign if negative.
    if (signbit)
        value = -value;

    return value;
}

void old_bf_read::ReadBitVec3Coord(Vector& fa)
{
    int		xflag, yflag, zflag;

    // This vector must be initialized! Otherwise, If any of the flags aren't set, 
    // the corresponding component will not be read and will be stack garbage.
    fa.Init(0, 0, 0);

    xflag = ReadOneBit();
    yflag = ReadOneBit();
    zflag = ReadOneBit();

    if (xflag)
        fa[0] = ReadBitCoord();
    if (yflag)
        fa[1] = ReadBitCoord();
    if (zflag)
        fa[2] = ReadBitCoord();
}

float old_bf_read::ReadBitNormal(void)
{
    // Read the sign bit
    int	signbit = ReadOneBit();

    // Read the fractional part
    unsigned int fractval = ReadUBitLong(NORMAL_FRACTIONAL_BITS);

    // Calculate the correct floating point value
    float value = (float)fractval * NORMAL_RESOLUTION;

    // Fixup the sign if negative.
    if (signbit)
        value = -value;

    return value;
}

void old_bf_read::ReadBitVec3Normal(Vector& fa)
{
    int xflag = ReadOneBit();
    int yflag = ReadOneBit();

    if (xflag)
        fa[0] = ReadBitNormal();
    else
        fa[0] = 0.0f;

    if (yflag)
        fa[1] = ReadBitNormal();
    else
        fa[1] = 0.0f;

    // The first two imply the third (but not its sign)
    int znegative = ReadOneBit();

    float fafafbfb = fa[0] * fa[0] + fa[1] * fa[1];
    if (fafafbfb < 1.0f)
        fa[2] = sqrt(1.0f - fafafbfb);
    else
        fa[2] = 0.0f;

    if (znegative)
        fa[2] = -fa[2];
}

void old_bf_read::ReadBitAngles(QAngle& fa)
{
    Vector tmp;
    ReadBitVec3Coord(tmp);
    fa.Init(tmp.x, tmp.y, tmp.z);
}

int old_bf_read::ReadChar()
{
    return ReadSBitLong(sizeof(char) << 3);
}

int old_bf_read::ReadByte()
{
    return ReadUBitLong(sizeof(unsigned char) << 3);
}

int old_bf_read::ReadShort()
{
    return ReadSBitLong(sizeof(short) << 3);
}

int old_bf_read::ReadWord()
{
    return ReadUBitLong(sizeof(unsigned short) << 3);
}

long old_bf_read::ReadLong()
{
    return ReadSBitLong(sizeof(long) << 3);
}

int64 old_bf_read::ReadLongLong()
{
    int64 retval;
    uint* pLongs = (uint*)&retval;

    // Read the two DWORDs according to network endian
    const short endianIndex = 0x0100;
    byte* idx = (byte*)&endianIndex;
    pLongs[*idx++] = ReadUBitLong(sizeof(long) << 3);
    pLongs[*idx] = ReadUBitLong(sizeof(long) << 3);

    return retval;
}

float old_bf_read::ReadFloat()
{
    float ret;
    Assert(sizeof(ret) == 4);
    ReadBits(&ret, 32);

    // Swap the float, since ReadBits reads raw data
    LittleFloat(&ret, &ret);
    return ret;
}

bool old_bf_read::ReadBytes(void* pOut, int nBytes)
{
    ReadBits(pOut, nBytes << 3);
    return !IsOverflowed();
}

bool old_bf_read::ReadString(char* pStr, int maxLen, bool bLine, int* pOutNumChars)
{
    Assert(maxLen != 0);

    bool bTooSmall = false;
    int iChar = 0;
    while (1)
    {
        char val = ReadChar();
        if (val == 0)
            break;
        else if (bLine && val == '\n')
            break;

        if (iChar < (maxLen - 1))
        {
            pStr[iChar] = val;
            ++iChar;
        }
        else
        {
            bTooSmall = true;
        }
    }

    // Make sure it's null-terminated.
    Assert(iChar < maxLen);
    pStr[iChar] = 0;

    if (pOutNumChars)
        *pOutNumChars = iChar;

    return !IsOverflowed() && !bTooSmall;
}


char* old_bf_read::ReadAndAllocateString(bool* pOverflow)
{
    char str[2048];

    int nChars;
    bool bOverflow = !ReadString(str, sizeof(str), false, &nChars);
    if (pOverflow)
        *pOverflow = bOverflow;

    // Now copy into the output and return it;
    char* pRet = new char[nChars + 1];
    for (int i = 0; i <= nChars; i++)
        pRet[i] = str[i];

    return pRet;
}

bool CBitRead::Seek(int nPosition)
{
    bool bSucc = true;
    if (nPosition < 0 || nPosition > m_nDataBits)
    {
        SetOverflowFlag();
        bSucc = false;
        nPosition = m_nDataBits;
    }
    int nHead = m_nDataBytes & 3;							// non-multiple-of-4 bytes at head of buffer. We put the "round off"
                                                            // at the head to make reading and detecting the end efficient.

    int nByteOfs = nPosition / 8;
    if ((m_nDataBytes < 4) || (nHead && (nByteOfs < nHead)))
    {
        // partial first dword
        uint8 const* pPartial = (uint8 const*)m_pData;
        if (m_pData)
        {
            m_nInBufWord = *(pPartial++);
            if (nHead > 1)
                m_nInBufWord |= (*pPartial++) << 8;
            if (nHead > 2)
                m_nInBufWord |= (*pPartial++) << 16;
        }
        m_pDataIn = (uint32 const*)pPartial;
        m_nInBufWord >>= (nPosition & 31);
        m_nBitsAvail = (nHead << 3) - (nPosition & 31);
    }
    else
    {
        int nAdjPosition = nPosition - (nHead << 3);
        m_pDataIn = reinterpret_cast<uint32 const*> (
            reinterpret_cast<uint8 const*>(m_pData) + ((nAdjPosition / 32) << 2) + nHead);
        if (m_pData)
        {
            m_nBitsAvail = 32;
            GrabNextDWord();
        }
        else
        {
            m_nInBufWord = 0;
            m_nBitsAvail = 1;
        }
        m_nInBufWord >>= (nAdjPosition & 31);
        m_nBitsAvail = MIN(m_nBitsAvail, 32 - (nAdjPosition & 31));	// in case grabnextdword overflowed
    }
    return bSucc;
}


void CBitRead::StartReading(const void* pData, int nBytes, int iStartBit, int nBits)
{
    // Make sure it's dword aligned and padded.
    Assert(((unsigned long)pData & 3) == 0);
    m_pData = (uint32*)pData;
    m_pDataIn = m_pData;
    m_nDataBytes = nBytes;

    if (nBits == -1)
    {
        m_nDataBits = nBytes << 3;
    }
    else
    {
        Assert(nBits <= nBytes * 8);
        m_nDataBits = nBits;
    }
    m_bOverflow = false;
    m_pBufferEnd = reinterpret_cast<uint32 const*> (reinterpret_cast<uint8 const*> (m_pData) + nBytes);
    if (m_pData)
        Seek(iStartBit);

}

bool CBitRead::ReadString(char* pStr, int maxLen, bool bLine, int* pOutNumChars)
{
    Assert(maxLen != 0);

    bool bTooSmall = false;
    int iChar = 0;
    while (1)
    {
        char val = ReadChar();
        if (val == 0)
            break;
        else if (bLine && val == '\n')
            break;

        if (iChar < (maxLen - 1))
        {
            pStr[iChar] = val;
            ++iChar;
        }
        else
        {
            bTooSmall = true;
        }
    }

    // Make sure it's null-terminated.
    Assert(iChar < maxLen);
    pStr[iChar] = 0;

    if (pOutNumChars)
        *pOutNumChars = iChar;

    return !IsOverflowed() && !bTooSmall;
}

char* CBitRead::ReadAndAllocateString(bool* pOverflow)
{
    char str[2048];

    int nChars;
    bool bOverflow = !ReadString(str, sizeof(str), false, &nChars);
    if (pOverflow)
        *pOverflow = bOverflow;

    // Now copy into the output and return it;
    char* pRet = new char[nChars + 1];
    for (int i = 0; i <= nChars; i++)
        pRet[i] = str[i];

    return pRet;
}

int64 CBitRead::ReadLongLong(void)
{
    int64 retval;
    uint* pLongs = (uint*)&retval;

    // Read the two DWORDs according to network endian
    const short endianIndex = 0x0100;
    byte* idx = (byte*)&endianIndex;
    pLongs[*idx++] = ReadUBitLong(sizeof(long) << 3);
    pLongs[*idx] = ReadUBitLong(sizeof(long) << 3);
    return retval;
}

void CBitRead::ReadBits(void* pOutData, int nBits)
{
    unsigned char* pOut = (unsigned char*)pOutData;
    int nBitsLeft = nBits;


    // align output to dword boundary
    while (((unsigned long)pOut & 3) != 0 && nBitsLeft >= 8)
    {
        *pOut = (unsigned char)ReadUBitLong(8);
        ++pOut;
        nBitsLeft -= 8;
    }

    // X360TBD: Can't read dwords in ReadBits because they'll get swapped
    if (IsPC())
    {
        // read dwords
        while (nBitsLeft >= 32)
        {
            *((unsigned long*)pOut) = ReadUBitLong(32);
            pOut += sizeof(unsigned long);
            nBitsLeft -= 32;
        }
    }

    // read remaining bytes
    while (nBitsLeft >= 8)
    {
        *pOut = ReadUBitLong(8);
        ++pOut;
        nBitsLeft -= 8;
    }

    // read remaining bits
    if (nBitsLeft)
    {
        *pOut = ReadUBitLong(nBitsLeft);
    }

}

bool CBitRead::ReadBytes(void* pOut, int nBytes)
{
    ReadBits(pOut, nBytes << 3);
    return !IsOverflowed();
}

float CBitRead::ReadBitAngle(int numbits)
{
    float shift = (float)(GetBitForBitnum(numbits));

    int i = ReadUBitLong(numbits);
    float fReturn = (float)i * (360.0 / shift);

    return fReturn;
}

// Basic Coordinate Routines (these contain bit-field size AND fixed point scaling constants)
float CBitRead::ReadBitCoord(void)
{
    int		intval = 0, fractval = 0, signbit = 0;
    float	value = 0.0;


    // Read the required integer and fraction flags
    intval = ReadOneBit();
    fractval = ReadOneBit();

    // If we got either parse them, otherwise it's a zero.
    if (intval || fractval)
    {
        // Read the sign bit
        signbit = ReadOneBit();

        // If there's an integer, read it in
        if (intval)
        {
            // Adjust the integers from [0..MAX_COORD_VALUE-1] to [1..MAX_COORD_VALUE]
            intval = ReadUBitLong(COORD_INTEGER_BITS) + 1;
        }

        // If there's a fraction, read it in
        if (fractval)
        {
            fractval = ReadUBitLong(COORD_FRACTIONAL_BITS);
        }

        // Calculate the correct floating point value
        value = intval + ((float)fractval * COORD_RESOLUTION);

        // Fixup the sign if negative.
        if (signbit)
            value = -value;
    }

    return value;
}

float CBitRead::ReadBitCoordMP(EBitCoordType coordType)
{
    int		intval = 0, fractval = 0, signbit = 0;
    float	value = 0.0;

    bool bInBounds = ReadOneBit() ? true : false;

    if (coordType == kCW_Integral)
    {
        // Read the required integer and fraction flags
        intval = ReadOneBit();
        // If we got either parse them, otherwise it's a zero.
        if (intval)
        {
            // Read the sign bit
            signbit = ReadOneBit();

            // If there's an integer, read it in
            // Adjust the integers from [0..MAX_COORD_VALUE-1] to [1..MAX_COORD_VALUE]
            if (bInBounds)
            {
                value = ReadUBitLong(COORD_INTEGER_BITS_MP) + 1;
            }
            else
            {
                value = ReadUBitLong(COORD_INTEGER_BITS) + 1;
            }
        }
    }
    else
    {
        // Read the required integer and fraction flags
        intval = ReadOneBit();

        // Read the sign bit
        signbit = ReadOneBit();

        // If we got either parse them, otherwise it's a zero.
        if (intval)
        {
            if (bInBounds)
            {
                intval = ReadUBitLong(COORD_INTEGER_BITS_MP) + 1;
            }
            else
            {
                intval = ReadUBitLong(COORD_INTEGER_BITS) + 1;
            }
        }

        // If there's a fraction, read it in
        fractval = ReadUBitLong(coordType == kCW_LowPrecision ? COORD_FRACTIONAL_BITS_MP_LOWPRECISION : COORD_FRACTIONAL_BITS);

        // Calculate the correct floating point value
        value = intval + ((float)fractval * (coordType == kCW_LowPrecision ? COORD_RESOLUTION_LOWPRECISION : COORD_RESOLUTION));
    }

    // Fixup the sign if negative.
    if (signbit)
        value = -value;

    return value;
}

void CBitRead::ReadBitVec3Coord(Vector& fa)
{
    int		xflag, yflag, zflag;

    // This vector must be initialized! Otherwise, If any of the flags aren't set, 
    // the corresponding component will not be read and will be stack garbage.
    fa.Init(0, 0, 0);

    xflag = ReadOneBit();
    yflag = ReadOneBit();
    zflag = ReadOneBit();

    if (xflag)
        fa[0] = ReadBitCoord();
    if (yflag)
        fa[1] = ReadBitCoord();
    if (zflag)
        fa[2] = ReadBitCoord();
}

float CBitRead::ReadBitNormal(void)
{
    // Read the sign bit
    int	signbit = ReadOneBit();

    // Read the fractional part
    unsigned int fractval = ReadUBitLong(NORMAL_FRACTIONAL_BITS);

    // Calculate the correct floating point value
    float value = (float)fractval * NORMAL_RESOLUTION;

    // Fixup the sign if negative.
    if (signbit)
        value = -value;

    return value;
}

void CBitRead::ReadBitVec3Normal(Vector& fa)
{
    int xflag = ReadOneBit();
    int yflag = ReadOneBit();

    if (xflag)
        fa[0] = ReadBitNormal();
    else
        fa[0] = 0.0f;

    if (yflag)
        fa[1] = ReadBitNormal();
    else
        fa[1] = 0.0f;

    // The first two imply the third (but not its sign)
    int znegative = ReadOneBit();

    float fafafbfb = fa[0] * fa[0] + fa[1] * fa[1];
    if (fafafbfb < 1.0f)
        fa[2] = sqrt(1.0f - fafafbfb);
    else
        fa[2] = 0.0f;

    if (znegative)
        fa[2] = -fa[2];
}

void CBitRead::ReadBitAngles(QAngle& fa)
{
    Vector tmp;
    ReadBitVec3Coord(tmp);
    fa.Init(tmp.x, tmp.y, tmp.z);
}