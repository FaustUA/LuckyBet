#include "bets_manager.h"
#include "log.h"
#include <limits>
#include <algorithm>

using std::endl;
using std::cout;
using std::cin;

bets_manager::bets_manager()
	: f_cfg("data/bets_manager.cfg")
{

}

void bets_manager::init()
{
	f_cfg.load();
	f_teams_storage.create_from_file(f_cfg.get_teams_filename());
}

void bets_manager::finalize()
{
	f_teams_storage.finalize(f_cfg.get_teams_filename());
}

bool bets_manager::is_same_team(const string& name1, const string& name2)
{
	return f_teams_storage.is_same_team(name1, name2);
}

bool bets_manager::process_same_team_request(const string& name1, const string& name2)
{
	console_log() << "_____________" << endl;
	console_log() << name1 << endl;
	console_log() << name2 << endl << endl;
	console_log() << "are the same?" << endl;
	console_log() << "y/n: ";
	char c;
	iconsole_log() >> c;
	if (c == 'y')
	{
		f_teams_storage.add_alias(name1, name2);
		return true;
	}

	return false;
}

void bets_manager::add_bet_parser(bookmaker_line_ptr ptr)
{
	f_bookmaker_lines.push_back(ptr);
}

void bets_manager::parse_data()
{
	for (bookmaker_line_list::iterator it = f_bookmaker_lines.begin(); it != f_bookmaker_lines.end(); ++it)
	{
		bookmaker_line_ptr pline = *it;

		pline->load_data();
		pline->save_raw_data_to_file();
		pline->parse();
		pline->save_parsed_data_to_file();
	}
}

void bets_manager::process()
{
	console_log() << "Merging bookmaker lines..." << endl;
	f_final_bet_infos.clear();

	for (bookmaker_line_list::iterator iter_line = f_bookmaker_lines.begin(); iter_line != f_bookmaker_lines.end(); ++iter_line)
	{
		bookmaker_line_ptr p_bets = *iter_line;
		//if (!dynamic_cast<bookmaker_line_marathon*>(p_bets))
		/*if (!dynamic_cast<bookmaker_line_fan_sport*>(p_bets))
			continue;*/

		const bet_info_list& bet_infos = p_bets->get_bet_infos();
		for (bet_info_list::const_iterator it = bet_infos.begin(); it != bet_infos.end(); ++it)
		{
			bet_info_ptr pbet = *it;
			string team1 = pbet->team_1;
			string team2 = pbet->team_2;

			bet_info_list::iterator bet_info_iter = f_final_bet_infos.end();
			for (auto iter = f_final_bet_infos.begin(); iter != f_final_bet_infos.end(); ++iter)
			{
				auto iter_bet = *iter;
				if (pbet->date == iter_bet->date && (is_same_team(iter_bet->team_1, pbet->team_1) || is_same_team(iter_bet->team_2, pbet->team_2)))
				{
					bet_info_iter = iter;
					break;
				}
			}

			if (bet_info_iter != f_final_bet_infos.end())
			{
				bet_info_ptr bet_info_ptr = (*bet_info_iter);
				if (!is_same_team(bet_info_ptr->team_1, team1))
				{
					if (!process_same_team_request(bet_info_ptr->team_1, team1))
						continue;
				}
				else if (!is_same_team(bet_info_ptr->team_2, team2))
				{
					if (!process_same_team_request(bet_info_ptr->team_2, team2))
						continue;
				}

				bet_info_ptr->koef_win_1 = get_max_koef(bet_info_ptr->koef_win_1, pbet->koef_win_1);
				bet_info_ptr->koef_draw = get_max_koef(bet_info_ptr->koef_draw, pbet->koef_draw);
				bet_info_ptr->koef_win_2 = get_max_koef(bet_info_ptr->koef_win_2, pbet->koef_win_2);
				bet_info_ptr->koef_win_1x = get_max_koef(bet_info_ptr->koef_win_1x, pbet->koef_win_1x);
				bet_info_ptr->koef_win_12 = get_max_koef(bet_info_ptr->koef_win_12, pbet->koef_win_12);
				bet_info_ptr->koef_win_2x = get_max_koef(bet_info_ptr->koef_win_2x, pbet->koef_win_2x);
			}
			else
			{
				f_final_bet_infos.push_back(std::make_shared<bet_info>(*pbet));
			}
		}
	}

	for (bet_info_list::const_iterator it = f_final_bet_infos.begin(); it != f_final_bet_infos.end(); ++it)
	{
		(*it)->calc_benefits();
	}
}

void bets_manager::output_main_lines_info()
{
	console_log() << endl;
	for (auto it = f_bookmaker_lines.begin(); it != f_bookmaker_lines.end(); ++it)
	{
		bookmaker_line_ptr line = *it;
		line->output_main_info();
	}

	console_log() << "Final events count: " << f_final_bet_infos.size() << std::endl;
	console_log() << std::endl;
}

void bets_manager::output_final_line()
{
	float min_1_x_2 = std::numeric_limits<float>::max();
	float min_1x_2 = std::numeric_limits<float>::max();
	float min_12_x = std::numeric_limits<float>::max();
	float min_2x_1 = std::numeric_limits<float>::max();

	for (auto it = f_final_bet_infos.begin(); it != f_final_bet_infos.end(); ++it)
	{
		bet_info_ptr p_bet = *it;

		if (p_bet->benefit_1_2_draw > 0 && p_bet->benefit_1_2_draw < min_1_x_2)
			min_1_x_2 = p_bet->benefit_1_2_draw;
		if (p_bet->benefit_1x_2 > 0 && p_bet->benefit_1x_2 < min_1x_2)
			min_1x_2 = p_bet->benefit_1x_2;
		if (p_bet->benefit_12_x > 0 && p_bet->benefit_12_x < min_12_x)
			min_12_x = p_bet->benefit_12_x;
		if (p_bet->benefit_2x_1 > 0 && p_bet->benefit_2x_1 < min_2x_1)
			min_2x_1 = p_bet->benefit_2x_1;

		final_log() << *p_bet;
	}

	final_log() << "_________________________" << endl;
	final_log() << "Total bets = " << f_final_bet_infos.size() << endl;
	final_log() << "Min benefit (1, X, 2) = " << min_1_x_2 << endl;
	final_log() << "Min benefit (1X, 2) = " << min_1x_2 << endl;
	final_log() << "Min benefit (12, X) = " << min_12_x << endl;
	final_log() << "Min benefit (2X, 1) = " << min_2x_1 << endl;
}

config bets_manager::get_config()
{
	return f_cfg;
}

bet_koef bets_manager::get_max_koef(const bet_koef& f1, const bet_koef& f2)
{
	return (f2.val > f1.val) ? f2 : f1;
}

bets_manager& get_bets_manager()
{
	return bets_manager::get_instance();
}