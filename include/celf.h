/**
 *	celf.h | ELF helper library.
 * -------------------------------------------------------------------------- */

#ifndef _CELF_H
# define _CELF_H

# include <basics.h>

enum	ELF_IdentIndices
{
	EI_MAG0			= 0,
	EI_MAG1			= 1,
	EI_MAG2			= 2,
	EI_MAG3			= 3,
	EI_CLASS		= 4,
	EI_DATA			= 5,
	EI_VERSION		= 6,
	EI_OSABI		= 7,
	EI_ABIVERSION	= 8,
};

enum	ELF_Class
{
	ELF_32BIT	= 1,
	ELF_64BIT	= 2,
};

enum	ELF_Endianness
{
	ELF_DATANONE	= 0x00,
	ELF_DATA2LSB	= 0x01,
	ELF_DATA2MSB	= 0x02,
};

enum	ELF_OsAbi
{
	EABI_SYSV		= 0x00,
	EABI_HPUX		= 0x01,
	EABI_NETBSD		= 0x02,
	EABI_LINUX		= 0x03,
	EABI_HURD		= 0x04,
	EABI_SOLARIS	= 0x06,
	EABI_AIX		= 0x07,
	EABI_IRIX		= 0x08,
	EABI_FREEBSD	= 0x09,
	EABI_TRU64		= 0x0A,
	EABI_MODESTO	= 0x0B,
	EABI_OPENBSD	= 0x0C,
	EABI_OPENVMS	= 0x0D,
	EABI_NSK		= 0x0E,
	EABI_AROS		= 0x0F,
	EABI_FENIXOS	= 0x10,
	EABI_CLOUDABI	= 0x11,
	EABI_OPENVOS	= 0x12,
};

/**
 * @struct	ELF_Ident.
 *
 * @brief		16 bytes ELF File identifier.
 *
 *		e_magic			0x7F 'E' 'L' 'F'
 *		e_class			File class			(32 or 64 bit)
 *		e_endianness	File endianness		(from offset 0x10)
 *		e_version		ELF version			(should always be 1)
 *		e_ABI			OS ABI
 *		e_ABIversion	ABI version spec
 *		e_padding		empty space			(should be 0 filled)
 */
typedef union
{
	struct
	{
		u8	e_magic[4];
		u8	e_class;
		u8	e_endianness;
		u8	e_version;
		u8	e_ABI;
		u8	e_ABIversion;
		u8	e_padding[7];
	};
	u8	e_ident[16];
}	
PACKED ELF_Ident;

# define	ELF_MAG0	'\x7f'
# define	ELF_MAG1	'E'
# define	ELF_MAG2	'L'
# define	ELF_MAG3	'F'
# define	ELF_MAGIC	0x464C457F

enum	ELF_ExType
{
	ET_NONE		= 0x00,
	ET_REL		= 0x01,
	ET_EXEC		= 0x02,
	ET_DYN		= 0x03,
	ET_CORE		= 0x04,
	ET_LOOS		= 0xFE00,
	ET_HIOS		= 0xFEFF,
	ET_LOPROC	= 0xFF00,
	ET_HIPROC	= 0xFFFF
};

enum	ELF_Machine
{
    EM_NONE        = 0x00,
    EM_M32         = 0x01,
    EM_SPARC       = 0x02,
    EM_386         = 0x03,
    EM_68K         = 0x04,
    EM_88K         = 0x05,
    EM_IAMCU       = 0x06,
    EM_860         = 0x07,
    EM_MIPS        = 0x08,
    EM_ARM         = 0x28,
    EM_X86_64      = 0x3E,
	EM_AARCH64     = 0xB7,
    EM_RISCV       = 0xF3,
};

enum	ELF_Version
{
	EV_NONE		=	0x00,
	EV_CURRENT	=	0x01,
	EV_NUM		=	0x02,
};

