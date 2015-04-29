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


PCOperand pc_get_operand_from_string(std::string tocheck)
{
	if(tocheck == "+")
		return plus;
	else if(tocheck == "-")
		return minus;
	else if(tocheck == "*")
		return mul;
	else if(tocheck == "/")
		return divide;
	else if(tocheck == "^")
		return exponent;
	else if(tocheck == "sqrt")
		return squareroot;
	else if(tocheck == "root")
		return root;
	else if(tocheck == "abs")
		return absolutevalue;
	else if(tocheck == "(")
		return leftper;
	else if(tocheck == ")")
		return rightper;
	else if(tocheck == "log")
		return logarithm;
	else if(tocheck == "logb")
		return baselogarithm;
	else if(tocheck == "ln")
		return naturallogarithm;
	else
		return invalid;
}

bool pc_is_value_numeric(std::string tocheck)
{
	try
	{
		std::stod(tocheck);
	}
	catch(std::invalid_argument)
	{
		return false;
	}
	return true;
}

bool pc_is_value_numeric(PCCalcToken tocheck)
{
	if(tocheck.type == number)
		return true;
	else
		return false;
}

bool pc_i_is_in_blacklist(std::vector<int>& blacklist, int i)
{
	for(int j : blacklist)
	{
		if(j == i)
			return true;
	}

	return false;
}

std::vector<std::string> PCStringSplit(const std::string &source, const char *delimiter, bool keepEmpty)
{
    std::vector<std::string> results;

    size_t prev = 0;
    size_t next = 0;

    while ((next = source.find_first_of(delimiter, prev)) != std::string::npos)
    {
        if (keepEmpty || (next - prev != 0))
        {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < source.size())
    {
        results.push_back(source.substr(prev));
    }

    return results;
}

PCCalcToken pc_iterate_tokens(std::vector<PCCalcToken>& tokens)
{
	std::vector<PCCalcToken> wcopy = tokens;

	while(wcopy.size() > 1)
	{
		bool gofurther = true;
		PCCalcToken newvalue;
		std::vector<int> blacklist;
		std::vector<PCCalcToken> newtokenlist;
		int insertat = 0;
		bool foundoperation = false;

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand)
				foundoperation = true;
		}

		if(!foundoperation)
			throw operationerror;
			
		

		//Parentheses
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == leftper && gofurther == true)
			{
				int pneeded = 1;
				int pcount = 0;
				int l = i + 1;
				std::vector<PCCalcToken> parentheses;


				blacklist.push_back(i);
				try
				{
					while(pcount < pneeded && l < wcopy.size())
					{	
						if(wcopy[l].type == operand && wcopy[l].value == leftper)
							pneeded++;
						if(wcopy[l].type == operand && wcopy[l].value == rightper)
							pcount++;

						if((wcopy[l].type == operand && wcopy[l].value != rightper) || pneeded > 1 || wcopy[l].type == number)
							parentheses.push_back(wcopy[l]);

						blacklist.push_back(l);

						l++;

					}
				}
				catch(std::out_of_range)
				{
					throw syntaxerror;
				}

				if(pcount != pneeded)
					throw syntaxerror;

				if(!pc_is_value_numeric(parentheses[parentheses.size() -1]) && parentheses[parentheses.size() - 1].value == rightper)
					parentheses.pop_back();

				newvalue = pc_create_token(number, pc_iterate_tokens(parentheses).value);
				insertat = i;
				gofurther = false;
			}
		}

		//Negatives
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == minus && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					if(!pc_is_value_numeric(wcopy[i - 1]))
					{
						gofurther = false;
						newvalue = pc_create_token(number,0 - wcopy[i + 1].value);
						//blacklist.push_back(i - 1);
						blacklist.push_back(i + 1);
						blacklist.push_back(i);
						insertat = i;
					}

				}
			}
		}


		//Square Root and Absolute Value
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == squareroot && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					if(wcopy[i + 1].value < 0)
						throw domainerror;
					gofurther = false;
					newvalue = pc_create_token(number, sqrt(wcopy[i + 1].value));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == absolutevalue && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					gofurther = false;
					newvalue = pc_create_token(number, abs(wcopy[i + 1].value));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		//Logarithms
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == logarithm && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					if(wcopy[i + 1].value < 0)
						throw domainerror;
					gofurther = false;
					newvalue = pc_create_token(number, log10(wcopy[i + 1].value));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == naturallogarithm && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					if(wcopy[i + 1].value < 0)
						throw domainerror;
					gofurther = false;
					newvalue = pc_create_token(number, log(wcopy[i + 1].value));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == baselogarithm && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]) || !pc_is_value_numeric(wcopy[i + 2]))
					throw syntaxerror;
				else
				{
					if(wcopy[i + 1].value < 0 || wcopy[i + 2].value < 0)
						throw domainerror;
					gofurther = false;
					newvalue = pc_create_token(number, log10(wcopy[i + 2].value)/log10(wcopy[i + 1].value));
					blacklist.push_back(i + 1);
					blacklist.push_back(i + 2);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}


		//Root and Exponent
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == root && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					if((long)wcopy[i + 1].value % 2 == 0 && wcopy[i - 1].value < 0)
						throw domainerror;
					gofurther = false;
					newvalue = pc_create_token(number, pow(wcopy[i + 1].value,1.0/wcopy[i - 1].value));
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == exponent && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					gofurther = false;
					newvalue = pc_create_token(number, pow(wcopy[i - 1].value,wcopy[i + 1].value));
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}


		//Multiplication and Division and Modulo
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == mul && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					gofurther = false;
					newvalue = pc_create_token(number, wcopy[i - 1].value * wcopy[i + 1].value);
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == divide && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					gofurther = false;
					newvalue = pc_create_token(number, wcopy[i - 1].value / wcopy[i + 1].value);
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}
		
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == modulo && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					gofurther = false;
					newvalue = pc_create_token(number, fmod(wcopy[i - 1].value,wcopy[i + 1].value));
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		//Addition and Subtraction
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == plus && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					gofurther = false;
					newvalue = pc_create_token(number, wcopy[i - 1].value + wcopy[i + 1].value);
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == minus && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					gofurther = false;
					newvalue = pc_create_token(number, wcopy[i - 1].value - wcopy[i + 1].value);
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		//Rewrite the list of tokens

		for(int i = 0; i < wcopy.size();i++)
		{
			if(i == insertat)
				newtokenlist.push_back(newvalue);
			else if(pc_i_is_in_blacklist(blacklist, i))
				;
			else
				newtokenlist.push_back(wcopy[i]);
		}

		wcopy = newtokenlist;




	}

	return wcopy[0];
}



PCCalcToken pc_create_token(PCTokenType type, double value)
{
	PCCalcToken t;

	t.type = type;
	t.value = value;

	return t;
}

long double pc_evaluate_math_expression(std::string expr)
{
	/*std::vector<std::string> stringtokens = PCStringSplit(expr);
	std::vector<PCCalcToken> tokens;

	for(std::string strtoken : stringtokens)
	{
		if(!pc_is_value_numeric(strtoken))
		{
			PCOperand op = pc_get_operand_from_string(strtoken);

			if(op == invalid)
				throw syntaxerror;
			else
				tokens.push_back(pc_create_token(operand, op));

		}
		else
		{
			tokens.push_back(pc_create_token(number, std::stod(strtoken)));
		}
	}
	*/

	std::vector<PCCalcToken> tokens;
	tokens = pc_parse_string(expr);

	PCCalcToken last = pc_iterate_tokens(tokens);

	if(last.type != number)
		throw lasttokenerror;

	return last.value;




}

