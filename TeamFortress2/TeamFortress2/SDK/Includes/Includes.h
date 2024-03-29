#pragma once
#include "Classes.h"
#include "Const.h"
#include "Enums.h"
#include "Structs.h"
#include "bitbuf.h"
#include "MD5.h"
#include "Typedef.h"
#include <deque>

class IRefCounted
{
public:
	virtual int AddRef() = 0;
	virtual int Release() = 0;
};