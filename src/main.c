/**
* main.c
*/

#include <basics.h>

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

i32
main(i32 argc, char **argv)
{
	const char	*exe = SHIFT(argc, argv);
	
	return usage(exe, argc != 0);
}
