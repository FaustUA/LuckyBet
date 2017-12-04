#include "bookmaker_line.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

#include "bets_manager.h"
#include "curl_wrapper.h"
#include "log.h"

using std::fstream;
using std::stringstream;
using std::map;

bookmaker_line::bookmaker_line(const char* url)
	: f_url(url)
	, f_skip_events(0)
	, f_raw_data("")
	, f_marker_pre_event("")
	, f_marker_event("")
	, f_marker_separate_teams("")
	, f_marker_end_teams("")
{
	
}

void bookmaker_line::parse_pre_event(bet_info_ptr info, size_t& pos)
{
	if (f_marker_pre_event != "")
	{
		pos = f_raw_data.find(f_marker_pre_event, pos);
		if (pos != string::npos)
		{
			pos += f_marker_pre_event.length();
			process_pre_event(info, pos);
		}
	}
}

void bookmaker_line::parse()
{
	if (!f_raw_data.empty())
	{
		console_log() << "Parsing " << get_id() << " line..." << std::endl;

		size_t event_pos = 0;
		bet_info_ptr bet = std::make_shared<bet_info>();
		parse_pre_event(bet, event_pos);
		event_pos = f_raw_data.find(f_marker_event, event_pos);
		int count = 0;
		int skip_count = 0;

		while (event_pos != string::npos)
		{
			size_t real_event_pos = event_pos;
			size_t next_pos = event_pos + f_marker_event.length();

			if (f_skip_events <= 0 || skip_count >= f_skip_events)
			{
				// get teams names
				size_t team1_pos = f_raw_data.find(f_marker_separate_teams, next_pos);
				size_t len = team1_pos - next_pos;
				if (len <= C_MAX_COMMAND_NAME_LENGTH)
				{
					string team1 = f_raw_data.substr(next_pos, len);
					next_pos = team1_pos + f_marker_separate_teams.length();
					size_t team2_pos = f_raw_data.find(f_marker_end_teams, next_pos);
					len = team2_pos - next_pos;
					if (len <= C_MAX_COMMAND_NAME_LENGTH)
					{
						string team2 = f_raw_data.substr(next_pos, len);

						bet = std::make_shared<bet_info>();
						bet->team_1 = team1;
						bet->team_2 = team2;

						next_pos = team2_pos + f_marker_end_teams.length();

						// get event date
						parse_date(bet, next_pos);

						// get koefficients
						parse_koefficients(bet, next_pos);

						f_bets.push_back(bet);
					}
				}

				parse_pre_event(bet, next_pos);
				skip_count = 0;
				++count;
			}
			else
			{
				++skip_count;
			}

			event_pos = f_raw_data.find(f_marker_event, next_pos);
		}
	}
}

void bookmaker_line::load_data()
{
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = nullptr;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, f_url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &f_raw_data);
		CURLcode res = curl_easy_perform(curl);
	}
}

void bookmaker_line::save_raw_data_to_file()
{
	string html_path = get_bets_manager().get_config().get_html_path();
	string fname = html_path + get_file_name();
	std::ofstream fs(fname);
	fs << f_raw_data;
}

void bookmaker_line::save_parsed_data_to_file()
{
	string parsed_path = get_bets_manager().get_config().get_parsed_path();
	string fname = parsed_path + get_file_name();
	std::ofstream fs(fname);
	if (!fs.is_open())
		return;

	fs << "Total bets: " << f_bets.size() << std::endl;

	for (bet_info_list::const_iterator it = f_bets.begin(); it != f_bets.end(); ++it)
	{
		bet_info_ptr p_bet = *it;
		fs << *p_bet;
	}
}

void bookmaker_line::output_bets() const
{
	for (bet_info_list::const_iterator it = f_bets.begin(); it != f_bets.end(); ++it)
	{
		bet_info_ptr p_bet = *it;
		final_log() << *p_bet;
	}
}

string bookmaker_line::find_string_value(size_t &pos, const string& start_marker, const string& end_marker)
{
	size_t res_pos = f_raw_data.find(start_marker, pos);

	if (res_pos == string::npos)
	{
		pos = res_pos;
		return "";
	}

	res_pos += start_marker.length();
	size_t res_end_pos = f_raw_data.find(end_marker, res_pos + 1);
	string value = f_raw_data.substr(res_pos, res_end_pos - res_pos);
	pos = res_end_pos + end_marker.length();

	return value;
}

