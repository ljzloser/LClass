#include "LDateTime.h"

LDate::LDate(const QDate& date)
{
	this->_date = date;
}

LDate::LDate(const QDateTime& datetime)
{
	this->_date = datetime.date();
}

LDate::LDate(const LDate& other)
{
	this->_date = other.toQDate();
}

LDate::LDate(int year, int month, int day)
{
	this->_date = QDate(year, month, day);
}

QDate LDate::toQDate() const
{
	return this->_date;
}

int LDate::year() const
{
	return this->_date.year();
}

int LDate::month() const
{
	return this->_date.month();
}

int LDate::day() const
{
	return this->_date.day();
}

LDate LDate::AddDays(int days) const
{
	return LDate(this->_date.addDays(days));
}

LDate LDate::AddMonths(int months) const
{
	return LDate(this->_date.addMonths(months));
}

LDate LDate::AddYears(int years) const
{
	return LDate(this->_date.addYears(years));
}

void LDate::Add(ljz::Date unit, int num)
{
	switch (unit)
	{
	case ljz::Date::y:
	{
		this->_date = this->_date.addYears(num);
		break;
	}
	case ljz::Date::M:
	{
		this->_date = this->_date.addMonths(num);
		break;
	}
	case ljz::Date::d:
	{
		this->_date = this->_date.addDays(num);
		break;
	}
	default:
		break;
	}
}

qint64 LDate::toNumber(ljz::Date unit) const
{
	switch (unit)
	{
	case ljz::Date::y:
	{
		return this->_date.year();
	}
	case ljz::Date::M:
	{
		return this->_date.year() * 12 + this->_date.month();
	}
	case ljz::Date::d:
	{
		return 1;
	}
	default:
		break;
	}

	return qint64();
}

qint64 LDate::toNumber(ljz::Time unit) const
{
	return qint64();
}

bool LDate::operator==(const LDate& other) const
{
	return this->_date == other.toQDate();
}

bool LDate::operator==(const QDate& other) const
{
	return	this->_date == other;
}

bool LDate::operator!=(const LDate& other) const
{
	return this->_date != other.toQDate();
}

bool LDate::operator!=(const QDate& other) const
{
	return this->_date != other;
}

bool LDate::operator>(const LDate& other) const
{
	return	this->_date > other.toQDate();
}

bool LDate::operator>(const QDate& other) const
{
	return	this->_date > other;
}

bool LDate::operator<(const LDate& other) const
{
	return this->_date < other.toQDate();
}

bool LDate::operator<(const QDate& other) const
{
	return this->_date < other;
}
bool LDate::operator>=(const LDate& other) const
{
	return	this->_date >= other.toQDate();
}

bool LDate::operator>=(const QDate& other) const
{
	return	this->_date >= other;
}
bool LDate::operator<=(const LDate& other) const
{
	return this->_date <= other.toQDate();
}

bool LDate::operator<=(const QDate& other) const
{
	return this->_date <= other;
}
LDate::~LDate()
{
}