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
#include <string.h>
#include <sstream>
#include <readline/readline.h>
#include <readline/history.h>

typedef struct PCIFunction
{
	std::string name;
	std::vector<PCCalcToken> tokens;
	std::vector<char> variable_names;
} PCIFunction;

void replaceAll(std::string& str, const std::string& from, const std::string& to);
long double evaluateInput(std::string input, long double ans, bool exitonexception);
void evaluateCmdInput(int argc, char **argv);
void exceptionresult(PCException err, bool exitonexception);

std::string historypath;
std::vector<PCVariable> variables;
std::vector<PCIFunction> functions;

PCIFunction pci_create_function(std::string name, std::vector<PCCalcToken> tokens, std::vector<char> variable_names);
int pci_get_function_id_by_name(std::string name);
long double pci_evaluate_function(PCIFunction function, std::string args);



PCIFunction pci_create_function(std::string name, std::vector<PCCalcToken> tokens, std::vector<char> variable_names)
{
	PCIFunction func;

	func.name = name;
	func.tokens = tokens;
	func.variable_names = variable_names;

	return func;
}

int pci_get_function_id_by_name(std::string name)
{
	for(int i = 0; i < functions.size(); i++)
	{
		if(functions[i].name == name)
			return i;
	}

	throw variableerror;
	return 0;
}

long double pci_evaluate_function(PCIFunction function, std::string args)
{
	//Make a copy of the variables list
	std::vector<PCVariable> specvars = variables;

	//Delete variables that are function arguments
	for(int i = 0; i < function.variable_names.size(); i++)
	{
		for(int j = 0; j < specvars.size(); j++)
		{
			if(specvars[j].name == function.variable_names[i])
			{
				specvars.erase(specvars.begin()+j);
				break;
			}
		}
	}

	//Parse the arguments
	std::vector<std::string> arguments = PCStringSplit(args, ",", false);

	if(arguments.size() != function.variable_names.size())
		throw variableerror;

	for(int i = 0; i < arguments.size(); i++)
	{
		specvars.push_back(pc_create_variable(pc_parse_string(arguments[i], true), function.variable_names[i]));
	}

	//Evaluate
	return pc_evaluate_math_expression(function.tokens, specvars);
}

void exceptionresult(PCException err, bool exitonexception)
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
				else if(err == variableerror)
				{
					std::cout << "Error: Variable error." << std::endl;
					exitresult = 8;
				}
				else
				{
					std::cout << "Error: An unknown exception was caught." << std::endl;
					exitresult = 5;
				}
				
				if(exitonexception)
					exit(exitresult);
}

