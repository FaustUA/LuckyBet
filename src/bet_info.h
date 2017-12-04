#pragma once

#include <string>
#include <list>
#include <iostream>
#include <memory>

using std::string;
using std::ostream;

struct date_class
{
	date_class(int m = 0, int d = 0, int h = 0, int min = 0);

	int month;
	int day;
	int hour;
	int minute;

	bool operator == (const date_class& d);
	bool operator != (const date_class& d);

	static const date_class get_current_date();
	friend ostream& operator <<(ostream& os, const date_class& date);
};

struct bet_koef
{
	float val;
	string line_id;

	friend ostream& operator << (ostream& os, const bet_koef& k);
	bool operator > (const float v) const;
};

struct bet_info
{
	string team_1;
	string team_2;

	date_class date;

	bet_koef koef_win_1;
	bet_koef koef_win_2;
	bet_koef koef_draw;
	bet_koef koef_win_1x;
	bet_koef koef_win_2x;
	bet_koef koef_win_12;

	float benefit_1_2_draw;
	float benefit_1x_2;
	float benefit_2x_1;
	float benefit_12_x;

	friend ostream& operator <<(ostream& os, const bet_info& info);
	void calc_benefits();
};

typedef std::shared_ptr<bet_info> bet_info_ptr;
typedef std::list<bet_info_ptr> bet_info_list;