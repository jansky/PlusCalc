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
#pragma once

#include <sstream>
#include <vector>
#include <stdexcept>
#include <cmath>

#define PC_E 2.71828182845904523536
#define PC_PI  3.141592653589793238462643383279502884

enum PCTokenType{operand,number,variable};
enum PCOperand{invalid,plus,minus,mul,divide,exponent,squareroot,root,absolutevalue,rightper,leftper,logarithm, naturallogarithm, baselogarithm,modulo,factorial,npr,ncr,sine,cosine,tangent,arcsine,arccosine,arctangent};
enum PCException{syntaxerror,lasttokenerror,domainerror,operationerror,variableerror};
enum PCTrigFunction{tfsine, tfcosine, tftangent, tfarcsine, tfarccosine, tfarctangent};
enum PCTrigMode{deg, rad, grad, turn};

typedef struct PCCalcToken
{
	PCTokenType type;
	long double value;
} PCCalcToken;

typedef struct PCVariable
{
    char name;
    std::vector<PCCalcToken> tokens;
    
} PCVariable;

bool pc_is_value_numeric(std::string tocheck);
bool pc_is_value_numeric(PCCalcToken tocheck);
bool pc_i_is_in_blacklist(std::vector<int>& blacklist, int i);

double pc_trig_eval(PCTrigFunction ftype, double value, PCTrigMode tmode);

PCOperand pc_get_operand_from_string(std::string tocheck);


PCCalcToken pc_iterate_tokens(std::vector<PCCalcToken>& tokens, PCTrigMode tmode = deg);

std::vector<std::string> PCStringSplit(const std::string &source, const char *delimiter = " ", bool keepEmpty = false, bool keepEscape = false);

std::string PCStringEscapeRemove(const std::string source);

PCCalcToken pc_create_token(PCTokenType type, long double value);

PCVariable pc_create_variable(long double value, char name);
PCVariable pc_create_variable(std::vector<PCCalcToken> value, char name);
std::vector<PCCalcToken> pc_variable_substitute(std::vector<PCCalcToken> tok, std::vector<PCVariable> vars);
int pc_get_variable_by_name(std::vector<PCVariable> vars, char name);

long double pc_evaluate_math_expression(std::string expr, PCTrigMode tmode = deg);
long double pc_evaluate_math_expression(std::vector<PCCalcToken> expr, PCTrigMode tmode = deg);
long double pc_evaluate_math_expression(std::string expr, std::vector<PCVariable> vars, PCTrigMode tmode = deg);
long double pc_evaluate_math_expression(std::vector<PCCalcToken> expr, std::vector<PCVariable> vars, PCTrigMode tmode = deg);

std::vector<PCCalcToken> pc_parse_string(std::string s,bool varsenabled=true);

std::string pc_tokens_to_string(std::vector<PCCalcToken> tokens);

long double pc_factorial(long long n);
long double pc_factorial_lim(long long n, long long l);


