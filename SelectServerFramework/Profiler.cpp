// version 1.1.0
#include <cassert>
#include <iostream>
#include <cstring>
#include <Windows.h>

#include "Profiler.h"

struct ProfilingData
{
	WCHAR Tag[MAX_TAG_LENGTH];
	LARGE_INTEGER ElapsedTimeSum;
	LARGE_INTEGER ElapsedTimeMin;
	LARGE_INTEGER ElapsedTimeMax;
	LARGE_INTEGER LastBeginTick;
	DWORD64 CallCount;
};

#ifdef PROFILE_ON

static ProfilingData g_profilingDatas[MAX_PROFILING_DATA_COUNT];
static size_t g_profilingDataCount = 0;

void ProfileBegin(const WCHAR* tag)
{
	for (size_t i = 0; i < g_profilingDataCount; ++i)
	{
		if (wcscmp(g_profilingDatas[i].Tag, tag) == 0)
		{
			QueryPerformanceCounter(&g_profilingDatas[i].LastBeginTick);
			return;
		}
	}

	++g_profilingDataCount;
	wcscpy_s(g_profilingDatas[g_profilingDataCount - 1].Tag, tag);
	g_profilingDatas[g_profilingDataCount - 1].ElapsedTimeMin.QuadPart = INT64_MAX;
	g_profilingDatas[g_profilingDataCount - 1].ElapsedTimeMax.QuadPart = 0;
	QueryPerformanceCounter(&g_profilingDatas[g_profilingDataCount - 1].LastBeginTick);
}

void ProfileEnd(const WCHAR* tag)
{
	for (size_t i = 0; i < g_profilingDataCount; ++i)
	{
		if (wcscmp(g_profilingDatas[i].Tag, tag) == 0)
		{
			LARGE_INTEGER endTick;
			LARGE_INTEGER elapsedTick;
			QueryPerformanceCounter(&endTick);

			elapsedTick.QuadPart = endTick.QuadPart - g_profilingDatas[i].LastBeginTick.QuadPart;

			g_profilingDatas[i].ElapsedTimeSum.QuadPart += elapsedTick.QuadPart;

			if (g_profilingDatas[i].ElapsedTimeMin.QuadPart > elapsedTick.QuadPart)
			{
				g_profilingDatas[i].ElapsedTimeMin.QuadPart = elapsedTick.QuadPart;
			}

			if (g_profilingDatas[i].ElapsedTimeMax.QuadPart < elapsedTick.QuadPart)
			{
				g_profilingDatas[i].ElapsedTimeMax.QuadPart = elapsedTick.QuadPart;
			}

			++g_profilingDatas[i].CallCount;
			return;
		}
	}

	assert(false);
}

void ProfileDataOutText(const WCHAR* fileName)
{
	FILE* file;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	if (_wfopen_s(&file, fileName, L"w") == EINVAL || file == nullptr)
	{
		return;
	}

	fprintf_s(file, "-----------------------------------------------------------------------------------------------------------------------------------------\n");
	fprintf_s(file, "%32s | %25s | %25s | %25s | %16s |\n", "Name", "Average", "Min", "Max", "Call");
	fprintf_s(file, "-----------------------------------------------------------------------------------------------------------------------------------------\n");

	for (size_t i = 0; i < g_profilingDataCount; ++i)
	{
		float min = (float)(g_profilingDatas[i].ElapsedTimeMin.QuadPart * 1000000) / frequency.QuadPart;
		float max = (float)(g_profilingDatas[i].ElapsedTimeMax.QuadPart * 1000000) / frequency.QuadPart;
		LONGLONG sumExcludingOutliers = g_profilingDatas[i].ElapsedTimeSum.QuadPart - g_profilingDatas[i].ElapsedTimeMax.QuadPart - g_profilingDatas[i].ElapsedTimeMin.QuadPart;
		float average = (float)((sumExcludingOutliers / (g_profilingDatas[i].CallCount - 2)) * 1000000) / frequency.QuadPart;
		fprintf_s(file, "%32ls | %16.4fmicrosecs | %16.4fmicrosecs | %16.4fmicrosecs | %16lld |\n", g_profilingDatas[i].Tag, average, min, max, g_profilingDatas[i].CallCount);
	}

	fwprintf_s(file, L"-----------------------------------------------------------------------------------------------------------------------------------------\n");
	fclose(file);
}

void ProfileReset(void)
{
	g_profilingDataCount = 0;
}

#endif