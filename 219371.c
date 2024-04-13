string_base62(unsigned long int value)
{
static uschar yield[7];
uschar *p = yield + sizeof(yield) - 1;
*p = 0;
while (p > yield)
  {
  *(--p) = base62_chars[value % BASE_62];
  value /= BASE_62;
  }
return yield;
}