#include "javascript.h"

#ifdef LOG_TO_JAVASCRIPT
void log_string(const std::string &s) {
	console_log((uint32_t)s.data(), s.size());
}
#endif
