/* repr - a representation utility.
 *
 * Author: Jaime Castro Enrique.
 */

#include "66/66core.h"

typedef enum op_s
{
  CHARS	  = 'c',
  DOUBLE  = 'f',
  BINARY  = 'b',
  DECIMAL = 'd',
  HEX	  = 'x',
  NATIVE  = 'n',
  SWAPPED = 's',
} op_t;

_66_INLINE_PRAGMA _66_CONSTF _66_HOTF int
_66isdigit (int c)
{
  return (unsigned)c - '0' < 10;
}

_66_INLINE_PRAGMA _66_CONSTF _66_HOTF int
_66isxdigit (int c)
{
  return _66isdigit (c) || ((unsigned)c | 32) - 'a' < 6;
}

#define _66PRINTBNH                                                           \
  _66_SC_WRITE (0x1,                                                          \
				"7......6.......5........4.......3.......2.......1.......0.." \
				"..... (Mathematical layout)\n",                              \
				87)

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define _66PRINTBREPRH                                                        \
  _66_SC_WRITE (0x1,                                                          \
				"LSB....1.......2........3.......4.......5.......6.......7.." \
				"..MSB (LE byte layout)\n",                                   \
				82)
#define _66PRINTBREPRHS                                                       \
  _66_SC_WRITE (0x1,                                                          \
				"MSB....7.......6........5.......4.......3.......2.......1.." \
				"..LSB (BE byte layout)\n",                                   \
				82)
#else
#define _66PRINTBREPRH                                                        \
  _66_SC_WRITE (0x1,                                                          \
				"MSB....7.......6........5.......4.......3.......2.......1.." \
				"..LSB (BE byte layout)\n",                                   \
				82)

#define _66PRINTBREPRHS                                                       \
  _66_SC_WRITE (0x1,                                                          \
				"LSB....1.......2........3.......4.......5.......6.......7.." \
				"..MSB (LE byte layout)\n",                                   \
				82)
#endif

_66_INLINE_PRAGMA void
phelp (void)
{
  _66_WRITEMSG (
	  "usage: repr [CNF] <N>\n\n"
	  "CNF    values=[b(inary)|d(decimal)|x(adecimal)|f(loating point double "
	  "precision)|n(ative repr)|s(wapped native repr)|c(hars)]\n"
	  "       format=\"%c%c\"\n\n"
	  "Note that the float inputs are only designed to be used against 'x' or "
	  "'b', not 'd'\n"
	  "Native and swapped show the exact bit representation the cpu sees, not "
	  "the value\n"
	  "00jc 2025");
}