float bookmaker_line::find_float_value(size_t &pos, const string& start_marker, const string& end_marker)
{
	string val = find_string_value(pos, start_marker, end_marker);
	return atof(val.c_str());
}

float bookmaker_line::find_float_value(size_t &pos, const string& start_marker, const string& continue_marker, const string& end_marker)
{
	string str = find_string_value(pos, start_marker, end_marker);
	size_t pos1 = str.find(continue_marker);
	pos1 += continue_marker.length();
	string s_val = str.substr(pos1, str.length() - pos1);

	return atof(s_val.c_str());
}

int bookmaker_line::get_month(const string& month)
{
	static map<string, int> months = {
		{"Jan", 1},
		{"Feb", 2},
		{"Mar", 3},
		{"Apr", 4},
		{"May", 5},
		{"Jun", 6},
		{"Jul", 7},
		{"Aug", 8},
		{"Sep", 9},
		{"Oct", 10},
		{"Nov", 11},
		{"Dec", 12}
	};

	return months[month];
}

string bookmaker_line::get_file_name()
{
	string fname = get_id();
	std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
	std::replace(fname.begin(), fname.end(), ' ', '_');
	fname += ".txt";

	return fname;
}

void bookmaker_line::output_main_info()
{
	console_log() << get_id() << ": " << f_bets.size() << std::endl;
}

// ************************************
//			  Maratahon
// ************************************

bookmaker_line_marathon::bookmaker_line_marathon()
	: baseclass("https://www.marathonbet.com/en/popular/Football/?menu=11")
{
	f_marker_event = "data-event-name=\"";
	f_marker_separate_teams = " vs ";
	f_marker_end_teams = "\"";
}

string bookmaker_line_marathon::get_id()
{
	return "Marathon";
}

float bookmaker_line_marathon::get_koefficient(size_t &pos)
{
	static const string C_RESULT_MARKER = "data-selection-price=\"";
	static const string C_RESULT_END_MARKER = "\"";
	size_t res_pos = f_raw_data.find(C_RESULT_MARKER, pos);
	res_pos += C_RESULT_MARKER.length();
	size_t res_end_pos = f_raw_data.find(C_RESULT_END_MARKER, res_pos + 1);
	string value = f_raw_data.substr(res_pos, res_end_pos - res_pos);
	pos = res_end_pos + C_RESULT_END_MARKER.length();

	return atof(value.c_str());
}

void bookmaker_line_marathon::parse_koefficients(bet_info_ptr info, size_t &pos)
{
	const string C_START_MARKER = "data-selection-price=\"";
	const string C_END_MARKER = "\"";
	
	info->koef_win_1 = { find_float_value(pos, C_START_MARKER, C_END_MARKER), get_id() };
	info->koef_draw = { find_float_value(pos, C_START_MARKER, C_END_MARKER), get_id() };
	info->koef_win_2 = { find_float_value(pos, C_START_MARKER, C_END_MARKER), get_id() };
}

void bookmaker_line_marathon::parse_date(bet_info_ptr info, size_t &pos)
{
	string start_marker = "d rowspan=\"2\" class=\"date \">\n";
	string end_marker = "\n";
	string str = find_string_value(pos, start_marker, end_marker);
	size_t white_pos = str.find_first_not_of(' ');
	if (white_pos != string::npos)
		str = str.substr(white_pos, str.length() - white_pos);

	stringstream ss(str);
	if (str.length() > 6)
	{
		ss >> info->date.day;
		string month;
		ss >> month;
		info->date.month = get_month(month);
	}
	else
	{
		date_class cur_date = date_class::get_current_date();
		info->date.month = cur_date.month;
		info->date.day = cur_date.day;
	}

	ss >> info->date.hour;
	char c;
	ss >> c;
	ss >> info->date.minute;
}

// ************************************
//              1Xbet
// ************************************

