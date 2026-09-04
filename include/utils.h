/**
 * utils
 */

#if !defined (_UTILS_H)
# define _UTILS_H

# include <basics.h>

void
hexdump(const u8 *data, const u32 size);

bool
file_read(Bytes *buf, const char *filename);

#endif