/**
 * @struct	ELF64_Hdr
 *
 * @brief		64-bit ELF Header.
 *
 *		e_ident			Identification bytes
 *		e_type			File type
 *		e_machine		Instruction set architecture
 *		e_version		ELF version	(should always be 1)
 *		e_entry			Program entrypoint address
 *		e_phoff			Program header table offset
 *		e_shoff			Section header table offset
 *		e_flags			Architecture-specific
 *		e_hsize			ELF header size
 *		e_phsize		Program header table entry size
 *		e_phnum			Program header table entry count
 *		e_shsize		Section header table entry size
 *		e_shnum			Section header table entry count
 *		e_shstridx		Section header index for section names
 */
typedef struct
{
	ELF_Ident	e_ident;
	u16			e_type;
	u16			e_machine;
	u32			e_version;
	u64			e_entry;
	u64			e_phoff;
	u64			e_shoff;
	u32			e_flags;
	u16			e_hsize;
	u16			e_phsize;
	u16			e_phnum;
	u16			e_shsize;
	u16			e_shnum;
	u16			e_shstridx;
}
PACKED ELF64_Hdr;

/**
 * @struct	ELF32_Hdr
 *
 * @brief		32-bit ELF Header.
 *
 *		e_ident			Identification bytes
 *		e_type			File type
 *		e_machine		Instruction set architecture
 *		e_version		ELF version	(should always be 1)
 *		e_entry			Program entrypoint address
 *		e_phoff			Program header table offset
 *		e_shoff			Section header table offset
 *		e_flags			Architecture-specific
 *		e_hsize			ELF header size
 *		e_phsize		Program header table entry size
 *		e_phnum			Program header table entry count
 *		e_shsize		Section header table entry size
 *		e_shnum			Section header table entry count
 *		e_shstridx		Section header index for section names
 */
typedef struct
{
	ELF_Ident	e_ident;
	u16			e_type;
	u16			e_machine;
	u32			e_version;
	u32			e_entry;
	u32			e_phoff;
	u32			e_shoff;
	u32			e_flags;
	u16			e_hsize;
	u16			e_phsize;
	u16			e_phnum;
	u16			e_shsize;
	u16			e_shnum;
	u16			e_shstridx;
}
PACKED ELF32_Hdr;

/* PROGRAM HEADER ----------------------------------------------------------- */

enum	ELF_ProgType
{
	PT_NULL			= 0x00000000,
	PT_LOAD			= 0x00000001,
	PT_DYNAMIC		= 0x00000002,
	PT_INTERP		= 0x00000003,
	PT_NOTE			= 0x00000004,
	PT_SHLIB		= 0x00000005,
	PT_PHDR			= 0x00000006,
	PT_TLS			= 0x00000007,
	PT_LOOS			= 0x60000000,
	PT_GNU_EH_FRAME	= 0x6474e550,
	PT_GNU_STACK	= 0x6474e551,
	PT_GNU_RELRO	= 0x6474e552,
	PT_GNU_PROPERTY	= 0x6474e553,
	PT_LOSUNW		= 0x6ffffffa,
	PT_SUNWBSS		= 0x6ffffffa,
	PT_SUNWSTACK	= 0x6ffffffb,
	PT_HISUNW		= 0x6fffffff,
	PT_HIOS			= 0x6FFFFFFF,
	PT_LOPROC		= 0x70000000,
	PT_HIPROC		= 0x7FFFFFFF,
};

enum	ELF_ProgFlags
{
	PF_X		= (1 << 0),
	PF_W	= (1 << 1),
	PF_R		= (1 << 2),
	PF_MASKOS	= 0x0FF00000,
	PF_MASKPROC	= 0xF0000000,
};

/**
 * @struct	ELF64_Phdr
 *
 * @brief		64-bit Program Header.
 *
 *		p_type			Segment type
 *		p_flags			Segment flags
 *		p_offset		Offset into the file
 *		p_vaddr			Virtual address of the segment into memory
 *		p_paddr			Physical address of the segment
 *		p_filesize		Size of the segment in file
 *		p_memsize		Size of the segment in memory
 *		p_align			Alignment for p_vaddr
 */
typedef struct
{
	u32	p_type;
	u32	p_flags;
	u64	p_offset;
	u64	p_vaddr;
	u64	p_paddr;
	u64	p_filesize;
	u64	p_memsize;
	u64	p_align;
}
PACKED ELF64_Phdr;

