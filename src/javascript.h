#ifndef _JAVASCRIPT_H_
#define _JAVASCRIPT_H_

#include <string>
#include <stdint.h>

extern "C" {
	extern void console_log(uint32_t, unsigned int);
}

void log_string(const std::string&);

#endif