int main(int argc, char **argv)
{
	
	historypath = getenv("HOME");
	historypath += "/.pluscalcint_history";
	
	if(argc > 1)
		evaluateCmdInput(argc, argv);
	else
	{
		
		
		read_history(historypath.c_str());
		
		
		
		long double ans = 0;
	
		std::cout << "PlusCalc Interpreter v1.99" << std::endl << "Copyright (c) 2015 Ian Duncan" << std::endl << "This is free software with ABSOLUTELY NO WARRANTY.\nEnter 'license' for more information." <<  std::endl << "Enter 'quit' to exit and 'clear' to clear the history file (~/.pluscalcint_history)." << std::endl << std::endl;
	
		while(1)
		{
			char *rawinput;
			char prompt[3] = "> ";
			//std::string input;

			//std::cout << "> ";
			//std::getline(std::cin, input);
			
			rawinput = readline(prompt);

			if(strlen(rawinput) == 0)
				;
			else
			{
			
				if(!rawinput || std::string(rawinput) == "quit")
					exit(0);
				
				add_history(rawinput);
				write_history(historypath.c_str());

				ans = evaluateInput(std::string(rawinput), ans, false);
				
				free(rawinput);
			}
			
			

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



long double evaluateInput(std::string input, long double ans, bool exitonexception)
{
		
		std::cout.precision(std::numeric_limits<long double>::digits10 + 1);
		
		long double newans = 0;
		
		replaceAll(input,"ans",std::to_string(ans));
		
		if(input == "quit")
			exit(0);
		else if(input == "license")
		{
		
		    std::cout << "PlusCalc Interpreter v1.99" << std::endl;
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
				std::vector<std::string> inputvec;

				inputvec = PCStringSplit(input);

				//We need to set a variable
				if(inputvec[0] == "set" && inputvec.size() > 2)
				{
					char name = (inputvec[1])[0];

					//Remove variable entries with the same name
					for(int i = 0; i < variables.size(); i++)
					{
						if(variables[i].name == name)
						{
							variables.erase(variables.begin()+i);
							break;
						}
					}

					//Build the string to tokenize
					std::string varraw = "";

					for(int i = 2; i < inputvec.size(); i++)
					{
						varraw += inputvec[i] + " ";
					}

					variables.push_back(pc_create_variable(pc_parse_string(varraw, true), name));


				}
				//We need to define a function
				else if(inputvec[0] == "func" && inputvec.size () > 2)
				{
					std::string name = inputvec[1];

					//Remove function entries with the same name
					for(int i = 0; i < functions.size(); i++)
					{
						if(functions[i].name == name)
						{
							functions.erase(functions.begin()+i);
							break;
						}
					}

					//Build the string to tokenize
					std::string fargsdef = "";

					for(int i = 2; i < inputvec.size(); i++)
					{
						fargsdef += inputvec[i] + " ";
					}

					//Split the arguments and definition
					std::vector<std::string> argsdef = PCStringSplit(fargsdef, ";", false);

					if(argsdef.size() != 2)
						throw syntaxerror;

					std::vector<char> variable_names;

					//Parse the list of arguments
					std::vector<std::string> args = PCStringSplit(argsdef[0], ",", false);

					if(args.size() < 1)
						throw syntaxerror;

					for(int i = 0; i < args.size(); i++)
					{
						char argname = (args[i])[0];

						//Check for duplicate arguments
						for(int j = 0; j < variable_names.size(); j++)
						{
							if(variable_names[j] == argname)
								throw syntaxerror;
						}

						variable_names.push_back(argname);
					}

					//Create the function and add it to the list
					functions.push_back(pci_create_function(name, pc_parse_string(argsdef[1], true), variable_names));




				}
				//We need to execute a function
				else if(inputvec[0] == "call" && inputvec.size() > 2)
				{
					std::string name = inputvec[1];

					//Join the arguments together
					std::string args = "";

					for(int i = 2; i < inputvec.size(); i++)
					{
						args += inputvec[i] + " ";
					}

					newans = pci_evaluate_function(functions[pci_get_function_id_by_name(name)], args);

					if(fmod(newans, 1) == 0)
						printf("%0.0Lf\n", newans);
					else
						std::cout <<  newans << std::endl;



				}
				//We need to generate a table of values
				else if(inputvec[0] == "tablegen" && inputvec.size() == 4)
				{
					std::string name = inputvec[1];

					int begin = atoi(inputvec[2].c_str());
					int end = atoi(inputvec[3].c_str());

					if(end < begin)
						throw syntaxerror;

					for(int i = begin; i <= end; i++)
					{
						std::stringstream ss;
						ss << i;

						newans = pci_evaluate_function(functions[pci_get_function_id_by_name(name)], ss.str());

						if((begin < 0 || end < 0) && i >= 0)
							std::cout << " " << i << "\t";
						else
							std::cout << i << "\t";

						if(fmod(newans, 1) == 0)
							if( newans >= 0)
								printf(" %0.0Lf\n", newans);
							else
								printf("%0.0Lf\n", newans);
						else
						{
							if( newans >= 0)
								std::cout <<  " " << newans << std::endl;
							else
								std::cout <<  newans << std::endl;
						}
					}
				}
				else
				{
					newans = pc_evaluate_math_expression(input, variables);
					
					if(fmod(newans, 1) == 0)
						printf("%0.0Lf\n", newans);
					else
						std::cout <<  newans << std::endl;
				}
			}
			catch(PCException err)
			{
				exceptionresult(err, exitonexception);
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