/**
 * @struct	ELF32_Phdr
 *
 * @brief		32-bit Program Header.
 *
 *		p_type			Segment type
 *		p_offset		Offset into the file
 *		p_vaddr			Virtual address of the segment into memory
 *		p_paddr			Physical address of the segment
 *		p_filesize		Size of the segment in file
 *		p_memsize		Size of the segment in memory
 *		p_flags			Segment flags
 *		p_align			Alignment for p_vaddr
 */
typedef struct
{
	u32	p_type;
	u32	p_offset;
	u32	p_vaddr;
	u32	p_paddr;
	u32	p_filesize;
	u32	p_memsize;
	u32	p_flags;
	u32	p_align;
}
PACKED ELF32_Phdr;

/* SECTION HEADER ----------------------------------------------------------- */

enum	ELF_SectionType
{
	SHT_NULL			= 0x0,
	SHT_PROGBITS		= 0x1,
	SHT_SYMTAB			= 0x2,
	SHT_STRTAB			= 0x3,
	SHT_RELA			= 0x4,
	SHT_HASH			= 0x5,
	SHT_DYNAMIC			= 0x6,
	SHT_NOTE			= 0x7,
	SHT_NOBITS			= 0x8,
	SHT_REL				= 0x9,
	SHT_SHLIB			= 0x0A,
	SHT_DYNSYM			= 0x0B,
	SHT_INIT_ARRAY		= 0x0E,
	SHT_FINI_ARRAY		= 0x0F,
	SHT_PREINIT_ARRAY	= 0x10,
	SHT_GROUP			= 0x11,
	SHT_SYMTAB_SHNDX	= 0x12,
	SHT_NUM				= 0x13,
	SHT_LOOS			= 0x60000000,
	SHT_GNU_VERSYM		= 0x6FFFFFF0,
	SHT_GNU_HASH		= 0x6FFFFFF6,
	SHT_GNU_LIBLIST		= 0x6FFFFFF7,
	SHT_GNU_VERDEF		= 0x6FFFFFFD,
	SHT_GNU_VERNEED		= 0x6FFFFFFE,
	SHT_HIOS			= 0xFFFFFFFF,
};

enum	ELF_SectionFlags
{
	SHF_WRITE				= 0x1,
	SHF_ALLOC				= 0x2,
	SHF_EXECINSTR			= 0x4,
	SHF_MERGE				= 0x10,
	SHF_STRINGS				= 0x20,
	SHF_INFO_LINK			= 0x40,
	SHF_LINK_ORDER			= 0x80,
	SHF_OS_NONCONFORMING	= 0x100,
	SHF_GROUP				= 0x200,
	SHF_TLS					= 0x400,
	SHF_MASKOS				= 0x0FF00000,
	SHF_MASKPROC			= 0xF0000000,
	SHF_ORDERED				= 0x4000000,
	SHF_EXCLUDE				= 0x8000000,
	SHF_SOLARIS				= 0x80000000,
};

/**
 * @struct	ELF64_Shdr
 *
 * @brief		64-bit Section Header.
 *
 *		sh_name			Section name. SHT_STRTAB index
 *		sh_type			Section type, see ELF_SectionType
 *		sh_flags		Section flags
 *		sh_addr			Section virtual address
 *		sh_offset		Section offset in file
 *		sh_size			Section size
 *		sh_link			Link to another section
 *		sh_info			Additional information
 *		sh_align		Section alignment
 *		sh_entsize		Entry size if a table is held
 */
typedef struct
{
	u32	sh_name;
	u32	sh_type;
	u64	sh_flags;
	u64	sh_addr;
	u64	sh_offset;
	u64	sh_size;
	u32	sh_link;
	u32	sh_info;
	u64	sh_align;
	u64	sh_entsize;
}	
PACKED ELF64_Shdr;

/**
 * @struct	ELF32_Shdr
 *
 * @brief		32-bit Section Header.
 *
 *		sh_name			Section name. SHT_STRTAB index
 *		sh_type			Section type, see ELF_SectionType
 *		sh_flags		Section flags
 *		sh_addr			Section virtual address
 *		sh_offset		Section offset in file
 *		sh_size			Section size
 *		sh_link			Link to another section
 *		sh_info			Additional information
 *		sh_align		Section alignment
 *		sh_entsize		Entry size if a table is held
 */
