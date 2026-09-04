/**
 * bitset.h
 */

#if !defined (_BITSET_H)
# define _BITSET_H

#include <assert.h>

typedef unsigned long long	BitsetChunk;

# if !defined (BITSET_CAP)
#  define BITSET_CAP	256
# endif

# define	BITSET_CHUNK_SIZE	sizeof(BitsetChunk)
# define	BITSET_CHUNK_BITS	(BITSET_CHUNK_SIZE << 3)
# define	BITSET_SIZE			(BITSET_CAP / BITSET_CHUNK_BITS)

typedef struct Bitset
{
	BitsetChunk bits[BITSET_SIZE];
}
Bitset;

# define	BIT_CHUNK(_bit)	((_bit) / BITSET_CHUNK_BITS)
# define	BIT_MASK(_bit)	(1ull << ((_bit) % BITSET_CHUNK_BITS))
# define	BIT_SET(_set)	((_set).bits)

static_assert(sizeof(unsigned long) == 8, "unsigned long is not 64bit.");

# define	bit_set(_set, _bit)								\
	(BIT_SET(_set)[BIT_CHUNK(_bit)] |=  BIT_MASK(_bit))

# define	bit_clr(_set, _bit)								\
	(BIT_SET(_set)[BIT_CHUNK(_bit)] &= ~BIT_MASK(_bit))

# define	bit_test(_set, _bit)							\
	(BIT_SET(_set)[BIT_CHUNK(_bit)] &   BIT_MASK(_bit))

# define	bitset_clear(_set, _bit)						\
	do														\
	{														\
		for (unsigned int _i = 0; _i < BITSET_SIZE; ++_i)	\
			BIT_SET(_set)[_i] = 0ull;						\
	} while (0)

#endif // _BITSET_H
