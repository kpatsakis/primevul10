test_parse_range_header (void)
{
  unsigned i;
  static const struct {
    const char * rangehdr;
    const wgint firstbyte;
    const wgint lastbyte;
    const wgint length;
    const bool shouldPass;
  } test_array[] = {
      { "bytes 0-1000/1000", 0, 1000, 1000, false },
      { "bytes 0-999/1000", 0, 999, 1000, true },
      { "bytes 100-99/1000", 100, 99, 1000, false },
      { "bytes 100-100/1000", 100, 100, 1000, true },
      { "bytes 0-1000/100000000", 0, 1000, 100000000, true },
      { "bytes 1-999/1000", 1, 999, 1000, true },
      { "bytes 42-1233/1234", 42, 1233, 1234, true },
      { "bytes 42-1233/*", 42, 1233, -1, true },
      { "bytes 0-2147483648/2147483649", 0, 2147483648U, 2147483649U, true },
#if SIZEOF_WGINT >= 8
      { "bytes 2147483648-4294967296/4294967297", 2147483648U, 4294967296ULL, 4294967297ULL, true },
#endif
  };

  wgint firstbyteptr[sizeof(wgint)];
  wgint lastbyteptr[sizeof(wgint)];
  wgint lengthptr[sizeof(wgint)];
  bool result;
  for (i = 0; i < countof (test_array); i++)
    {
      result = parse_content_range (test_array[i].rangehdr, firstbyteptr, lastbyteptr, lengthptr);
#if 0
      printf ("%ld %ld\n", test_array[i].firstbyte, *firstbyteptr);
      printf ("%ld %ld\n", test_array[i].lastbyte, *lastbyteptr);
      printf ("%ld %ld\n", test_array[i].length, *lengthptr);
      printf ("\n");
#endif
      mu_assert ("test_parse_range_header: False Negative", result == test_array[i].shouldPass);
      mu_assert ("test_parse_range_header: Bad parse", test_array[i].firstbyte == *firstbyteptr &&
                                                       test_array[i].lastbyte == *lastbyteptr &&
                                                       test_array[i].length == *lengthptr);
    }

  return NULL;
}