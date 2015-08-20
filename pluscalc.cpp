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
#include <iostream>
#include <sstream>


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

double pc_trig_eval(PCTrigFunction ftype, double value, PCTrigMode tmode)
{
	double nvalue = value;
	double result = 0;
	bool convert_result = false;

	switch(tmode)
	{
		case rad:
		{
			break;
		}
		case deg:
		{
			nvalue = value * (PC_PI/180);
			break;
		}
		case grad:
		{
			nvalue = value * (PC_PI/200);
			break;
		}
		case turn:
		{
			nvalue = value * (2*PC_PI);
			break;
		}
		default:
		{
			throw syntaxerror;
		}
	}

	switch(ftype)
	{
		case tfsine:
		{
			result = sin(nvalue);
			break;
		}
		case tfcosine:
		{
			result = cos(nvalue);
			break;
		}
		case tftangent:
		{
			if(nvalue == 0.7853981633974482789)
				result = 1;
			else
				result = tan(nvalue);
			break;
		}
		case tfarcsine:
		{
			result = asin(value);
			convert_result = true;
			break;
		}
		case tfarccosine:
		{
			result = acos(value);
			convert_result = true;
			break;
		}
		case tfarctangent:
		{
			result = atan(value);
			convert_result = true;
			break;
		}
		default:
		{
			throw syntaxerror;
		}
	}

	if(convert_result)
	{
		switch(tmode)
		{
			case rad:
			{
				break;
			}
			case deg:
			{
				result = result * (180/PC_PI);
				break;
			}
			case grad:
			{
				result = result * (200/PC_PI);
				break;
			}
			case turn:
			{
				result = result / (2*PC_PI);
				break;
			}
			default:
			{
				throw syntaxerror;
			}
		}
	}

	return result;
	
}

/*std::vector<std::string> PCStringSplit(const std::string &source, const char *delimiter, bool keepEmpty)
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
*/

std::vector<std::string> PCStringSplit(const std::string &source, const char *delimiter, bool keepEmpty, bool keepEscape)
{
	std::vector<std::string> result;
	std::string buf;
	const char splitter = delimiter[0];

	
	
	for(int i = 0; i < source.length(); i++)
	{
		
		if(source[i] == '\\')
		{
			

			if(i+1 >= source.length())
				throw syntaxerror;

			if(keepEscape)
				buf += '\\';

			buf += source[i+1];

		

			i++;

			

		}
		else if(source[i] == splitter)
		{
		    
	
			if(buf.length() > 0 || keepEmpty)
				result.push_back(buf);

			buf = "";

		}
		else
		{
			buf += source[i];
			
		}
	}

	if(buf != "")
		result.push_back(buf);

	return result;
}

long double pc_factorial(long long n)
{
  long double value = 1;

    for(long long i = 2; i <= n; i++)
    {
    
        value = value * i;
    }

    return value;
}

long double pc_factorial_lim(long long n, long long l)
{
	long double value = n; 

    for(long long i = n-1; i > l; i--)
    {
    
        value = value * i;
    }

    return value;
}

