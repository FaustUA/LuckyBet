#pragma once

#include <string>
#include <memory>
#include "bet_info.h"

using std::string;
using std::shared_ptr;

static const int C_MAX_COMMAND_NAME_LENGTH = 50;

class bookmaker_line
{
public:
	bookmaker_line(const char* url = "");
	virtual ~bookmaker_line() {}

	virtual void process_pre_event(bet_info_ptr info, size_t& pos) {}
	void parse_pre_event(bet_info_ptr info, size_t& pos);
	virtual void parse();
	void load_data();
	void save_raw_data_to_file();
	void save_parsed_data_to_file();
	void output_bets() const;
	string find_string_value(size_t &pos, const string& start_marker, const string& end_marker);
	float find_float_value(size_t &pos, const string& start_marker, const string& end_marker);
	float find_float_value(size_t &pos, const string& start_marker, const string& continue_marker, const string& end_marker);
	virtual void parse_koefficients(bet_info_ptr info, size_t& pos) {}
	virtual int get_month(const string& month);
	virtual void parse_date(bet_info_ptr info, size_t& pos) {}

	const char* get_url() const { return f_url; }
	string get_raw_data() const { return f_raw_data; }
	const bet_info_list& get_bet_infos() { return f_bets; }
	virtual string get_id() { return ""; }
	string get_file_name();
	void output_main_info();
protected:
	string f_raw_data;
	bet_info_list f_bets;

	// markers and delimiters for parsing page
	const char* f_url;
	int f_skip_events;

	string f_marker_pre_event;
	string f_marker_event;
	string f_marker_separate_teams;
	string f_marker_end_teams;
};

class bookmaker_line_marathon : public bookmaker_line
{
	typedef bookmaker_line baseclass;
public:
	bookmaker_line_marathon();

	string get_id() override;

	float get_koefficient(size_t &pos);
	void parse_koefficients(bet_info_ptr info, size_t &pos) override;
	void parse_date(bet_info_ptr info, size_t &pos) override;
};

class bookmaker_line_1xbet : public bookmaker_line
{
	typedef bookmaker_line baseclass;
public:
	bookmaker_line_1xbet();

	string get_id() override;

	void process_pre_event(bet_info_ptr info, size_t& pos) override;
	void parse_koefficients(bet_info_ptr info, size_t &pos) override;
};

class bookmaker_line_fan_sport : public bookmaker_line_1xbet
{
	typedef bookmaker_line_1xbet baseclass;
public:
	bookmaker_line_fan_sport();

	string get_id() override;
};

class bookmaker_line_betfair : public bookmaker_line
{
	typedef bookmaker_line baseclass;
public:
	bookmaker_line_betfair();

	string get_id() override;

	void parse_koefficients(bet_info_ptr info, size_t &pos) override;
};

typedef bookmaker_line* bookmaker_line_ptr;
typedef std::list<bookmaker_line_ptr> bookmaker_line_list;
