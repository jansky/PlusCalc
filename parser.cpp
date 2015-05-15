/*
    PlusCalc Library (libpluscalc)
    Copyright (C) 2015 Ian Duncan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "pluscalc.h"
#include <string>
#include <ctype.h>
#include <iostream>

std::vector<PCCalcToken> pc_parse_string(std::string s,bool varsenabled)
{
	std::vector<PCCalcToken> tokens;

	std::string curtoken;

	bool on_number = false;

	int i = 0;

	while(i < s.length())
	{
		//std::cout << i << std::endl;
		if(isdigit(s[i]) || s[i] == '.')
		{
			on_number = true;

			curtoken += s[i];

			i++;
		}
		else
		{
			if(on_number == true)
			{
				tokens.push_back(pc_create_token(number, std::stod(curtoken)));

				curtoken = "";
				on_number = false;
			}

			if(s[i] != ' ')
			{
				//std::cout << "!!" << std::endl;
				if(s[i] == '+')
				{
					tokens.push_back(pc_create_token(operand, plus));
				}
				else if(s[i] == '-')
				{
					tokens.push_back(pc_create_token(operand, minus));
				}
				else if(s[i] == '*')
				{
					tokens.push_back(pc_create_token(operand, mul));
				}
				else if(s[i] == '/')
				{
					tokens.push_back(pc_create_token(operand, divide));
				}
				else if(s[i] == '^')
				{
					tokens.push_back(pc_create_token(operand, exponent));
				}
				else if(s[i] == '(')
				{
					tokens.push_back(pc_create_token(operand, leftper));
				}
				else if(s[i] == ')')
				{
					tokens.push_back(pc_create_token(operand, rightper));
				}
				else if(s[i] == '%')
				{
					tokens.push_back(pc_create_token(operand, modulo));
				}
				else if(s[i] == '!')
				{
					
					tokens.push_back(pc_create_token(operand, factorial));
				}
				else if(s[i] == 'e')
				{
					tokens.push_back(pc_create_token(number, PC_E));
				}
				else if(s[i] == 's')
				{
					if(s.length() > i + 3 && s[i + 1] == 'q' && s[i + 2] == 'r' && s[i+3] == 't')
					{
						tokens.push_back(pc_create_token(operand, squareroot));
						i+=3;
					}
					else
					{
						if(varsenabled)
						{
							
							//Variables are enabled, treat char as a variable
							tokens.push_back(pc_create_token(variable, (double)s[i]));
						}
						else
						{
							//No variables, unrecognized character
							throw syntaxerror;
						}
					}
				}
				else if(s[i] == 'r')
				{
					if(s.length() > i + 3 && s[i + 1] == 'o' && s[i + 2] == 'o' && s[i+3] == 't')
					{
						tokens.push_back(pc_create_token(operand, root));
						i+=3;
					}
					else
					{
						if(varsenabled)
						{
							
							//Variables are enabled, treat char as a variable
							tokens.push_back(pc_create_token(variable, (double)s[i]));
						}
						else
						{
							//No variables, unrecognized character
							throw syntaxerror;
						}
					}
					
				}
				else if(s[i] == 'a')
				{
					if(s.length() > i + 2 && s[i + 1] == 'b' && s[i + 2] == 's')
					{
						tokens.push_back(pc_create_token(operand, absolutevalue));
						i+=2;
					}
					else
					{
						if(varsenabled)
						{
							
							//Variables are enabled, treat char as a variable
							tokens.push_back(pc_create_token(variable, (double)s[i]));
						}
						else
						{
							//No variables, unrecognized character
							throw syntaxerror;
						}
					}
					
				}
				else if(s[i] == 'n')
				{
					if(s.length() > i + 2 && s[i + 1] == 'P' && s[i + 2] == 'r')
					{
						tokens.push_back(pc_create_token(operand, npr));
						i+=2;
					}
					else if(s.length() > i + 2 && s[i + 1] == 'C' && s[i + 2] == 'r')
					{
						tokens.push_back(pc_create_token(operand, ncr));
						i+=2;
					}
					else
					{
						if(varsenabled)
						{
							
							//Variables are enabled, treat char as a variable
							tokens.push_back(pc_create_token(variable, (double)s[i]));
						}
						else
						{
							//No variables, unrecognized character
							throw syntaxerror;
						}
					}
					
				}
				else if(s[i] == 'l')
				{
					if(s.length() > i + 3 && s[i + 1] == 'o' && s[i + 2] == 'g' && s[i+3] == 'b')
					{
						tokens.push_back(pc_create_token(operand, baselogarithm));
						i+=3;
					}
					else if(s.length() > i + 2 && s[i + 1] == 'o' && s[i + 2] == 'g')
					{
						tokens.push_back(pc_create_token(operand, logarithm));
						i+=2;
					}
					else if(s.length() > i + 1 && s[i + 1] == 'n')
					{
						tokens.push_back(pc_create_token(operand, naturallogarithm));
						i+=1;
					}
					else
					{
						if(varsenabled)
						{
							
							//Variables are enabled, treat char as a variable
							tokens.push_back(pc_create_token(variable, (double)s[i]));
						}
						else
						{
							//No variables, unrecognized character
							throw syntaxerror;
						}
					}
					
				}
				else if(s[i] == 'p')
				{
					if(s.length() > i + 1 && s[i + 1] == 'i')
					{
						tokens.push_back(pc_create_token(number, PC_PI));
						i+=1;
					}
					else
					{
						if(varsenabled)
						{
							
							//Variables are enabled, treat char as a variable
							tokens.push_back(pc_create_token(variable, (double)s[i]));
						}
						else
						{
							//No variables, unrecognized character
							throw syntaxerror;
						}
					}
				}
					
				else
				{
					//std::cout << "~~" << std::endl;

					if(varsenabled)
					{
						//std::cout << s[i] << std::endl;
						//Variables are enabled, treat char as a variable
						tokens.push_back(pc_create_token(variable, (double)s[i]));
					}
					else
					{
						//No variables, unrecognized character
						throw syntaxerror;
					}
				}
			}

			
			i++;


			


		}
	}

	if(on_number == true)
	{
		tokens.push_back(pc_create_token(number, std::stod(curtoken)));

		curtoken = "";
		on_number = false;
	}

	//std::cout << tokens.size() << std::endl;
	return tokens;
}