PCCalcToken pc_iterate_tokens(std::vector<PCCalcToken>& tokens, PCTrigMode tmode)
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

		//Trig functions
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == sine && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					
					gofurther = false;
					newvalue = pc_create_token(number, pc_trig_eval(tfsine, wcopy[i+1].value, tmode));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == cosine && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					
					gofurther = false;
					newvalue = pc_create_token(number, pc_trig_eval(tfcosine, wcopy[i+1].value, tmode));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == tangent && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					
					gofurther = false;
					newvalue = pc_create_token(number, pc_trig_eval(tftangent, wcopy[i+1].value, tmode));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == arcsine && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					
					gofurther = false;
					newvalue = pc_create_token(number, pc_trig_eval(tfarcsine, wcopy[i+1].value, tmode));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == arccosine && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					
					gofurther = false;
					newvalue = pc_create_token(number, pc_trig_eval(tfarccosine, wcopy[i+1].value, tmode));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == arctangent && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					
					gofurther = false;
					newvalue = pc_create_token(number, pc_trig_eval(tfarctangent, wcopy[i+1].value, tmode));
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}


		//Probability Functions

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == npr && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					if(wcopy[i-1].value < (wcopy[i-1].value - wcopy[i+1].value))
						throw domainerror;
					gofurther = false;
					newvalue = pc_create_token(number, pc_factorial_lim((long long)wcopy[i-1].value, (long long)wcopy[i-1].value - wcopy[i+1].value));
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == ncr && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]) || !pc_is_value_numeric(wcopy[i + 1]))
					throw syntaxerror;
				else
				{
					if(wcopy[i-1].value < (wcopy[i-1].value - wcopy[i+1].value))
						throw domainerror;
					gofurther = false;
					long double step1 = pc_factorial_lim((long long)wcopy[i-1].value, (long long)(wcopy[i-1].value - wcopy[i+1].value));
					newvalue = pc_create_token(number, step1/pc_factorial((long long)wcopy[i+1].value));
					blacklist.push_back(i - 1);
					blacklist.push_back(i + 1);
					blacklist.push_back(i);
					insertat = i;

				}
			}
		}

		//Factorial
		for(int i = 0; i<wcopy.size(); i++)
		{
			if(wcopy[i].type == operand && wcopy[i].value == factorial && gofurther == true)
			{
				if(!pc_is_value_numeric(wcopy[i - 1]))
					throw syntaxerror;
				else
				{
					if(wcopy[i - 1].value < 0)
						throw domainerror;
					gofurther = false;
					newvalue = pc_create_token(number, pc_factorial((long long)wcopy[i - 1].value));
					blacklist.push_back(i - 1);
					blacklist.push_back(i);
					insertat = i;

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



PCCalcToken pc_create_token(PCTokenType type, long double value)
{
	PCCalcToken t;

	t.type = type;
	t.value = value;

	return t;
}

PCVariable pc_create_variable(long double value, char name)
{
	PCVariable var;
	std::vector<PCCalcToken> tokens;

	tokens.push_back(pc_create_token(number, value));

	var.tokens = tokens;
	var.name = name;

	return var;
}

PCVariable pc_create_variable(std::vector<PCCalcToken> value, char name)
{
	PCVariable var;
	

	var.tokens = value;
	var.name = name;

	//Check that variable definition is not endlessly recursive
	for(int i = 0; i < value.size(); i++)
	{
		if(value[i].type == variable && value[i].value == (long double)name)
			throw variableerror;
	}

	return var;
}

long double pc_evaluate_math_expression(std::string expr, PCTrigMode tmode)
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
	tokens = pc_parse_string(expr, false);

	PCCalcToken last = pc_iterate_tokens(tokens, tmode);

	if(last.type != number)
		throw lasttokenerror;

	return last.value;




}

long double pc_evaluate_math_expression(std::vector<PCCalcToken> expr, PCTrigMode tmode)
{
	PCCalcToken last = pc_iterate_tokens(expr, tmode);

	if(last.type != number)
		throw lasttokenerror;

	return last.value;
}

int pc_get_variable_by_name(std::vector<PCVariable> vars, char name)
{
	for(int i = 0; i < vars.size(); i++)
	{
		if(vars[i].name == name)
		{
			//5std::cout << vars[i].name << std::endl;
			return i;
		}
	}

	throw variableerror;

	return 0;
}

std::vector<PCCalcToken> pc_variable_substitute(std::vector<PCCalcToken> tok, std::vector<PCVariable> vars)
{
	std::vector<PCCalcToken> tokens = tok;
	bool foundvar = false;

	for(int i = 0; i < tokens.size(); i++)
	{
			//std::cout << tokens[i].value << std::endl;

			if(tokens[i].type == variable)
			{
				int varid = pc_get_variable_by_name(vars, (char)tokens[i].value);

				//std::cout << (vars[varid]).tokens[0].value << std::endl;

				//std::cout << tokens[i + 1].value << std::endl;

				tokens.erase(tokens.begin()+i);

				tokens.insert(tokens.begin()+i, vars[varid].tokens.begin(), vars[varid].tokens.end());

				//std::cout << tokens[i].value << std::endl;

				//std::cout << tokens[i + 1].value << std::endl;

				foundvar = true;
				goto outofloop;
			}
	}

	outofloop:

	if(foundvar)
		return pc_variable_substitute(tokens, vars);
	else
		return tokens;
}

long double pc_evaluate_math_expression(std::string expr, std::vector<PCVariable> vars, PCTrigMode tmode)
{
	

	std::vector<PCCalcToken> tokens;
	tokens = pc_parse_string(expr, true);
	//Not really but we need this to be true for the sake of the while loop
	bool foundvar = true;

	//Perform the substitution
	tokens = pc_variable_substitute(tokens, vars);
	


	PCCalcToken last = pc_iterate_tokens(tokens, tmode);

	if(last.type != number)
		throw lasttokenerror;

	return last.value;




}

long double pc_evaluate_math_expression(std::vector<PCCalcToken> expr, std::vector<PCVariable> vars, PCTrigMode tmode)
{
	

	std::vector<PCCalcToken> tokens = expr;
	//Not really but we need this to be true for the sake of the while loop
	bool foundvar = true;

	//Perform the substitution
	tokens = pc_variable_substitute(tokens, vars);
	


	PCCalcToken last = pc_iterate_tokens(tokens, tmode);

	if(last.type != number)
		throw lasttokenerror;

	return last.value;

}

std::string pc_tokens_to_string(std::vector<PCCalcToken> tokens)
{
	std::string result = "";

	for(int i = 0; i < tokens.size(); i++)
	{
		if(tokens[i].type == operand)
		{
			

			if(tokens[i].value == plus)
				result += "+";
			else if(tokens[i].value == minus)
				result += "-";
			else if(tokens[i].value == mul)
				result += "*";
			else if(tokens[i].value == divide)
				result += "/";
			else if(tokens[i].value == exponent)
				result += "^";
			else if(tokens[i].value == squareroot)
				result += "sqrt ";
			else if(tokens[i].value == root)
				result += " root ";
			else if(tokens[i].value == absolutevalue)
				result += "abs ";
			else if(tokens[i].value == leftper)
				result += "(";
			else if(tokens[i].value == rightper)
				result += ")";
			else if(tokens[i].value == logarithm)
				result += "log ";
			else if(tokens[i].value == naturallogarithm)
				result += "ln ";
			else if(tokens[i].value == baselogarithm)
				result += "logb ";
			else if(tokens[i].value == modulo)
				result += "%";
			else if(tokens[i].value == factorial)
				result += "!";
			else if(tokens[i].value == sine)
				result += "sin ";
			else if(tokens[i].value == cosine)
				result += "cos ";
			else if(tokens[i].value == tangent)
				result += "tan ";
			else if(tokens[i].value == arcsine)
				result += "asin ";
			else if(tokens[i].value == arccosine)
				result += "acos ";
			else if(tokens[i].value == arctangent)
				result += "atan ";
			else
				throw syntaxerror;
		}
		else if(tokens[i].type == variable)
		{
			result += char(tokens[i].value);
		}
		else if(tokens[i].type == number)
		{
			std::stringstream ss;
			ss << tokens[i].value;

			result += ss.str();
		}
		else
			throw syntaxerror;
	}

	return result;
}

std::string PCStringEscapeRemove(const std::string source)
{
	std::vector<int> positions;
	std::string new_str = source;

	for(int i = 0; i < source.length(); i++)
	{
		if(source[i] == '\\')
			positions.push_back(i);
	}

	for(int i = 0; i < positions.size(); i++)
	{
		new_str.erase(new_str.begin()+positions[i]);
	}

	return new_str;
}

	
