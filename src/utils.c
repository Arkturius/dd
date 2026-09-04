/**
 * utils.c
 */

#include "utils.h"
#include <ctype.h>

#include <basics.h>

typedef union	HDBuff
{
	u8	buf[16];
	struct
	{
		u64	a;
		u64	b;
	};
}	HDBuff;

INTERN void
hexdump_to_buffer(Buffer *out, const u8 *data, const u32 len)
{
	const u64	dots = 0x2e2e2e2e2e2e2e2e;
	HDBuff		hd   = {0};
	HDBuff		tmp1 = {0};
	bool		repeat = false;
	u32			addr = 0;
	u64			mask = 0;

	for (; addr < len; addr += 16)
	{
		hd = *(HDBuff *)&data[addr];

		if (addr && hd.a == tmp1.a && hd.b == tmp1.b)
		{
			if (repeat)
				continue ;
			buf_append(out, "*\n");
			repeat = true;
			continue ;
		}
		tmp1 = hd;
		repeat = false;

		buf_appendf(out, "%08x  ", addr);

		u32 i = 0;
		for (; i < 8 && addr + i < len; ++i)
			buf_appendf(out, "%02x ", data[addr + i]);
		for (; i < 8; ++i)
			buf_append(out, "   ");
		buf_appendc(out, ' ');
		for (; i < 16 && addr + i < len; ++i)
			buf_appendf(out, "%02x ", data[addr + i]);
		for (; i < 16; ++i)
			buf_append(out, "   ");
		buf_append(out, " |");

		for (u32 i = 0; i < 8; ++i)
			mask = (mask << 8) | (isprint(data[addr + 7 - i]) ? 0xff : 0x00);
		hd.a = ((hd.a ^ dots) & mask) ^ dots;
		for (u32 i = 0; i < 8; ++i)
			mask = (mask << 8) | (isprint(data[addr + 15 - i]) ? 0xff : 0x00);
		hd.b = ((hd.b ^ dots) & mask) ^ dots;
		buf_appendf(out, "%.*s|\n", len - addr < 16 ? len - addr : 16, hd.buf);
	}
	if (addr != len)
		buf_appendf(out, "%08x\n", len);
}

void
hexdump(const u8 *data, const u32 size)
{
	buf_with(out)
	{
		hexdump_to_buffer(&out, data, size);
		buf_print(&out);
	}
}

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

bool
file_read(Bytes *buf, const char *filename)
{
	struct stat	st = {0};

	if (stat(filename, &st) == -1)
		return false;

	i32	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return false;

	void	*map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	close(fd);
	if (map == MAP_FAILED)
		return false;

	buf->arr = malloc(st.st_size);
	buf->len = st.st_size;
	buf->cap = st.st_size;
	memcpy(buf->arr, map, buf->len);
	munmap(map, st.st_size);
	
	return true;
}
