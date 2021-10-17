#pragma once
#include "../../Utils/Utils.h"

typedef int qboolean;
/*
typedef int XNADDR;
typedef uint64 XUID;

typedef struct {
	BYTE        ab[8];                          // xbox to xbox key identifier
} XNKID;

typedef struct {
	BYTE        ab[16];                         // xbox to xbox key exchange key
} XNKEY;

typedef struct _XSESSION_INFO
{
	XNKID sessionID;                // 8 bytes
	XNADDR hostAddress;             // 36 bytes
	XNKEY keyExchangeKey;           // 16 bytes
} XSESSION_INFO, * PXSESSION_INFO;

typedef struct _XUSER_DATA
{
	BYTE                                type;

	union
	{
		int                            nData;     // XUSER_DATA_TYPE_INT32
		int64                        i64Data;   // XUSER_DATA_TYPE_INT64
		double                          dblData;   // XUSER_DATA_TYPE_DOUBLE
		struct                                     // XUSER_DATA_TYPE_UNICODE
		{
			uint                       cbData;    // Includes null-terminator
			char* pwszData;
		} string;
		float                           fData;     // XUSER_DATA_TYPE_FLOAT
		struct                                     // XUSER_DATA_TYPE_BINARY
		{
			uint                       cbData;
			char* pbData;
		} binary;
	};
} XUSER_DATA, * PXUSER_DATA;

typedef struct _XUSER_PROPERTY
{
	DWORD                               dwPropertyId;
	XUSER_DATA                          value;
} XUSER_PROPERTY, * PXUSER_PROPERTY;

typedef struct _XUSER_CONTEXT
{
	DWORD                               dwContextId;
	DWORD                               dwValue;
} XUSER_CONTEXT, * PXUSER_CONTEXT;

*/