// hexdump.h -- HexDump template class
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

// ISC License(ISC)
// 
// Copyright 2018 Farid Hajji <farid@hajji.name>
// 
// Permission to use, copy, modify, and/or distribute this software
// for any purpose with or without fee is hereby granted, provided
// that the above copyright notice and this permission notice appear
// in all copies.
// 
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS.IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
// OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <list>
#include <string>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cassert>

template <class Container = std::list<std::string>>
class HexDump
{
public:
	using Lines = Container;

	HexDump(const unsigned int chars_per_col = 8,
		const bool show_addresses = true,
		const std::size_t &address_width = 8,
		const std::string &sep_char = " ",
		const std::string &sep_col = "    ",
		const std::string &sep_print = " | ") :
		chars_per_col_(chars_per_col),
		show_addresses_(show_addresses),
		address_width_(address_width),
		sep_char_(sep_char),
		sep_col_(sep_col),
		sep_print_(sep_print) {}

	Lines toaddr(const std::string &input);
	Lines tohex(const std::string &input);
	Lines toprint(const std::string &input);

	std::string toprintline(const std::string &input);
	std::string fromhex(const std::string &hexline);
	std::string fromhexlines(const Lines &hexlines);

	std::string dump(const std::string &input);

protected:
	std::string lines_to_string(const Lines &addrlines, const Lines &hexlines, const Lines &printlines);

private:
	std::string char_to_hex(const unsigned char c);
	char char_to_print(const unsigned char c);

	unsigned int chars_per_col_;
	bool show_addresses_;
	std::size_t address_width_;
	std::string sep_char_;
	std::string sep_col_;
	std::string sep_print_;
};

template <class Container>
std::string HexDump<Container>::dump(const std::string &input)
{
	auto linesaddr = toaddr(input);
	auto lineshex = tohex(input);
	auto linesprint = toprint(input);

	auto out = lines_to_string(linesaddr, lineshex, linesprint);

	return out;
}

template <class Container>
typename HexDump<Container>::Lines HexDump<Container>::toaddr(const std::string &input)
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

template <class Container>
typename HexDump<Container>::Lines HexDump<Container>::tohex(const std::string & input)
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

template <class Container>
typename HexDump<Container>::Lines HexDump<Container>::toprint(const std::string & input)
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

template <class Container>
std::string HexDump<Container>::toprintline(const std::string &input)
{
	std::ostringstream oss;
	for (const auto &c : input)
		oss << char_to_print(c);
	return oss.str();
}

template <class Container>
std::string HexDump<Container>::fromhex(const std::string &hexline)
{
	std::ostringstream oss;
	std::istringstream iss(hexline);
	unsigned int c;
	while (iss >> std::hex >> c)
		oss << static_cast<unsigned char>(c);
	return oss.str();
}

template<class Container>
std::string HexDump<Container>::fromhexlines(const Lines &hexlines)
{
	std::ostringstream oss;
	for (const auto &line : hexlines)
		oss << fromhex(line);
	return oss.str();
}

template <class Container>
std::string HexDump<Container>::lines_to_string(const Lines &addrlines, const Lines &hexlines, const Lines &printlines)
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

template <class Container>
std::string HexDump<Container>::char_to_hex(const unsigned char c)
{
	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(c);
	return oss.str();
}

template <class Container>
char HexDump<Container>::char_to_print(const unsigned char c)
{
	return std::isprint(c) ? c : '.';
}