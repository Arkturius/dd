/**
* main.c
*/

#include <basics.h>
#include <stdbool.h>

i32
usage(const char *exe, i32 ret)
{
	if (ret) buf_with(msg)
	{
		buf_appendf(&msg, "Usage: %s " "\e[3m" "filename" "\e[0m\n", exe);
		buf_print(&msg);
	}
	return ret;
}

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

bool
file_read(Buffer *buf, const char *filename)
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

#include <utils.h>

i32
main(i32 argc, char **argv)
{
	const char	*exe = SHIFT(argc, argv);

	if (argc == 0)
		return usage(exe, 1);

	const char	*src = SHIFT(argc, argv);

	Buffer	content = {0};

	file_read(&content, src);
	hexdump((u8 *)content.arr, buf_len(&content));

	return usage(exe, argc != 0);
}
