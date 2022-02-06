/*
	This test program verifies that the strings are printed correctly
	(or the way, I expect them to print)

	Note: Correct performance on TurboC relies on being compiled for
	the small or tiny model.
	
	If strings are wrong, the program will wait for a keypress
*/

#include <conio.h>
#include <string.h>

#include "../tnyprntf.h"

#ifdef __WATCOMC__
#define bioskey(x) getch()
#endif

struct {
  char *should;
  char *format;
  unsigned lowint;
  unsigned highint;

} testarray[] = {
  {
  "hello world", "%s %s", (unsigned)"hello", (unsigned)"world"},
  {
  "hello", "%3s", (unsigned)"hello", 0},
  {
  "  hello", "%7s", (unsigned)"hello", 0},
  {
  "hello  ", "%-7s", (unsigned)"hello", 0},
  {
  "hello", "%s", (unsigned)"hello", 0},
  {
  "1", "%d", 1, 0},
  {
  "-1", "%d", -1, 0},
  {
  "65535", "%u", -1, 0},
  {
  "-32768", "%d", 0x8000, 0},
  {
  "32767", "%d", 0x7fff, 0},
  {
  "-32767", "%d", 0x8001, 0},
  {
  "8000", "%x", 0x8000, 0},
  {
  "   1", "%4x", 1, 0},
  {
  "0001", "%04x", 1, 0},
  {
  "1   ", "%-4x", 1, 0},
  {
  "1000", "%-04x", 1, 0},
  {
  "1", "%ld", 1, 0},
  {
  "-1", "%ld", -1, -1},
  {
  "65535", "%ld", -1, 0},
  {
  "65535", "%u", -1, 0},
  {
  "8000", "%lx", 0x8000, 0},
  {
  "80000000", "%lx", 0, 0x8000},
  {
  "   1", "%4lx", 1, 0},
  {
  "0001", "%04lx", 1, 0},
  {
  "1   ", "%-4lx", 1, 0},
  {
  "1000", "%-04lx", 1, 0},
  {
  "-2147483648", "%ld", 0, 0x8000},
  {
  "2147483648", "%lu", 0, 0x8000},
  {
  "2147483649", "%lu", 1, 0x8000},
  {
  "-2147483647", "%ld", 1, 0x8000},
  {
  "32767", "%ld", 0x7fff, 0},
  {
  "ptr 1234:5678", "ptr %p", 0x5678, 0x1234},
  {
  "percent character '%'", "percent character '%%'"},
  {0}};

void test(char *should, char *format, unsigned lowint, unsigned highint)
{
  char b[100];

  Tsprintf(b, format, lowint, highint);

  Tprintf("'%s' = '%s'\n", should, b);

  if (strcmp(b, should))
  {
    Tprintf("\nhit ANYKEY\n");
    getch();
  }
}

int main()
{
  int i;

  for (i = 0; testarray[i].should; i++)
  {
    test(testarray[i].should, testarray[i].format, testarray[i].lowint,
         testarray[i].highint);
  }
  return 0;
}
