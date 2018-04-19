// hexdump.cpp -- HexDump class
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

#include <string>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cassert>

#include "hexdump.h"

std::string HexDump::dump(const std::string &input)
{
	auto linesaddr = toaddr(input);
	auto lineshex = tohex(input);
	auto linesprint = toprint(input);

	auto out = lines_to_string(linesaddr, lineshex, linesprint);

	return out;
}

HexDump::Lines HexDump::toaddr(const std::string &input)
{
	Lines result;
	std::ostringstream oss;

	for (std::size_t addr = 0; addr < input.size(); addr += (2 * chars_per_col_)) {
		oss << std::setw(address_width_) << std::setfill('0') << std::hex << addr;
		result.push_back(oss.str());
		oss.str("");
	}

	return result;
}

HexDump::Lines HexDump::tohex(const std::string & input)
{
	Lines result;
	std::ostringstream oss;

	for (auto i = 0; i != input.size(); ++i) {
		if ((i > 0) && (i % (2 * chars_per_col_) == 0)) {
			// start a new line
			result.push_back(oss.str());
			oss.str("");
		}
		else if ((i > 0) && (i % chars_per_col_ == 0))
			// start second column
			oss << sep_col_;
		else if (i % chars_per_col_ != 0)
			// prepare to output next hex code
			oss << sep_char_;

		oss << char_to_hex(input[i]);
	}

	if (input.size() != 0) {
		// output incomplete last line
		std::size_t nspaces = (4 * chars_per_col_ /* 2 chars per hex on 2 columns */
			+ 2 * chars_per_col_*sep_char_.size() /* 1 separator per hex on 2 columns */
			- 2 * sep_char_.size() /* excluding separator for last hex, on 2 columns */
			+ sep_col_.size() /* including column separator */
			- oss.str().size() /* but excluding already generated content */);
		oss << std::string(nspaces, ' ');
		result.push_back(oss.str());
	}

	return result;
}

HexDump::Lines HexDump::toprint(const std::string & input)
{
	Lines result;
	std::ostringstream oss;

	for (auto i = 0; i != input.size(); ++i) {
		if ((i > 0) && (i % (2 * chars_per_col_) == 0)) {
			result.push_back(oss.str());
			oss.str("");
		}
		oss << char_to_print(input[i]);
	}
	if (input.size() != 0)
		result.push_back(oss.str());

	return result;
}

std::string HexDump::lines_to_string(const Lines &addrlines, const Lines &hexlines, const Lines &printlines)
{
	assert(addrlines.size() == hexlines.size());
	assert(hexlines.size() == printlines.size());

	std::ostringstream oss;

	auto iaddr = addrlines.begin();
	auto ihex = hexlines.begin();
	auto iprt = printlines.begin();

	for (; ihex != hexlines.end(); ++iaddr, ++ihex, ++iprt) {
		if (show_addresses_)
			oss << *iaddr << " ";
		oss << *ihex << sep_print_ << *iprt << std::endl;
	}
	return oss.str();
}


std::string HexDump::char_to_hex(const unsigned char c)
{
	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(c);
	return oss.str();
}

char HexDump::char_to_print(const unsigned char c)
{
	return std::isprint(c) ? c : '.';
}