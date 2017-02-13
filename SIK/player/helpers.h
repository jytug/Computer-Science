#ifndef _HELPERS_PLAYER_H
#define _HELPERS_PLAYER_H

bool endswith(std::string const &, std::string const &);
int parse_value(std::string const &, std::string const &);
short parse_port_num(char *);
std::string &ltrim(std::string &, const char* t = " \t\n\r\f\v");

#endif
