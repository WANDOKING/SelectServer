// version 1.1.0
#pragma once

#include <winnt.h>

#define MAX_PROFILING_DATA_COUNT 50
#define MAX_TAG_LENGTH 32

/*************************** HOT TO USE ***************************/
// PROFILE_BEGIN(L"test");
// ...
// PROFILE_END(L"test");
// PROFILE_SAVE(L"profile_result.txt");
/******************************************************************/

void ProfileBegin(const WCHAR* tag); // call this function at the starting point
void ProfileEnd(const WCHAR* tag);   // call this function at the end point
void ProfileDataOutText(const WCHAR* fileName); // profiling data save
void ProfileReset(void);

//#define PROFILE_ON

#ifdef PROFILE_ON
#define PROFILE_BEGIN(Tag) ProfileBegin(Tag)
#define PROFILE_END(Tag) ProfileEnd(Tag)
#define PROFILE_SAVE(fileName) ProfileDataOutText(fileName)

#else
#define PROFILE_BEGIN(Tag)
#define PROFILE_END(Tag) 
#define PROFILE_SAVE(fileName)

#endif