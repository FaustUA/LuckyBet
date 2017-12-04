#include "bet_info.h"
#include <time.h>
#include <iomanip>

date_class::date_class(int m, int d, int h, int min)
	: month(m)
	, day(d)
	, hour(h)
	, minute(min)
{

}

bool date_class::operator == (const date_class& d)
{
	return this->month == d.month && this->day == d.day;
}

bool date_class::operator != (const date_class& d)
{
	return !(*this == d);
}

const date_class date_class::get_current_date()
{
	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	date_class res;
	res.month = timeinfo.tm_mon + 1;
	res.day = timeinfo.tm_mday;
	res.hour = timeinfo.tm_hour;
	res.minute = timeinfo.tm_min;

	return res;
}

bool bet_koef::operator > (const float v) const
{
	return (val > v);
}

ostream& operator <<(ostream& os, const date_class& date)
{
	os << std::setfill('0') << std::setw(2) << date.day << '.' << date.month << ' ' << date.hour << ':' << date.minute << std::endl;
	return os;
}

void bet_info::calc_benefits()
{
	if (koef_win_1 > 0 && koef_win_2 > 0 && koef_draw > 0)
		benefit_1_2_draw = 1 / koef_win_1.val +1 / koef_win_2.val + 1 / koef_draw.val;
	if (koef_win_1x > 0 && koef_win_2 > 0)
		benefit_1x_2 = 1 / koef_win_1x.val + 1 / koef_win_2.val;
	if (koef_win_12 > 0 && koef_draw > 0)
		benefit_12_x = 1 / koef_win_12.val + 1 / koef_draw.val;
	if (koef_win_2x > 0 && koef_draw > 0)
		benefit_2x_1 = 1 / koef_win_2x.val + 1 / koef_win_1.val;
}

ostream& operator << (ostream& os, const bet_koef& k)
{
	os << k.val << " (" << k.line_id << ")";
	return os;
}

ostream& operator <<(ostream& os, const bet_info& info)
{
	os << "______________________" << std::endl;

	os << info.team_1 << " -- " << info.team_2 << std::endl;
	os << info.date << std::endl;

	if (info.koef_win_1 > 0)
		os << "WIN 1:    " << info.koef_win_1 << std::endl;
	if (info.koef_draw > 0)
		os << "DRAW:    " << info.koef_draw << std::endl;
	if (info.koef_win_2 > 0)
		os << "WIN 2:    " << info.koef_win_2 << std::endl;

	os << std::endl;

	if (info.koef_win_1 > 0)
		os << "1X:    " << info.koef_win_1x << std::endl;
	if (info.koef_draw > 0)
		os << "12:    " << info.koef_win_12 << std::endl;
	if (info.koef_win_2 > 0)
		os << "2X:    " << info.koef_win_2x << std::endl;

	os << std::endl;

	if (info.benefit_1_2_draw > 0)
		os << "BENEFIT (1, X, 2):    " << info.benefit_1_2_draw << std::endl;
	if (info.benefit_1x_2 > 0)
		os << "BENEFIT (1X, 2):    " << info.benefit_1x_2 << std::endl;
	if (info.benefit_12_x > 0)
		os << "BENEFIT (12, X):    " << info.benefit_12_x << std::endl;
	if (info.benefit_2x_1 > 0)
		os << "BENEFIT (2X, 1):    " << info.benefit_2x_1 << std::endl;

	return os;
}
