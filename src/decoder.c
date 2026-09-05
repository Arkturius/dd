/**
* decoder.c
*/

#include <decoder.h>

void
decode_finalize(Decoder *d, Byte b)
{
	(void)d; (void)b;
	TODO("decode_finalize");
}

void
decode_rex_prefix(Decoder *d, Byte b)
{
	(void)d; (void)b;
	TODO("decode_rex_prefix");
}

void
decode_raw_prefix(Decoder *d, Byte b)
{
	(void)d; (void)b;
	TODO("decode_raw_prefix");
}

void
decode_opcode(Decoder *d, Byte b)
{
	(void)d; (void)b;
	TODO("decode_opcode");
}

void
decode_error(Decoder *d, Byte b)
{
	(void)d; (void)b;
	TODO("decode_error");
}

INTERN void
decoder_step(Decoder *d, u32 n)
{
	if (!d)
		return ;
	d->pc += n;
}

INTERN void
decoder_classify(Decoder *d, Byte b)
{
	if (!d)
		return ;
	d->bclass = byte_classes[b];
}

# define	OPCODE_NOP	0x90

DecoderFunc
decoder_switch(Decoder *d)
{
	d->state = decoder_transitions[d->state][d->bclass];
	return decode_handlers[d->state];
}

DecoderError
decode(Bytes *code)
{
	Decoder	dec = {0};
	u32		nop = 0;

	while (dec.pc < array_len(code) && dec.state != STATE_ERROR)
	{
		Byte	b = array_at(code, dec.pc);

		if (b == OPCODE_NOP)
		{
			nop++;
			decoder_step(&dec, 1);
			continue ;
		}
		if (nop > 0)
		{
			INFO("%d NOP instructions skipped.", nop);
			nop = 0;
		}
		INFO("byte = %02x", b);
		decoder_classify(&dec, b);
		decoder_switch(&dec)(&dec, b);
		decoder_step(&dec, 1);
	}
	return DECODE_OK;
}
