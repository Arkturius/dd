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

array_type(Bytes, SectionList);

# if 1

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

// GET the first section that looks like executable code, 
// works fine with .o.
// MUST be refactored into something that collect all interesting bytes
//   .text,
//   .data,
//   .rodata,
//   .plt,
//   etc...

	ELF64_Hdr	*file_header     = (ELF64_Hdr *)data;
	ELF64_Shdr	*section_headers = (ELF64_Shdr *)(data + file_header->e_shoff);
	SectionList	section_list     = {0};

	for (u32 i = 0; i < file_header->e_shnum; ++i)
	{
		ELF64_Shdr	*section = &section_headers[i];

		bool	ax    = (section->sh_flags & SHF_EXECINSTR) && (section->sh_flags & SHF_ALLOC);

		if (section->sh_type == SHT_PROGBITS && ax)
		{
			code = array_slice(&content, section->sh_offset, section->sh_size);
			array_push(&section_list, code);
		}
	}

	array_foreach(section, &section_list)
	{
		x86_Instructions	result = {0};

		INFO("SECTION DISASS --------------------");
		if (!decode(&result, section->arr, array_len(section)))
			return 1;
	}
	return usage(exe, argc != 0);
}

#else

i32
main(void)
{
	INFO("size of Instruction = %zu", sizeof(x86_Instruction));
}

#endif
