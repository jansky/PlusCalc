/*
    PlusCalc Interpreter
    Copyright (C) 2015 Ian Duncan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "pluscalc.h"
#include <string.h>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

void replaceAll(std::string& str, const std::string& from, const std::string& to);
double evaluateInput(std::string input, double ans, bool exitonexception);
void evaluateCmdInput(int argc, char **argv);

std::string historypath;
	


int main(int argc, char **argv)
{
	
	historypath = getenv("HOME");
	historypath += "/.pluscalcint_history";
	
	if(argc > 1)
		evaluateCmdInput(argc, argv);
	else
	{
		
		
		read_history(historypath.c_str());
		
		
		
		double ans = 0;
	
		std::cout << "PlusCalc Interpreter" << std::endl << "Copyright (c) 2015 Ian Duncan" << std::endl << "This is free software with ABSOLUTELY NO WARRANTY.\nEnter 'license' for more information." <<  std::endl << "Enter 'quit' to exit and 'clear' to clear the history file (~/.pluscalcint_history)." << std::endl << std::endl;
	
		while(1)
		{
			char *rawinput;
			char prompt[3] = "> ";
			//std::string input;

			//std::cout << "> ";
			//std::getline(std::cin, input);
			
			rawinput = readline(prompt);
			
			if(!rawinput || std::string(rawinput) == "quit")
				exit(0);
			
			add_history(rawinput);
			write_history(historypath.c_str());

			ans = evaluateInput(std::string(rawinput), ans, false);
			
			free(rawinput);
			
			

		}
	}

	return 0;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

double evaluateInput(std::string input, double ans, bool exitonexception)
{
		
		std::cout.precision(std::numeric_limits<double>::digits10 + 1);
		
		double newans = 0;
		
		replaceAll(input,"ans",std::to_string(ans));
		
		if(input == "quit")
			exit(0);
		else if(input == "license")
		{
		
		    std::cout << "PlusCalc Interpreter" << std::endl;
    		    std::cout << "Copyright (C) 2015 Ian Duncan" << std::endl << std::endl;

    		    std::cout << "This program is free software: you can redistribute it and/or modify" << std::endl;
		    std::cout << "it under the terms of the GNU General Public License as published by" << std::endl;
		    std::cout << "the Free Software Foundation, either version 3 of the License, or" << std::endl;
		    std::cout << "(at your option) any later version." << std::endl << std::endl;

		    std::cout << "This program is distributed in the hope that it will be useful," << std::endl;
		    std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << std::endl;
		    std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << std::endl;
		    std::cout << "GNU General Public License for more details." << std::endl << std::endl;

		    std::cout << "You should have received a copy of the GNU General Public License" << std::endl;
		    std::cout << "along with this program.  If not, see <http://www.gnu.org/licenses/>." << std::endl << std::endl;
		}
		else if(input == "clear")
		{
			clear_history();
			if(history_truncate_file(historypath.c_str(), 0) != 0)
			{
				std::cout << "History clear NOT successful. Entries may remain in file." << std::endl;
				if(exitonexception)
					exit(7);
			}
			else
			{
				
				std::cout << "History clear successful." << std::endl;
			}
		}
		else
		{
			try
			{
				newans = pc_evaluate_math_expression(input);
				
				if(fmod(newans, 1) == 0)
					printf("%0.0lf\n", newans);
				else
					std::cout <<  newans << std::endl;
			}
			catch(PCException err)
			{
				int exitresult = 1;
				
				if(err == syntaxerror)
					std::cout << "Error: Syntax Error while evaluating input." << std::endl;
				else if(err == lasttokenerror)
				{
					std::cout << "Error: Last token was not a number." << std::endl;
					exitresult = 2;
				}
				else if(err == domainerror)
				{
					std::cout << "Error: Domain error." << std::endl;
					exitresult = 3;
				}
				else if(err == operationerror)
				{
					std::cout << "Error: No operations remain on stack." << std::endl;
					exitresult = 4;
				}
				else
				{
					std::cout << "Error: An unknown exception was caught." << std::endl;
					exitresult = 5;
				}
				
				if(exitonexception)
					exit(exitresult);
				

			}
			catch(...)
			{
				std::cout << "Error: An unknown exception was caught." << std::endl;
				
				if(exitonexception)
					exit(6);
			}
		}
		
		return newans;
}

void evaluateCmdInput(int argc, char **argv)
{
	std::string input;
	
	for(int i = 1; i < argc; i++)
	{
		input += std::string(argv[i]);
	}
	
	evaluateInput(input, 0, true);
}
