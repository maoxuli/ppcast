//
// base64.h
//

#ifndef __BASE_64_H__
#define	__BASE_64_H__

#include "os.h"

// Table for encoding base64
static const char Base64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Table for encoding URL-safe base64
static const char URLBase64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!*";

// Table for decoding base64
#define XX 127
static const char Index64[256] = { /* Flawfinder: ignore */
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,62,XX,XX, XX,XX,XX,XX, XX,XX,63,62, XX,XX,XX,63,
    52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,XX,XX,XX,
    XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
    XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
    XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX
};

#define CHAR64(c)  (Index64[(unsigned char)(c)])

static void Output64Chunk(int c1, int c2, int c3, int pads, char* pBuf, size_t bufOffset)
{
    pBuf[bufOffset++] = Base64[c1>>2];
    pBuf[bufOffset++] = Base64[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)];
    if (pads == 2)
    {
        pBuf[bufOffset++] = '=';
        pBuf[bufOffset++] = '=';
    }
    else if (pads)
    {
        pBuf[bufOffset++] = Base64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        pBuf[bufOffset++] = '=';
    }
    else
    {
        pBuf[bufOffset++] = Base64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        pBuf[bufOffset++] = Base64[c3 & 0x3F];
    }
}

static void OutputURL64Chunk(int c1, int c2, int c3, int pads, char* pBuf, size_t bufOffset)
{
    pBuf[bufOffset++] = URLBase64[c1>>2];
    pBuf[bufOffset++] = URLBase64[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)];
    if (pads == 2)
    {
        pBuf[bufOffset++] = '=';
        pBuf[bufOffset++] = '=';
    }
    else if (pads)
    {
        pBuf[bufOffset++] = URLBase64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        pBuf[bufOffset++] = '=';
    }
    else
    {
        pBuf[bufOffset++] = URLBase64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        pBuf[bufOffset++] = URLBase64[c3 & 0x3F];
    }
}

UInt32L BinTo64(const uint8_t* pInBuf, size_t len, char* pOutBuf)
{
    int c1, c2, c3;
    //UInt32L ct = 0;
    //UInt32L written = 0;
    size_t inOffset = 0;
    size_t outOffset = 0;
    
    while(inOffset < len)
    {
        c1 = pInBuf[inOffset++];
        if(inOffset == len)
        {
            Output64Chunk(c1, 0, 0, 2, pOutBuf, outOffset);
            outOffset += 4;
        }
        else
        {
            c2 = pInBuf[inOffset++];
            if(inOffset == len)
            {
                Output64Chunk(c1, c2, 0, 1, pOutBuf, outOffset);
                outOffset += 4;
            }
            else
            {
                c3 = pInBuf[inOffset++];
                Output64Chunk(c1, c2, c3, 0, pOutBuf, outOffset);
                outOffset += 4;
            }
        }
#if XXXBAB
        ct += 4;
        if(ct > 44)	// EOL after 45 chars
        {
            pOutBuf[outOffset++] = '\n';
            written += 46;
            ct = 0;
        }
#endif
    }
#if XXXBAB
    if(ct)
    {
        pOutBuf[outOffset++] = '\n';
        ct++;
    }
#endif
    pOutBuf[outOffset++] = '\0';
    return outOffset;
}

UInt32L BinToURL64(const uint8_t* pInBuf, size_t len, char* pOutBuf)
{
    int c1, c2, c3;
    size_t inOffset = 0;
    size_t outOffset = 0;
    
    while(inOffset < len)
    {
        c1 = pInBuf[inOffset++];
        if(inOffset == len)
        {
            OutputURL64Chunk(c1, 0, 0, 2, pOutBuf, outOffset);
            outOffset += 4;
        }
        else
        {
            c2 = pInBuf[inOffset++];
            if(inOffset == len)
            {
                OutputURL64Chunk(c1, c2, 0, 1, pOutBuf, outOffset);
                outOffset += 4;
            }
            else
            {
                c3 = pInBuf[inOffset++];
                OutputURL64Chunk(c1, c2, c3, 0, pOutBuf, outOffset);
                outOffset += 4;
            }
        }
    }
    pOutBuf[outOffset++] = '\0';
    return outOffset;
}

UInt32L BinFrom64(const char* pInBuf, size_t len, uint8_t* pOutBuf)
{
    int c1, c2, c3, c4;
    size_t offset = 0;
    size_t outOffset = 0;
    
    while(offset < len)
    {
        c1 = pInBuf[offset++];
        if(c1 != '=' && CHAR64(c1) == XX)
        {
            continue;
        }
        else if (offset == len)
        {
            /* BAD data */
            return 0;
        }
        do
        {
            c2 = pInBuf[offset++];
        }
        while(offset < len && c2 != '=' && CHAR64(c2) == XX);
        if (offset == len)
        {
            /* BAD data */
            return 0;
        }
        
        do
        {
            c3 = pInBuf[offset++];
        }
        while(offset < len && c3 != '=' && CHAR64(c3) == XX);
        
        if (offset == len)
        {
            /* BAD data */
            return 0;
        }
        
        do
        {
            c4 = pInBuf[offset++];
        }
        while(offset < len && c4 != '=' && CHAR64(c4) == XX);
        
        if (offset == len && c4 != '=' && CHAR64(c4) == XX)
        {
            /* BAD data */
            return 0;
        }
        if(c1 == '=' || c2 == '=')
        {
            continue;
        }
        c1 = CHAR64(c1);
        c2 = CHAR64(c2);
        pOutBuf[outOffset++]  = ((c1 << 2) | ((c2 & 0x30) >> 4));
        if(c3 == '=')
        {
            continue;
        }
        else
        {
            c3 = CHAR64(c3);
            pOutBuf[outOffset++]  = (((c2 & 0x0f) << 4) | ((c3 & 0x3c) >> 2));
            if(c4 == '=')
            {
                continue;
            }
            else
            {
                c4 = CHAR64(c4);
                pOutBuf[outOffset++] = (((c3 & 0x03) << 6) | c4);
            }
        }
    }
    
    return outOffset;
}

#endif