typedef struct
{
	u32	sh_name;
	u32	sh_type;
	u32	sh_flags;
	u32	sh_addr;
	u32	sh_offset;
	u32	sh_size;
	u32	sh_link;
	u32	sh_info;
	u32	sh_addralign;
	u32	sh_entsize;
}
PACKED ELF32_Shdr;

/* SYMBOLS ------------------------------------------------------------------ */

enum	ELF_SymbolVisibility
{
	STV_DEFAULT		= 0,
	STV_INTERNAL	= 1,
	STV_HIDDEN		= 2,
	STV_PROTECTED	= 3,
};

enum	ELF_SymbolBind
{
	STB_LOCAL		= 0,
	STB_GLOBAL		= 1,
	STB_WEAK		= 2,
	STB_NUM			= 3,
	STB_LOOS		= 10,
	STB_GNU_UNIQUE	= 10,
	STB_HIOS		= 12,
	STB_LOPROC		= 13,
	STB_HIPROC		= 15, 
};

enum	ELF_SymbolType
{
	STT_NOTYPE		= 0,
	STT_OBJECT		= 1,
	STT_FUNC		= 2,
	STT_SECTION		= 3,
	STT_FILE		= 4,
	STT_COMMON		= 5,
	STT_TLS			= 6,
	STT_NUM			= 7,
	STT_LOOS		= 10,
	STT_GNU_IFUNC	= 10,
	STT_HIOS		= 12,
	STT_LOPROC		= 13,
	STT_HIPROC		= 15,
};

enum	ELF_SectionIndices
{
	SHN_UNDEF		= 0,
	SHN_LORESERVE	= 0xFF00,
	SHN_LOPROC		= 0xFF00,
	SHN_BEFORE		= 0xFF00,
	SHN_AFTER		= 0xFF01,
	SHN_HIPROC		= 0xFF1F,
	SHN_ABS			= 0xFFF1,
	SHN_COMMON		= 0xFFF2,
	SHN_HIRESERVE	= 0xFFFF,
};

/**
 * @struct	ELF64_Sym
 *
 * @brief		64-bit ELF Symbol.
 *
 *		sh_name			Symbol name. SHT_STRTAB index.
 *		st_info			Symbol type and biding. see macros and enums
 *		st_other		Symbol visibility
 *		st_shidx		Section index
 *		st_value		Symbol value
 *		st_size			Symbol size
 */
typedef struct
{
	u32	st_name;
	u8	st_info;
	u8	st_other;
	u16	st_shidx;
	u64	st_value;
	u64	st_size;
}	
PACKED ELF64_Sym;

# define		ELF64_ST_BIND(info)			(((u8)(info)) >> 4)
# define		ELF64_ST_TYPE(info)			((info) & 0xF)
# define		ELF64_ST_INFO(bind, type)	(((bind) << 4) | ((type) & 0xF)))

/**
 * @struct	ELF32_Sym
 *
 * @brief		32-bit ELF Symbol.
 *
 *		sh_name			Symbol name. SHT_STRTAB index
 *		st_value		Symbol value
 *		st_size			Symbol size
 *		st_info			Symbol type and biding. see macros and enums
 *		st_other		Symbol visibility
 *		st_shidx		Section index
 */
typedef struct
{
	u32	st_name;
	u32	st_value;
	u32	st_size;
	u8	st_info;
	u8	st_other;
	u16	st_shidx;
}
PACKED ELF32_Sym;

# define		ELF32_ST_BIND(info)			ELF64_ST_BIND(info)
# define		ELF32_ST_TYPE(info)			ELF64_ST_TYPE(info)
# define		ELF32_ST_INFO(bind, type)	ELF64_ST_INFO(bind, type)

# define		ST_BIND(info)				ELF32_ST_BIND(info)
# define		ST_TYPE(info)				ELF32_ST_TYPE(info)

/* DYNAMIC ------------------------------------------------------------------ */

