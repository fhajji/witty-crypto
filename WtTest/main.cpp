/*
* Simple Encrypt / Decrypt Demo.
* Copyright (C) 2018 Farid Hajji <farid@hajji.name>
*/

#include "encdecapplication.h"

int main(int argc, char **argv)
{
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment &env) {
		return std::make_unique<EncDecApplication>(env);
	});
}