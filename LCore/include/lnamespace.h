#pragma once

namespace ljz
{
	enum class HeavenlyStem
	{
		JIA = 0,		// 甲
		YI = 1,			// 乙
		BING = 2,		// 丙
		DING = 3,		// 丁
		WU = 4,			// 戊
		JI = 5,			// 己
		GENG = 6,		// 庚
		XIN = 7,		// 辛
		REN = 8,		// 壬
		GUI = 9			// 癸
	};
	enum class TerrestrialBranch
	{
		ZI = 0,			// 子
		CHOU = 1,		// 丑
		YIN = 2,		// 寅
		MAO = 3,		// 卯
		CHEN = 4,		// 辰
		SI = 5,			// 巳
		WU = 6,			// 午
		WEI = 7,		// 未
		SHEN = 8,		// 申
		YOU = 9,		// 酉
		XU = 10,		// 戌
		HAI = 11		// 亥
	};
	enum class Date
	{
		y = 0,
		M = 1,
		d = 2
	};
	enum class Time
	{
		H = 0,
		m = 1,
		s = 2,
		z = 3,
	};
}