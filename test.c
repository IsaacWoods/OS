#include <stdio.h>
#include <stdbool.h>

#ifndef __unix__
  #error "Fuck you compile me properly"
#endif

char* itoa(long int value, char* buffer, int base)
{
  unsigned int i = 0u;
  bool isNegative = false;

  // Explicity handle 0
  if (value == 0)
  {
    buffer[i++] = '0';
    buffer[i] = '\0';
    return buffer;
  }

  if (value < 0 && base == 10)
  {
    isNegative = true;
    value = -value;
  }

  while (value != 0)
  {
    long int rem = value % base;
    buffer[i++] = (rem > 9) ? (rem-10)+'a' : rem+'0';
    value /= base;
  }

  if (isNegative)
  {
    buffer[i++] = '-';
  }

  buffer[i] = '\0';

  // Reverse the buffer
  i--;
  for (unsigned int j = 0;
       j < i;
       j++, i--)
  {
    char temp = buffer[j];
    buffer[j] = buffer[i];
    buffer[i] = temp;
  }

  return buffer;
}
int main()
{
  char buffer[1024u] = {};
  itoa(0xDEADBABA, buffer, 16);
  printf("0xDEADBABA (Userspace): %s\n", buffer);

  char b[1024u];
  itoa(12345, b, 10);
  printf("12345: %s\n", b);
  return 0;
}
