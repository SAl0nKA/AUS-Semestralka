#pragma once
#include <crtdbg.h>

#ifdef _DEBUG
#define DBGNEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
class MemLeak
{
public:
	~MemLeak() { _CrtDumpMemoryLeaks(); }
};

const MemLeak ml;

#else
#define DBGNEW new
#endif