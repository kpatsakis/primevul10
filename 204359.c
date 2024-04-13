value_checker (void *cls,
               enum MHD_ValueKind kind,
               const char *key,
               const char *filename,
               const char *content_type,
               const char *transfer_encoding,
               const char *data,
               uint64_t off,
               size_t size)
{
  int *want_off = cls;
  int idx = *want_off;
  (void) kind;  /* Unused. Silent compiler warning. */

#if 0
  fprintf (stderr,
           "VC: `%s' `%s' `%s' `%s' `%.*s' (%d)\n",
           key, filename, content_type, transfer_encoding,
           (int) size,
           data,
           (int) size);
#endif
  if ( (0 != off) && (0 == size) )
  {
    if (NULL == want[idx + 4])
      *want_off = idx + 5;
    return MHD_YES;
  }
  if ((idx < 0) ||
      (want[idx] == NULL) ||
      (0 != strcmp (key, want[idx])) ||
      (mismatch (filename, want[idx + 1])) ||
      (mismatch (content_type, want[idx + 2])) ||
      (mismatch (transfer_encoding, want[idx + 3])) ||
      (0 != memcmp (data,
                    &want[idx + 4][off],
                    size)))
  {
    *want_off = -1;
    fprintf (stderr,
             "Failed with: `%s' `%s' `%s' `%s' `%.*s'\n",
             key, filename, content_type, transfer_encoding,
             (int) size,
             data);
    fprintf (stderr,
             "Wanted: `%s' `%s' `%s' `%s' `%s'\n",
             want[idx],
             want[idx + 1],
             want[idx + 2],
             want[idx + 3],
             want[idx + 4]);
    fprintf (stderr,
             "Unexpected result: %d/%d/%d/%d/%d/%d/%d\n",
             (idx < 0),
             (want[idx] == NULL),
             (NULL != want[idx]) && (0 != strcmp (key, want[idx])),
             (mismatch (filename, want[idx + 1])),
             (mismatch (content_type, want[idx + 2])),
             (mismatch (transfer_encoding, want[idx + 3])),
             (0 != memcmp (data, &want[idx + 4][off], size)));
    return MHD_NO;
  }
  if ( ( (NULL == want[idx + 4]) &&
         (0 == off + size) ) ||
       (off + size == strlen (want[idx + 4])) )
    *want_off = idx + 5;
  return MHD_YES;
}