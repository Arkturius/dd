/**
* main.c
*/

#include <basics.h>
#include <celf.h>
#include <utils.h>
#include <decoder.h>

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

bool
didier_elf_check(ELF_Ident *ident)
{
	if (memcmp(&ident->e_magic, "\x7f""ELF", 4))
		return false;
	assert(ident->e_class      == ELF_64BIT);
	assert(ident->e_endianness == ELF_DATA2LSB);
	assert(ident->e_version    == EV_CURRENT);
	assert(ident->e_ABI        == EABI_SYSV);
	return true;
}

i32
main(i32 argc, char **argv)
{
	const char	*exe = SHIFT(argc, argv);

	if (argc == 0)
		return usage(exe, 1);

	const char	*src = SHIFT(argc, argv);

	Bytes	content = {0};
	Bytes	code    = {0};
	file_read(&content, src);

	u8			*data  = content.arr;
	ELF_Ident	*ident = (ELF_Ident *)data;

	if (!didier_elf_check(ident))
	{
		ERROR("File '%s' not supported.", src);
		return 1;
	}

	ELF64_Hdr	*file_header     = (ELF64_Hdr *)data;
	ELF64_Shdr	*section_headers = (ELF64_Shdr *)(data + file_header->e_shoff);

	for (u32 i = 0; i < file_header->e_shnum; ++i)
	{
		ELF64_Shdr	*section = &section_headers[i];

		if (section->sh_type == SHT_PROGBITS && (section->sh_flags & (SHF_EXECINSTR | SHF_ALLOC)))
		{
			INFO("Section [%u]: Type = 0x%08x", i, section->sh_type);
			code = array_slice(&content, section->sh_offset, section->sh_size);
			break ;
		}
	}
	
	decode(&code);

	return usage(exe, argc != 0);
}
