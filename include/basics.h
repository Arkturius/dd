/**
* basics.h
*/

#if !defined (_BASICS_H) 
# define _BASICS_H

# include <unistd.h>
# include <stdio.h>

/* Utilities **************************************************************** */

# if !defined (INTERN)
# define	INTERN	static inline
# endif

# if !defined (UNREACHABLE)
# define	UNREACHABLE()	__builtin_unreachable()
# endif

# if defined(__x86_64__)
#  define	BREAKPOINT	__asm__("int3")
# else
#  define	BREAKPOINT
# endif

# if !defined(FALLTHROUGH)
#  if __has_attribute(fallthrough)
#   define	FALLTHROUGH	__attribute__((fallthrough))
#  else
#   define	FALLTHROUGH
#  endif
#endif

# if !defined (TODO)
#  define	TODO(fmt, ...)													\
	do																		\
	{																		\
		dprintf(STDERR_FILENO, "%s:%d: TODO: " fmt, ##__VA_ARGS__);			\
		abort();															\
	} while (0)
# endif

# if !defined (ONCE)
#  define	ONCE(...)	do { __VA_ARGS__ } while (0)
# endif

# if !defined (SHIFT)
#  define	SHIFT(_ac, _av)	(--(_ac), *(_av)++)
# endif

# if !defined (SWAP)
#  define	SWAP(_a, _b)													\
	({																		\
		typeof(_a)		_pa  = (_a);										\
		typeof(_b)		_pb  = (_b);										\
		typeof(*_pa)	_tmp = *_pa;										\
		*_pa = *_pb;														\
		*_pb = _tmp;														\
	})
# endif

/* Logging ****************************************************************** */

# if defined (BASICS_LOG_STDERR)
#  define	BASICS_LOG_OUTPUT_FD	STDERR_FILENO
# endif
# if !defined (BASICS_LOG_OUTPUT_FD)	
#  define	BASICS_LOG_OUTPUT_FD	STDOUT_FILENO
# endif

# define	LOG(fmt, ...)													\
																			\
	dprintf(BASICS_LOG_OUTPUT_FD, fmt"\n", ##__VA_ARGS__)

# define	LOG_THUNK(fmt, ...)												\
																			\
	LOG("%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

# define	LOG_HEAD(type, fmt, ...)										\
																			\
	LOG_THUNK(LOG_HEAD_ ## type ": " fmt, ##__VA_ARGS__);

# define	LOG_RESET			"\033[0m"
# define	LOG_HEAD_INFO		"\033[36;1mINFO"    LOG_RESET
# define	LOG_HEAD_WARNING	"\033[33;1mWARNING" LOG_RESET
# define	LOG_HEAD_ERROR		"\033[31;1mERROR"   LOG_RESET

# define	INFO(fmt, ...)		LOG_HEAD(INFO,    fmt, ##__VA_ARGS__)
# define	WARNING(fmt, ...)	LOG_HEAD(WARNING, fmt, ##__VA_ARGS__)
# define	ERROR(fmt, ...)		LOG_HEAD(ERROR,   fmt, ##__VA_ARGS__)

/* Types ******************************************************************** */

# include <stdint.h>
# include <stdbool.h>

typedef uint64_t	u64;
typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8;

typedef int64_t		i64;
typedef int32_t		i32;
typedef int16_t		i16;
typedef int8_t		i8;

typedef float		f32;
typedef double		f64;

typedef uintptr_t	uptr;
typedef intptr_t	iptr;

/* Arrays ******************************************************************* */

# include <stdlib.h>

# define	BASICS_ARRAYS_BASE_CAP	256

# define	array_type(_type, _name)										\
																			\
	array__typedef(_name);													\
	array__structdef(_type, _name)

# define	array__typedef(_name)											\
																			\
	typedef struct _name ## _array	_name

# define	array__structdef(_type, _name)									\
																			\
	struct _name ## _array													\
	{																		\
		_type	*arr;														\
		u32		len;														\
		u32		cap;														\
	}

# define	array_fmt		"%s: { %p | %u / %u }"
# define	array_arg(_a)	#_a, (_a)->arr, (_a)->len, (_a)->cap

# define	array_len(_a)	(_a)->len
# define	array_cap(_a)	(_a)->cap

# if defined (BASICS_ARRAYS_NO_ASSERT)
#  define	array__assert(_pred)	(void)0
# else
#  include  <assert.h>
#  define	array__assert(_pred)	assert(_pred)
# endif

# define	array__notoob(_a, _i)	(array__assert(_i < array_len(_a)), _i)
# define	array__notnull(_a, _w)	(array__assert((_a)->arr), (_w))

# define	array__addr(_a, _i)		((_a)->arr + _i)
# define	array_index(_a, _p)		(_p - (_a)->arr)
# define	array_addr(_a, _i)		array__addr(_a, array__notoob(_a, _i))

# define	array__isempty(_a)		(!array_len(_a))
# define	array__safelen(_a)		(array_len(_a) - !array__isempty(_a))

# define	array_begin(_a)			array__addr(_a, 0)
# define	array_end(_a)			array__addr(_a, array_len(_a))
# define	array_rbegin(_a)		array__addr(_a, array__safelen(_a))
# define	array_rend(_a)			array__addr(_a, -1)

# define	array_first(_a)			*array__notnull(_a, array_begin(_a))
# define	array_last(_a)			*array__notnull(_a, array_rbegin(_a))
# define	array_at(_a, _i)		*array__notnull(_a, array_addr(_a, _i))

# define	array__realloc(_a, _added)										\
	ONCE																	\
	(																		\
		const u32	_item_size = sizeof(*(_a)->arr);						\
		u32			_new_cap   = array_cap(_a)								\
			? array_cap(_a)													\
			: BASICS_ARRAYS_BASE_CAP;										\
																			\
		while (array_len(_a) + _added > _new_cap)							\
			_new_cap <<= 1;													\
		(_a)->arr = realloc((_a)->arr, _new_cap * _item_size);				\
		array_cap(_a) = _new_cap;											\
	)

# define	array_reserve(_a, _t)											\
	ONCE																	\
	(																		\
		const u32	_add = (_t) - array_cap(_a);							\
																			\
		if ((_t) < array_cap(_a))											\
			break ;															\
		array__realloc(_a, _add);											\
	)

# define	array_empty(_a)	 ONCE(array_len(_a) = 0;)
# define	array_free(_a)													\
	({																		\
		free((_a)->arr);													\
		(_a)->arr = NULL;													\
		array_empty(_a);													\
	})

# define	array_push(_a, _item)											\
	({																		\
		if (array_len(_a) + 1 > array_cap(_a))								\
			array__realloc(_a, 1);											\
		(_a)->arr[array_len(_a)] = (_item);									\
		array_len(_a)++;													\
		array_end(_a);														\
	})

# define	array_pop(_a)													\
	({																		\
		typeof(array_first(_a))	_last = array_last(_a);						\
		array_len(_a)--;													\
		_last;																\
	})

# define	array_remove(_a, _i)											\
	({																		\
		typeof(array_addr(_a, _i)) _last = array_rbegin(_a);				\
		typeof(array_addr(_a, _i)) _elem = array_addr(_a, _i);				\
		SWAP(_last, _elem);													\
		array_pop(_a);														\
	})

# define	array_iter(_a, _it)												\
																			\
	typeof((_a)->arr) (_it) = array_begin(_a)

# define	array_riter(_a, _it)											\
																			\
	typeof((_a)->arr) (_it) = array_rbegin(_a)

# define	array_iter_bound(_a, _it)	(_it) && (_it) != array_end(_a)
# define	array_riter_bound(_a, _it)	(_it) && (_it) != array_rend(_a)

/**
 * These macros permit iteration on a fixed array.
 * You may NOT push/pop into the iterated array while in the loop body.
 */
# define	array_foreach(_it, _a)											\
																			\
	for (array_iter(_a, _it); array_iter_bound(_a, _it); (_it)++)

# define	array_rforeach(_it, _a)											\
																			\
	for (array_riter(_a, _it); array_riter_bound(_a, _it); (_it)--)

/**
 * This creates a non-owning memory view of the array at a certain index.
 * You may NOT pass the result of an array_slice() call to array_free().
 * You may NOT push any new element into a so-called slice.
 */
# define	array_slice(_a, _s, _c)											\
	({																		\
		const u32	_safe_start = (_s) > array_len(_a)						\
			? array_len(_a)													\
			: (_s);															\
		const u32	_safe_max   = array_len(_a) - _safe_start;				\
		const u32	_safe_count = (_c) > _safe_max							\
			? _safe_max														\
			: (_c);															\
																			\
		(typeof(*_a))														\
		{																	\
			.arr = array__addr(_a, _safe_start),							\
			.len = _safe_count,												\
			.cap = _safe_count,												\
		};																	\
	})

array_type(char, Buffer);

# define	buf_appendc(_b, _c)	array_push(_b, _c)
# define	buf_end(_b)			buf_appendc(_b, 0)
# define	buf_len(_b)			array_len(_b)
# define	buf_reset(_b)		array_len(_b) = 0
# define	buf_append(_b, _s)												\
	({																		\
		const u32	_len = strlen(_s);										\
																			\
		array_reserve(_b, array_len(_b) + _len);							\
		memcpy(array_end(_b), _s, _len);									\
		array_len(_b) += _len;												\
	})

# define	buf_appendf(_b, _fmt, ...)										\
	({																		\
		const u32	_len = snprintf(NULL, 0, _fmt, ##__VA_ARGS__);			\
																			\
		array_reserve(_b, array_len(_b) + _len);							\
		snprintf(array_end(_b), _len + 1, _fmt, ##__VA_ARGS__);				\
		array_len(_b) += _len;												\
	})

# define	buf_print(_b)													\
	({																		\
		buf_end(_b);														\
		printf("%s", array_begin(_b));										\
	})

# define	buf_with(_b)													\
	for																		\
	(																		\
		Buffer _b = {0};													\
		array__isempty(&_b);												\
		array_free(&_b), array_len(&_b)++									\
	)

#endif
