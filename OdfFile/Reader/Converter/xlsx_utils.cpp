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

#include "xlsx_utils.h"

#include <boost/lexical_cast.hpp>
// #include <boost/regex.hpp>
#include <regex>

// #include <boost/date_time/gregorian/gregorian.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>

#include "../../../OOXML/Base/Unit.h"

namespace cpdoccore {

namespace oox {

bool IsNumber(const std::wstring &value)
{
	std::wregex rule(L"^\\-{0,1}[0-9]*[.,]{0,1}[0-9]*$");
	std::match_results<std::wstring::const_iterator> results;

	return std::regex_search(value/*.begin(), value.end(), results*/, rule);
}
std::wstring GetNumberFromString(const std::wstring &value)
{
	std::wregex rule(L"^\\s*\\-{0,1}[0-9]*[.,]{0,1}[0-9]*\\s*$");
	std::match_results<std::wstring::const_iterator> results;

    if (std::regex_match(value, results, rule))
    {
        return (results[0].str());
	}
	else
		return L"";
}
std::wstring getColAddress(size_t col)
{
    static const size_t r = (L'Z' - L'A' + 1);
    std::wstring res;
    size_t r0 = col / r;

    if (r0 > 0)
    {
        const std::wstring rest = getColAddress(col - r * r0);
        const std::wstring res	= getColAddress(r0-1) + rest;
        return res;
    }
    else
        return std::wstring(1, (wchar_t)(L'A' + col));
}

std::wstring getRowAddress(size_t row)
{
    return std::to_wstring(row + 1);
}

std::wstring getCellAddress(size_t col, size_t row, bool bAbsolute)
{
	std::wstring a = bAbsolute ? L"$" : L"";
	return a + getColAddress(col) + a + getRowAddress(row);
}

size_t getColAddressInv(const std::wstring & a_)
{
    std::wstring a = a_;
    boost::algorithm::to_upper(a);
    
	static const size_t r = (L'Z' - L'A' + 1);
    size_t mul = 1;
    bool f = true;
    size_t res = 0;

	for (int i = a.length() - 1; i >= 0; i--)
	{
		size_t v = a[i] - L'A';
        if (f)
            f = false;
        else
            v += 1;
        res += v * mul;
        mul *= r;
    }
    return res;
}

size_t getRowAdderssInv(const std::wstring & a_)
{
	int sz = a_.length();
	if (a_.length()>0)
	{
	   return boost::lexical_cast<size_t>(a_)-1;
	}
	else
		return 0;
}

bool splitCellAddress(const std::wstring & a_, std::wstring & col, std::wstring & row)
{   
	std::wstring a = a_;

    XmlUtils::replace_all( a, L"$", L"");
	
	boost::algorithm::to_upper(a);
	
	for (size_t i = 0; i < a.length(); i++)
	{
		if (a[i] >= L'0' && a[i] <= L'9')
		{
			if (col.empty()) return false;

			row += a[i];
		}
		else if (a[i] >= L'A' && a[i] <= L'Z')
		{
			if (false == row.empty()) return false;

			col += a[i];
		}
		else
			return false;
    }

	if (col.size() > 5 || row.size() > 7) return false;

	return true;
}

bool getCellAddressInv(const std::wstring & a_, size_t & col, size_t & row)
{
    std::wstring colStr = L"", rowStr = L"";
	
	if (false == splitCellAddress(a_, colStr, rowStr)) return false;
    
    col = getColAddressInv( colStr );
    row = getRowAdderssInv( rowStr );

	return true;
}

bool parseBoolVal(const std::wstring & str)
{
    std::wstring s = str;
    boost::algorithm::to_lower(s);
    
	if (L"false" == str || L"0" == str)
        return false;
    else
        return true;
}

std::wstring cellType2Str(XlsxCellType::type type)
{
    switch(type)
    {
	case XlsxCellType::d:
        return L"d";
	case XlsxCellType::b:
        return L"b";
    case XlsxCellType::n:
        return L"n";
    case XlsxCellType::e:
        return L"e";
    case XlsxCellType::s:
        return L"s";
    case XlsxCellType::str:
        return L"str";
    case XlsxCellType::inlineStr:
        return L"inlineStr";
    default:
        return L"";
    }
}

int64_t convertDate(int Year, int Month, int Day)
{
    if (Year < 1400 || Year > 10000 || Month < 1 || Month > 12 || Day < 1 || Day > 31)
        return -1;

    std::tm start_date = {};
    start_date.tm_year = 0; // 1900
    start_date.tm_mon = 0;  // January
    start_date.tm_mday = 1; // 1st day

    std::time_t start_time = std::mktime(&start_date);

    std::tm input_date = {};
    input_date.tm_year = Year - 1900;
    input_date.tm_mon = Month - 1; // Months are 0-based in tm
    input_date.tm_mday = Day;

    std::time_t input_time = std::mktime(&input_date);

    const int64_t seconds_per_day = 60 * 60 * 24;
    int64_t daysFrom1900 = difftime(input_time, start_time) / seconds_per_day + 1;

    if (Year <= 1900 && Month <= 2 && Day <= 29) {
        return daysFrom1900;
    } else {
        return daysFrom1900 + 1;
    }
}

double convertTime(int hours, int minutes, double sec)
{
    // Calculate total milliseconds for the given time
    std::chrono::milliseconds t = hours * std::chrono::hours(1) +
                                  minutes * std::chrono::minutes(1) +
                                  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double>(sec));

