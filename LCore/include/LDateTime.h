#pragma once
#include <qdatetime.h>
#include "lnamespace.h"
#include "lcore_global.h"

// 声明存在三个类
class LDateTime;
class LDate;
class LTime;
class LTimeZone;

class LCORE_EXPORT LDate
{
public:
	LDate(const QDate& date);
	LDate(const QDateTime& datetime);
	LDate(const LDate& other);
	LDate(int year, int month, int day);
	QDate toQDate() const;
	int year() const;
	int month() const;
	int day() const;
	LDate AddDays(int days) const;
	LDate AddMonths(int months) const;
	LDate AddYears(int years) const;
	void Add(ljz::Date unit, int num);
	qint64 toNumber(ljz::Date unit = ljz::Date::d) const;
	qint64 toNumber(ljz::Time unit = ljz::Time::s) const;
	// 重载运算符 ==
	bool operator==(const LDate& other) const;
	bool operator==(const QDate& other) const;
	// 重载运算符 !=
	bool operator!=(const LDate& other) const;
	bool operator!=(const QDate& other) const;
	// 重载运算符 >
	bool operator>(const LDate& other) const;
	bool operator>(const QDate& other) const;
	// 重载运算符 <
	bool operator<(const LDate& other) const;
	bool operator<(const QDate& other) const;
	// 重载运算符 >=
	bool operator>=(const LDate& other) const;
	bool operator>=(const QDate& other) const;
	// 重载运算符 <=
	bool operator<=(const LDate& other) const;
	bool operator<=(const QDate& other) const;
	~LDate();
private:
	QDate _date;
};
