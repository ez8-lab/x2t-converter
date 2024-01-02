/*
 * (c) Copyright Ascensio System SIA 2010-2023
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at 20A-6 Ernesta Birznieka-Upish
 * street, Riga, Latvia, EU, LV-1050.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */

#include "SyntaxPtg.h"
#include "OperatorPtgs.h"
#include "PtgParen.h"

// #include <std/regex.hpp>
#include <regex>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "PtgList.h"

#include "../../../../../OOXML/Base/Unit.h"

namespace XLS
{

// static
const bool SyntaxPtg::is_operators(std::wstring::const_iterator first, std::wstring::const_iterator last)
{
	static std::wregex reg_operators(L"^ *[-+*/^&%<=>:]");
	return std::regex_search(first, last, reg_operators);
}


// static
const bool SyntaxPtg::extract_PtgAdd(std::wstring::const_iterator& first, std::wstring::const_iterator last, const bool operand_expected)
{
	if(!operand_expected)
	{
		static std::wregex reg_add(L"^ *\\+ *");
		std::match_results<std::wstring::const_iterator> results;
		if(std::regex_search(first, last, results, reg_add))
		{
			first = results[0].second;
			return true;
		}
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgUplus(std::wstring::const_iterator& first, std::wstring::const_iterator last, const bool operand_expected)
{
	if(operand_expected)
	{
		static std::wregex reg_uplus(L"^ *\\+ *");
		std::match_results<std::wstring::const_iterator> results;
		if(std::regex_search(first, last, results, reg_uplus))
		{
			first = results[0].second;
			return true;
		}
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgSub(std::wstring::const_iterator& first, std::wstring::const_iterator last, const bool operand_expected)
{
	if(!operand_expected)
	{
		static std::wregex reg_sub(L"^ *- *");
		std::match_results<std::wstring::const_iterator> results;
		if(std::regex_search(first, last, results, reg_sub))
		{
			first = results[0].second;
			return true;
		}
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgUminus(std::wstring::const_iterator& first, std::wstring::const_iterator last, const bool operand_expected)
{
	if(operand_expected)
	{
		static std::wregex reg_uminus(L"^ *- *");
		std::match_results<std::wstring::const_iterator> results;
		if(std::regex_search(first, last, results, reg_uminus))
		{
			first = results[0].second;
			return true;
		}
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgPercent(std::wstring::const_iterator& first, std::wstring::const_iterator last, const bool operand_expected)
{
	static std::wregex reg_perc(L"^ *% *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_perc))
	{
		OperatorPtgPtr last_operator;
		if(!operand_expected)
		{
			first = results[0].second;
			return true;
		}
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgMul(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_mul(L"^ *\\* *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_mul))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgDiv(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_div(L"^ */ *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_div))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgPower(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_pow(L"^ *\\^ *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_pow))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgEq(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_eq(L"^ *= *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_eq))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgNe(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_ne(L"^ *<> *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_ne))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgLe(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_le(L"^ *<= *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_le))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgLt(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_lt(L"^ *< *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_lt))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgGe(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_ge(L"^ *>= *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_ge))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgGt(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_gt(L"^ *> *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_gt))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgConcat(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_concat(L"^ *& *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_concat))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgUnion(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_union(L"^ *, *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_union))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::is_PtgIsect(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_before_comma(L"^ *[,()]");
	static std::wregex reg_isect(L"^ ");
	return !std::regex_search(first, last, reg_before_comma) &&
			std::regex_search(first, last, reg_isect);
}


// static
const bool SyntaxPtg::extract_PtgIsect(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	if(is_PtgIsect(first, last))
	{
		++first;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgRange(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_range(L"^ *: *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_range))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_comma(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_comma(L"^ *, *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_comma))
	{
		first = results[0].second;
		return true;
	}
	return false;
}

// static
const bool SyntaxPtg::extract_semicolon(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_semi(L"^ *; *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_semi))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgInt(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_differ(L"^\\d+[.:]"); // differ from PtgNum(5.55) and PtgRef(7:7)
	std::match_results<std::wstring::const_iterator> results;
	if(!std::regex_search(first, last, reg_differ))
	{
		static std::wregex reg_int(L"^\\d+");
		if(std::regex_search(first, last, results, reg_int))
		{
			std::wstring extracted(first, results[0].second);
			if(5 > extracted.length() || 5 == extracted.length() && L"65535" >= extracted)
			{
				out_str = extracted;
				first = results[0].second;
				return true;
			}
		}
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgNum(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_num(L"^[+-]?\\d*(\\d|\\.)\\d*([eE][+-]?\\d+)?");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_num))
	{
		out_str = std::wstring(first, results[0].second);
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgBool(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_bool(L"^(?i)(TRUE|FALSE)([-+*/^&%<=>: ;),]|$)"); // searched case insensitive
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_bool))
	{
		out_str = std::wstring(first, results[1].second);
		first = results[1].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgStr(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_str(L"^\"((\"\"|[^\"])*)\"");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_str))
	{
		out_str = results.str(1);
		XmlUtils::replace_all(out_str, L"\"\"", L"\"");
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgName(std::wstring::const_iterator& first, std::wstring::const_iterator last, unsigned int& out_num)
{
	static std::wregex reg_name(L"^(\\w[\\w\\d.]*)([-+*/^&%<=>: ;),]|$)");
	
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_name))
	{
		std::wstring define_names = results.str(1);

		out_num = XMLSTUFF::definenames2index(define_names);
		if (0xFFFFFFFF != out_num)
		{
			first = results[1].second;
			return true;
		}
	}
	return false;
}

// static
const bool SyntaxPtg::extract_PtgList(std::wstring::const_iterator& first, std::wstring::const_iterator last, PtgList& ptgList, unsigned short ixti)
{
	static std::wregex reg_table_name(L"^(\\w[\\w\\d.]*)\\["); //tableName '=SUM(tblName[Total])'

	std::match_results<std::wstring::const_iterator> results;
	if (std::regex_search(first, last, results, reg_table_name))
	{
		std::wstring tableName;
		_UINT32 indexTable;
		tableName = results.str(1);

		if (XMLSTUFF::isTableFmla(tableName, indexTable))
		{
			ptgList.listIndex = indexTable;
			ptgList.squareBracketSpace = false;
			ptgList.commaSpace = false;
			ptgList.invalid = false;
			ptgList.nonresident = false;
			ptgList.ixti = ixti;
			static std::wregex reg_inside_table1(L"\\[#?[\\s\\w\\d.]+\\]");
			static std::wregex reg_inside_table2(L"\\[#\\w[\\s\\w\\d.]*\\],\\[#\\w[\\s\\w\\d.]*\\]");
			static std::wregex reg_inside_table3(L"^[,;:]?\\[#?[\\s\\w\\d.]+\\]");

			first = results[1].second;

			std::match_results<std::wstring::const_iterator> results_1;
			if (std::regex_search(first, last, results_1, reg_inside_table1))
			{
				_UINT16 indexColumn = -1;
				std::wstring insider;
				ptgList.rowType = 0x00;
				ptgList.columns = 0x00;
				ptgList.colFirst = 65535;

				insider = results_1.str(0);

				if (insider == L"[#Data]")
				{
					if (std::regex_search(first, last, results_1, reg_inside_table2))
					{
						if (results_1.str(0) == L"[#Data],[#Totals]")
						{
							ptgList.rowType = 0x0C;
						}
					}
					else
						ptgList.rowType = 0x00;

				}
				else if (insider == L"[#All]")
				{
					ptgList.rowType = 0x01;
				}
				else if (insider == L"[#Headers]")
				{
					if (std::regex_search(first, last, results_1, reg_inside_table2))
					{
						if (results_1.str(0) == L"[#Headers],[#Data]")
						{
							ptgList.rowType = 0x06;
						}
					}
					else
						ptgList.rowType = 0x02;
				}
				else if (insider == L"[#Totals]")
				{
					ptgList.rowType = 0x08;
				}
				else if (insider == L"[#This Row]")
				{
					ptgList.rowType = 0x10;
				}

				else if (XMLSTUFF::isColumn(boost::algorithm::erase_last_copy(boost::algorithm::erase_first_copy(insider, L"["), L"]"), indexTable, indexColumn))
				{
					ptgList.rowType = 0x00;
					ptgList.colFirst = indexColumn;
					ptgList.columns = 0x01;
				}

				//first = results_1[0].first;
				first = results_1[0].second;

				if (std::regex_search(first, last, results_1, reg_inside_table3))
				{
					insider = results_1.str(0);
					if (!insider.empty() && insider[0] != '[')
						insider.erase(0, 1);

					if (XMLSTUFF::isColumn(boost::algorithm::erase_last_copy(boost::algorithm::erase_first_copy(insider, L"["), L"]"), indexTable, indexColumn))
					{
						//first = results_1[0].first;
						first = results_1[0].second;

						if (ptgList.colFirst == 65535)
						{
							ptgList.columns = 0x01;
							ptgList.colFirst = indexColumn;							

							if (std::regex_search(first, last, results_1, reg_inside_table3))
							{
								insider = results_1.str(0);
								if (!insider.empty() && insider[0] != '[')
									insider.erase(0, 1);

								if (XMLSTUFF::isColumn(boost::algorithm::erase_last_copy(boost::algorithm::erase_first_copy(insider, L"["), L"]"), indexTable, indexColumn))
								{
									ptgList.columns = 0x02;
									ptgList.colLast = indexColumn;
									//first = results_1[0].first;
									first = results_1[0].second;
								}
							}
						}
						else
						{
							ptgList.columns = 0x02;
							ptgList.colLast = indexColumn;
						}
					}					
				}

				if (first != last && *first == ']')
					++first;
					
				return true;
				
			}
		}
	}
	return false;
}

// static
const bool SyntaxPtg::extract_PtgRefErr(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_err(L"^(?i)#REF!");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_err))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgErr(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_err(L"^(?i)(#NULL!|#DIV/0!|#VALUE!|#REF!|#NAME\\?|#NUM!|#N/A)");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_err))
	{
		out_str = std::wstring(first, results[0].second);
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgArea(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_full(L"^\\$?[A-Za-z]+\\$?\\d+:\\$?[A-Za-z]+\\$?\\d+");
	static std::wregex reg_columns(L"^\\$?[A-Za-z]+:\\$?[A-Za-z]+");
	static std::wregex reg_rows(L"^\\$?\\d+:\\$?\\d+");
	std::match_results<std::wstring::const_iterator> results;
	if (std::regex_search(first, last, results, reg_full) ||
		std::regex_search(first, last, results, reg_columns) ||
		std::regex_search(first, last, results, reg_rows))
	{
		out_str = std::wstring(first, results[0].second);
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgRef(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_ref(L"^(\\$?[A-Za-z]+\\$?\\d+)([-+*/^&%<=>: ;),]|$)");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_ref))
	{
		out_str = std::wstring(first, results[1].second);
		first = results[1].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_3D_part(std::wstring::const_iterator& first, std::wstring::const_iterator last, unsigned short& ixti)
{
	static std::wregex reg_sheets(L"^(\\w[\\w\\d.]*(:\\w[\\w\\d.]*)?)!");
	static std::wregex reg_quoted(L"^'((''|[^]['\\/*?])*)'!");
	std::match_results<std::wstring::const_iterator> results;
	if (std::regex_search(first, last, results, reg_sheets) ||
		std::regex_search(first, last, results, reg_quoted))
	{

		std::wstring sheets_names = results.str(1);

		ixti = XMLSTUFF::sheetsnames2ixti(boost::algorithm::replace_all_copy(sheets_names, L"''", L"'"));
		if(0xFFFF != ixti)
		{
			first = results[0].second;
			return true;
		}
	}
	return false;
}


// static
const bool SyntaxPtg::extract_UndefinedName(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_undef(L"^([\\w\\d.]+)([-+*/^&%<=>: ;),]|$)");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_undef))
	{
		// Not compares to the defined names since it has already been done in extract_PtgName
		first = results[1].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgArray(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_array(L"^\\{([^{}\"]+|\"((\"\"|[^\"])*)\")([,;]([^{}\"]+|\"((\"\"|[^\"])*)\"))*\\}");
	std::match_results<std::wstring::const_iterator> results;
	if (std::regex_search(first, last, results, reg_array))
	{
		out_str = std::wstring(first, results[0].second);
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_LeftParenthesis(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_left_par(L"^ *\\( *");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_left_par))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_RightParenthesis(std::wstring::const_iterator& first, std::wstring::const_iterator last)
{
	static std::wregex reg_right_par(L"^ *\\)");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_right_par))
	{
		first = results[0].second;
		return true;
	}
	return false;
}


// static
const bool SyntaxPtg::extract_PtgFunc(std::wstring::const_iterator& first, std::wstring::const_iterator last, std::wstring& out_str)
{
	static std::wregex reg_func(L"^([\\w\\d.]+)\\(");
	std::match_results<std::wstring::const_iterator> results;
	if(std::regex_search(first, last, results, reg_func))
	{
		out_str = std::wstring(first, results[1].second);
		first = results[1].second;
		return true;
	}
	return false;
}


} // namespace XLS