    // Calculate total milliseconds in a day
    std::chrono::milliseconds day = std::chrono::hours(24);

    // Calculate the ratio of the given time to a day
    return static_cast<double>(t.count()) / static_cast<double>(day.count());
}

bool parseDateTime(const std::wstring & DateTime, int & Year, int & Month, int & Day,
									_CP_OPT(int) & Hours, _CP_OPT(int) & Minutes, _CP_OPT(int) & Sec)
{
    // for example, "1899-12-31T05:37:46.665696"
    try 
    {
        std::wregex r(L"([\\d]+)-([\\d]+)-([\\d]+)(?:T([\\d]+):([\\d]+):([\\d]+)(?:\\.([\\d]+))?)?");
        std::match_results<std::wstring::const_iterator> res;
        if (std::regex_match(DateTime, res, r))
        {
            Year    = boost::lexical_cast<int>(res[1].str());
            Month   = boost::lexical_cast<int>(res[2].str());
            Day     = boost::lexical_cast<int>(res[3].str());

            if (res[4].matched)
                Hours = boost::lexical_cast<int>(res[4].str());

            if (res[5].matched)
                Minutes = boost::lexical_cast<int>(res[5].str());

            if (res[6].matched)
                Sec = boost::lexical_cast<int>(res[6].str());

            if (res[7].matched)
				int FSec = boost::lexical_cast<int>(res[7].str());

            return true;
        }
    }
    catch(...)
    {
    }
    return false;
}

bool parseTime(const std::wstring & Time, int & Hours, int & Minutes, double & seconds)
{
    try 
    {
        std::wregex r(L"PT([\\d]+)H([\\d]+)M([\\d]+)(?:\\.([\\d]+))?S");
        std::match_results<std::wstring::const_iterator> res;
        if (std::regex_match(Time, res, r))
        {
            Hours   = boost::lexical_cast<int>(res[1].str());
            Minutes = boost::lexical_cast<int>(res[2].str());
            seconds = boost::lexical_cast<int>(res[3].str());

            if (res[4].matched)
            {
                seconds += boost::lexical_cast<double>(res[4].str()) / pow(10.0, 1.0 * res[4].str().length());
            }

            return true;
        }
    }
    catch(...)
    {
    }
    return false;
}

}
}
