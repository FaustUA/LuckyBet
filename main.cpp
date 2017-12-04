#include <iostream>
#include <string>
#include <conio.h>
#include "src/bets_manager.h"
#include "src/log.h"

using namespace std;

int wmain(int argc, wchar_t* args[])
{
	//bet_parsers parsers;
	bets_manager& mgr = get_bets_manager();
	mgr.init();

	mgr.add_bet_parser(new bookmaker_line_marathon());
	mgr.add_bet_parser(new bookmaker_line_1xbet());
	mgr.add_bet_parser(new bookmaker_line_fan_sport());
	mgr.add_bet_parser(new bookmaker_line_betfair());

	mgr.parse_data();
	mgr.process();
	mgr.output_final_line();
	mgr.output_main_lines_info();

	console_log() << "All bookmakers are parsed!" << endl;
	console_log() << "Press any key to exit!" << endl;
	_getch();

	mgr.finalize();

	return EXIT_SUCCESS;
}