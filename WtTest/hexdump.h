// hexdump.h -- HexDump class
// Copyright (C) 2018 Farid Hajji <farid@hajji.name>

#pragma once

#include <string>
#include <list>

class HexDump
{
public:
	using Lines = std::list<std::string>;

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