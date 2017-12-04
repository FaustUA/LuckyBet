#pragma once

#include "bookmaker_line.h"
#include "config.h"
#include "teams_storage.h"

class bets_manager;

typedef bets_manager* bets_manager_ptr;

class bets_manager
{
private:
	bets_manager();
	bets_manager(bets_manager const&);
	void operator =(bets_manager const&);
public:
	static bets_manager& get_instance()
	{
		static bets_manager instance;
		return instance;
	}

	void init();
	void finalize();

	bool is_same_team(const string& name1, const string& name2);
	bool process_same_team_request(const string& name1, const string& name2);
	void add_bet_parser(bookmaker_line* ptr);
	void parse_data();
	void process();

	void output_main_lines_info();
	void output_final_line();

	config get_config();
private:
	bet_koef get_max_koef(const bet_koef& f1, const bet_koef& f2);

	config f_cfg;
	teams_storage f_teams_storage;
	bookmaker_line_list f_bookmaker_lines;
	bet_info_list f_final_bet_infos;
};


bets_manager& get_bets_manager();