int
main (int ac, char *av[])
{
  _66_u32 cnf;
  if (ac != 3)
	return (phelp (), EXIT_SUCCESS);
  _66_ASSERT (av[1] != NULL);
  _66_ASSERT (av[2] != NULL);
  cnf = _66_readBE32 (av[1]);
  _66_PANIC (_66_UNLIKELY ((cnf & 0x0000FF00) == 0),
			 "Expected first argument to be length 2");
  _66_PANIC (_66_UNLIKELY ((cnf & 0xFF000000) != 0), "Empty first argument");
  _66_u64				   srcn = 0;
  unsigned char			   tmp	= 0;
  const char *_66_RESTRICT src	= av[2];
  static unsigned char	   _66_ALIGN (64) outputb[64];
  _66_PREFETCH (av[2] + 64ULL, 0, 2);
  switch ((cnf & 0xFF000000) >> 24)
	{
	case CHARS:
	  srcn = _66_readBE64 (av[2]);
	  srcn &= (0xFFFFFFFFFFFFFFFFULL) << (_66_L4B64 (srcn, 0) << 3);
	  srcn = _66_BE2NATIVE (srcn, 64);
	  break;
	case DECIMAL:
	  while (_66isdigit (tmp = (unsigned char)*src++))
		srcn = (srcn << 3) + (srcn << 1) + (_66_u64)(tmp - '0');
	  break;
	case DOUBLE:
	  {
		double x = .0;
		while (_66isdigit (tmp = (unsigned char)*src++))
		  srcn = (srcn << 3) + (srcn << 1) + (_66_u64)(tmp - '0');
		x = (double)srcn;
		if (tmp == '.')
		  {
			double r = .1;
			while (_66isdigit (tmp = (unsigned char)*src++))
			  {
				x += (double)(tmp - '0') * r;
				r *= .1;
			  }
		  }
		srcn = ((_66_f2d_t){ .d = x }).u;
		break;
	  }
	case BINARY:
	  while ((tmp = (_66_u8)*src++ - '0') <= 1)
		srcn = (srcn << 1) | tmp;
	  break;
	case HEX:
	  while (_66isxdigit (tmp = (unsigned char)*src++))
		srcn = (srcn << 4) + (tmp & 0x0FULL) + (tmp >> 6ULL) * 9ULL;
	  break;
	default:
	  _66_WRITEMSG (
		  "Expected one of: 'bdxfc' as values for the lhs of the cnf");
	  _66_EXIT_ERR;
	}
  _66_PREFETCH (outputb + 64ULL, 1, 3);
  __66_MEMSET_TYPE_INLINE (outputb, 0x30, 64ULL);
  _66_u32 x = sizeof (outputb);
  _66_u8  s = 0;
  switch ((cnf & 0x00FF0000) >> 16)
	{
	case DECIMAL:
	  {
		do
		  {
			_66_u64 t = srcn * 0xCCCCCCCDULL >> 35;
			outputb[--x]
				= (unsigned char)(srcn - ((t << 3ULL) + (t << 1ULL))) + '0';
			srcn = t;
		  }
		while (srcn);
		_66_SC_WRITE (0x01, outputb + x, sizeof (outputb) - x);
		_66_SC_WRITE (0x01, "\n", 1);
		break;
	  }
	case BINARY:
	  do
		{
		  outputb[--x] = (srcn & 1) + '0';
		  srcn >>= 1;
		}
	  while (srcn);
	  _66_SC_WRITE (0x01, outputb, sizeof (outputb));
	  _66_SC_WRITE (0x01, "\n", 1);
	  _66PRINTBNH;
	  break;
	case HEX:
	  do
		{
		  unsigned char h = (unsigned char)(srcn & 0x0F);
		  outputb[--x]
			  = (unsigned char)((h & 0x0F) + '0' + ((h + 6) >> 4) * 7);
		  srcn >>= 4;
		}
	  while (srcn);
	  _66_SC_WRITE (0x01, outputb + x, sizeof (outputb) - x);
	  _66_SC_WRITE (0x01, "\n", 1);
	  break;
	case CHARS:
	  {
#define bprint(x)                                                             \
  ((((srcn & (0x00000000000000FFULL << (x))) - (0x20ULL << (x))               \
	 <= (0x5EULL << (x)))                                                     \
	* (srcn & (0x00000000000000FFULL << (x))))                                \
   | (((srcn & (0x00000000000000FFULL << (x))) - (0x20ULL << (x))             \
	   > (0x5EULL << (x)))                                                    \
	  * (0x2EULL << (x))))

		srcn = bprint (0) | bprint (8) | bprint (16) | bprint (24)
			   | bprint (32) | bprint (40) | bprint (48) | bprint (56);
		_66_SC_WRITE (1, &srcn, 8ULL);
		_66_SC_WRITE (1, "\n", 1);
	  }
	  break;
	case SWAPPED:
	  srcn = _66_swap64 (srcn);
	  s	   = 1;
	  _66_FALLTHROUGH;
	case NATIVE:
	  {
		x = 0;
		_66_u64		c;
		_66_RAWMEMR p = _66_PTR_CAST (_66_RAWMEMR, &srcn);
		_66_UNROLL_FULL
		for (size_t i = 0; i < sizeof (srcn); ++i, x += 8)
		  {
			c			   = p[i];
			outputb[x + 0] = ((c >> 7) & 1) + '0';
			outputb[x + 1] = ((c >> 6) & 1) + '0';
			outputb[x + 2] = ((c >> 5) & 1) + '0';
			outputb[x + 3] = ((c >> 4) & 1) + '0';
			outputb[x + 4] = ((c >> 3) & 1) + '0';
			outputb[x + 5] = ((c >> 2) & 1) + '0';
			outputb[x + 6] = ((c >> 1) & 1) + '0';
			outputb[x + 7] = ((c >> 0) & 1) + '0';
		  }
		_66_SC_WRITE (0x01, outputb, sizeof (outputb));
		_66_SC_WRITE (0x01, "\n", 1);
		if (s)
		  _66PRINTBREPRHS;
		else
		  _66PRINTBREPRH;
		break;
	  }
	default:
	  _66_WRITEMSG (
		  "Expected one of: 'bdxnsc' as values for the rhs of the cnf");
	  _66_EXIT_ERR;
	}
  return (EXIT_SUCCESS);
}
