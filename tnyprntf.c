/****************************************************************/
/*                                                              */
/*                            prf.c                             */
/*                                                              */
/*                  Abbreviated printf Function                 */
/*                                                              */
/*                      Copyright (c) 1995                      */
/*                      Pasquale J. Villani                     */
/*                      All Rights Reserved                     */
/*                                                              */
/* This file is part of DOS-C.                                  */
/*                                                              */
/* DOS-C is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* DOS-C is distributed in the hope that it will be useful, but */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with DOS-C; see the file COPYING.  If not,     */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/****************************************************************/

#ifndef __GNUC__
#include <io.h>
#else
#define far __far
#endif

#include <dos.h>
#include <stdarg.h>

#include "tnyprntf.h"

#define FALSE 0
#define TRUE 1

static char *charp = 0;

static void handle_char(int);
static void put_console(int);
static char *ltob(long, char *, int);
static int do_printf(const char *, register va_list);

static int fstrlen(char far * s)
{
  int i = 0;

  while (*s++)
    i++;

  return i;
}

static void writechar(char c)
{
  union REGS r;

  r.h.ah = 0x02; /* direct character output */
  r.h.dl = c;
  intdos(&r, &r);
}

static void put_console(int c)
{
  if (c == '\n')
    put_console('\r');

  /* write(1, &c, 1); */             /* write character to stdout */
  writechar(c);
}

/* special handler to switch between sprintf and printf */
static void handle_char(int c)
{
  if (charp == 0)
    put_console(c);
  else
    *charp++ = c;
}

/* ltob -- convert an long integer to a string in any base (2-16) */
static char *ltob(long n, char * s, int base)
{
  unsigned long u;
  char *p, *q;
  int c;

  u = n;

  if (base == -10)              /* signals signed conversion */
  {
    base = 10;
    if (n < 0)
    {
      u = -n;
      *s++ = '-';
    }
  }

  p = q = s;
  do
  {                             /* generate digits in reverse order */
    *p++ = "0123456789ABCDEF"[(unsigned short) (u % base)];
  }
  while ((u /= base) > 0);

  *p = '\0';                    /* terminate the string */
  while (q < --p)
  {                             /* reverse the digits */
    c = *q;
    *q++ = *p;
    *p = c;
  }
  return s;
}

#define LEFT    0
#define RIGHT   1

/* printf -- short version of printf to conserve space */
int Tprintf(const char * fmt, ...)
{
  va_list arg;

  va_start(arg, fmt);
  charp = 0;
  return do_printf(fmt, arg);
}

int Tsprintf(char * buff, const char * fmt, ...)
{
  va_list arg;

  va_start(arg, fmt);
  charp = buff;
  do_printf(fmt, arg);
  handle_char(0);
  return charp - buff - 1;
}

static int do_printf(const char * fmt, va_list arg)
{
  int base;
  char s[11];
  char far * p;
  int c, flag, size, fill;
  int longarg;
  long currentArg;

  while ((c = *fmt++) != '\0')
  {
    if (c != '%')
    {
      handle_char(c);
      continue;
    }

    longarg = FALSE;
    size = 0;
    flag = RIGHT;
    fill = ' ';

    if (*fmt == '-')
    {
      flag = LEFT;
      fmt++;
    }

    if (*fmt == '0')
    {
      fill = '0';
      fmt++;
    }

    while (*fmt >= '0' && *fmt <= '9')
    {
      size = size * 10 + (*fmt++ - '0');
    }

    if (*fmt == 'l')
    {
      longarg = TRUE;
      fmt++;
    }

    c = *fmt++;
    switch (c)
    {
      case '\0':
        return 0;

      case 'c':  
        handle_char(va_arg(arg, int));
        continue;

      case '%':         /* added 2005 */
        handle_char('%');
        continue;

      case 'p':
        {
          unsigned short w0 = va_arg(arg, unsigned int);
          unsigned short w1 = va_arg(arg, unsigned int);
          char *tmp = charp;
          Tsprintf(s, "%04x:%04x", w1, w0);
          p = s;
          charp = tmp;
          goto do_outputstring;
        }

      case 's':
        p = va_arg(arg, char *);
        goto do_outputstring;

      case 'F':
        fmt++;
        /* we assume %Fs here */
      case 'S':
        p = va_arg(arg, char far *);
        goto do_outputstring;

      case 'i':
      case 'd':
        base = -10;
        goto lprt;

      case 'o':
        base = 8;
        goto lprt;

      case 'u':
        base = 10;
        goto lprt;

      case 'X':
      case 'x':
        base = 16;

      lprt:
        if (longarg)
          currentArg = va_arg(arg, long);
        else
          currentArg = base < 0 ? (long)va_arg(arg, int) :
              (long)va_arg(arg, unsigned int);
        ltob(currentArg, s, base);

        p = s;
      do_outputstring:

        size -= fstrlen(p);

        if (flag == RIGHT)
        {
          for (; size > 0; size--)
            handle_char(fill);
        }
        for (; *p != '\0'; p++)
          handle_char(*p);

        for (; size > 0; size--)
          handle_char(fill);

        continue;

      default:
        handle_char('?');

        handle_char(c);
        break;

    }
  }
  va_end(arg);
  return 0;
}

