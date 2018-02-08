#pragma once

#define DEBUG 1
#undef DEBUG

/// `Print` function to remove performance penalty
#ifdef DEBUG
	#define Print(format, args...) printf(format, ## args)
#else
	#define Print(format, args...) {}
#endif
