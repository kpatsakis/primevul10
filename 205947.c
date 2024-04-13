Item_bin_string::Item_bin_string(const char *str, uint str_length)
{
  const char *end= str + str_length - 1;
  uchar bits= 0;
  uint power= 1;

  max_length= (str_length + 7) >> 3;
  char *ptr= (char*) sql_alloc(max_length + 1);
  if (!ptr)
    return;
  str_value.set(ptr, max_length, &my_charset_bin);

  if (max_length > 0)
  {
    ptr+= max_length - 1;
    ptr[1]= 0;                     // Set end null for string
    for (; end >= str; end--)
    {
      if (power == 256)
      {
        power= 1;
        *ptr--= bits;
        bits= 0;
      }
      if (*end == '1')
        bits|= power;
      power<<= 1;
    }
    *ptr= (char) bits;
  }
  else
    ptr[0]= 0;

  collation.set(&my_charset_bin, DERIVATION_COERCIBLE);
  fixed= 1;
}