bookmaker_line_1xbet::bookmaker_line_1xbet()
	: baseclass("https://1xbetua.com/us/line/Football/")
	//: baseclass("https://1xbetua.com/us/line/Tennis/")
{
	f_marker_event = "<span class=\"n\" title=\"";
	//f_marker_separate_teams = " — ";
	f_marker_separate_teams = " â€” ";
	f_marker_end_teams = "   \">";
	f_marker_pre_event = "time min\">\n";
}

string bookmaker_line_1xbet::get_id()
{
	return "1X bet";
}

void bookmaker_line_1xbet::process_pre_event(bet_info_ptr info, size_t& pos)
{
	// get event date
	const string marker1 = ">";
	const string marker2 = "<";
	pos = f_raw_data.find(marker1, pos);
	size_t pos1 = pos;
	pos = f_raw_data.find(marker2, pos);
	size_t pos2 = pos;

	string s = f_raw_data.substr(pos1 + 1, pos2 - pos1 - 1);

	size_t p = s.find('.');
	string day = s.substr(0, p);
	s.erase(0, p+1);
	info->date.day = atoi(day.c_str());

	p = s.find(' ');
	string month = s.substr(0, p+1);
	s.erase(0, p+1);
	info->date.month = atoi(month.c_str());

	p = s.find(':');
	string hour = s.substr(0, p + 1);
	s.erase(0, p + 1);
	info->date.hour = atoi(hour.c_str());

	info->date.minute = atoi(s.c_str());
}

void bookmaker_line_1xbet::parse_koefficients(bet_info_ptr info, size_t &pos)
{
	static const string start_koef_marker = "coef=\"";
	static const string end_koef_marker = "\"";
	static const string start_bet_type_marker = "betname=\"";
	static const string end_bet_type_marker = "\"";
	static const string start_team1_marker = "opp1=\"";
	static const string end_team1_marker = "\"";

	while (true)
	{
		size_t old_pos = pos;
		float koef = find_float_value(pos, start_koef_marker, end_koef_marker);
		string type = find_string_value(pos, start_bet_type_marker, end_bet_type_marker);
		string team1 = find_string_value(pos, start_team1_marker, end_team1_marker);
		if (team1 != info->team_1)
		{
			pos = old_pos;
			return;
		}
		else
		{
			if (type == "1")
				info->koef_win_1 = { koef, get_id() };
			else if (type == "X")
				info->koef_draw = { koef, get_id() };
			else if (type == "2")
				info->koef_win_2 = { koef, get_id() };
			else if (type == "W1X")
				info->koef_win_1x = { koef, get_id() };
			else if (type == "12")
				info->koef_win_12 = { koef, get_id() };
			else if (type == "W2X")
				info->koef_win_2x = { koef, get_id() };
		}
	}
}

// ************************************
//             Fan sport
// ************************************

bookmaker_line_fan_sport::bookmaker_line_fan_sport()
	: baseclass()
{
	f_url = "https://fan-sport.com.ua/us/line/Football/";
}

string bookmaker_line_fan_sport::get_id()
{
	return "Fan sport";
}

// ************************************
//             Bet fair
// ************************************

bookmaker_line_betfair::bookmaker_line_betfair()
	: baseclass("https://www.betfair.com/sport/football")
{
	f_marker_event = "data-event=\"";
	f_marker_separate_teams = " v ";
	f_marker_end_teams = "\"";
	f_skip_events = 1;
}

string bookmaker_line_betfair::get_id()
{
	return "Bet fair";
}

void bookmaker_line_betfair::parse_koefficients(bet_info_ptr info, size_t &pos)
{
	const string C_START_MARKER = "ui-runner-price ";
	const string C_CONTINUE_MARKER = "> ";
	const string C_END_MARKER = " </span>";

	info->koef_win_1 = { find_float_value(pos, C_START_MARKER, C_CONTINUE_MARKER, C_END_MARKER), get_id() };
	info->koef_draw = { find_float_value(pos, C_START_MARKER, C_CONTINUE_MARKER, C_END_MARKER), get_id() };
	info->koef_win_2 = { find_float_value(pos, C_START_MARKER, C_CONTINUE_MARKER, C_END_MARKER), get_id() };
}
