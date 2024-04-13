test_parse_content_disposition (void)
{
  unsigned i;
  static const struct {
    const char *hdrval;
    const char *filename;
    bool result;
  } test_array[] = {
    { "filename=\"file.ext\"", "file.ext", true },
    { "attachment; filename=\"file.ext\"", "file.ext", true },
    { "attachment; filename=\"file.ext\"; dummy", "file.ext", true },
    { "attachment", NULL, false },
    { "attachement; filename*=UTF-8'en-US'hello.txt", "hello.txt", true },
    { "attachement; filename*0=\"hello\"; filename*1=\"world.txt\"",
      "helloworld.txt", true },
    { "attachment; filename=\"A.ext\"; filename*=\"B.ext\"", "B.ext", true },
    { "attachment; filename*=\"A.ext\"; filename*0=\"B\"; filename*1=\"B.ext\"",
      "A.ext", true },
    { "filename**0=\"A\"; filename**1=\"A.ext\"; filename*0=\"B\";\
filename*1=\"B\"", "AA.ext", true },
  };

  for (i = 0; i < countof (test_array); ++i)
    {
      char *filename;
      bool res;

      res = parse_content_disposition (test_array[i].hdrval, &filename);

      mu_assert ("test_parse_content_disposition: wrong result",
                 res == test_array[i].result
                 && (res == false
                     || 0 == strcmp (test_array[i].filename, filename)));
      xfree (filename);
    }

  return NULL;
}