enum	ELF_DynamicTag
{
	DT_NULL				= 0,
	DT_NEEDED			= 1,
	DT_PLTRELSZ			= 2,
	DT_PLTGOT			= 3,
	DT_HASH				= 4,
	DT_STRTAB			= 5,
	DT_SYMTAB			= 6,
	DT_RELA				= 7,
	DT_RELASZ			= 8,
	DT_RELAENT			= 9,
	DT_STRSZ			= 10,
	DT_SYMENT			= 11,
	DT_INIT				= 12,
	DT_FINI				= 13,
	DT_SONAME			= 14,
	DT_RPATH			= 15,
	DT_SYMBOLIC			= 16,
	DT_REL				= 17,
	DT_RELSZ			= 18,
	DT_RELENT			= 19,
	DT_PLTREL			= 20,
	DT_DEBUG			= 21,
	DT_TEXTREL			= 22,
	DT_JMPREL			= 23,
	DT_BIND_NOW			= 24,
	DT_INIT_ARRAY		= 25,
	DT_FINI_ARRAY		= 26,
	DT_INIT_ARRAYSZ		= 27,
	DT_FINI_ARRAYSZ		= 28,
	DT_RUNPATH			= 29,
	DT_FLAGS			= 30,
	DT_ENCODING			= 32,
	DT_PREINIT_ARRAY	= 32,
	DT_PREINIT_ARRAYSZ	= 33,
	DT_SYMTAB_SHNDX		= 34,
	DT_NUM				= 35,
	DT_LOOS				= 0x6000000d,
	DT_HIOS				= 0x6ffff000,
	DT_LOPROC			= 0x70000000,
	DT_HIPROC			= 0x7fffffff
};

/**
 * @struct	ELF64_Dyn
 *
 * @brief		64-bit Dynamic symbol
 *
 *		d_tag			Contains a symbol-specific tag (see ELF_DynamicTag)
 *		d_un			Contains additional data either as int or addr
 *			d_val			Integer value.
 *			d_ptr			Adress value.
 *
 */
typedef struct
{
	i64	d_tag;
	union
	{
		u64	d_val;
		u64	d_ptr;
	}	d_un;
}	
PACKED ELF64_Dyn;

/**
 * @struct	ELF32_Dyn
 *
 * @brief		32-bit Dynamic symbol
 *
 *		d_tag			Contains a symbol-specific tag (see ELF_DynamicTag)
 *		d_un			Contains additional data either as int or addr
 *			d_val			Integer value.
 *			d_ptr			Adress value.
 *
 */
typedef struct
{
	i32	d_tag;
	union
	{
		u32	d_val;
		u32	d_ptr;
	}	d_un;
}	
PACKED ELF32_Dyn;

/* RELOCATIONS -------------------------------------------------------------- */

/**
 * @struct	ELF64_Rel
 *
 * @brief		64-bit Relocation
 *
 *			r_offset		where the relocation takes place (ET_REL != ET_EXEC)
 *			r_info			Index of symbol table and relocation type
 */
typedef struct
{
	u64	r_offset;
	u64	r_info;
}	
PACKED ELF64_Rel;

/**
 * @struct	ELF32_Rel
 *
 * @brief		32-bit Relocation
 *
 *			r_offset		where the relocation takes place (ET_REL != ET_EXEC)
 *			r_info			Index of symbol table and relocation type
 */
typedef struct
{
	u32	r_offset;
	u32	r_info;
}	
PACKED ELF32_Rel;

/**
 * @struct	ELF64_Rel
 *
 * @brief		64-bit Relocation with addend
 *
 *			r_offset		Location of relocation (ET_REL != ET_EXEC)
 *			r_info			Index of symbol table and relocation type
 *			r_addend		Constant value for address calculation
 */
typedef struct
{
	u64	r_offset;
	u64	r_info;
	i64	r_addend;
}
PACKED ELF64_Rela;

/**
 * @struct	ELF32_Rel
 *
 * @brief		32-bit Relocation with addend
 *
 *			r_offset		Location of relocation (ET_REL != ET_EXEC)
 *			r_info			Index of symbol table and relocation type
 *			r_addend		Constant value for address calculation
 */
typedef struct
{
	u32	r_offset;
	u32	r_info;
	i32	r_addend;
}	
PACKED ELF32_Rela;

/* -------------------------------------------------------------------------- */

#endif	// _CELF_H
