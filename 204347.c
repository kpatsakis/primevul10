tar_checksum (union block *header, bool silent)
{
  size_t i;
  int unsigned_sum = 0;		/* the POSIX one :-) */
  int signed_sum = 0;		/* the Sun one :-( */
  int recorded_sum;
  int parsed_sum;
  char *p;

  p = header->buffer;
  for (i = sizeof *header; i-- != 0;)
    {
      unsigned_sum += (unsigned char) *p;
      signed_sum += (signed char) (*p++);
    }

  if (unsigned_sum == 0)
    return HEADER_ZERO_BLOCK;

  /* Adjust checksum to count the "chksum" field as blanks.  */

  for (i = sizeof header->header.chksum; i-- != 0;)
    {
      unsigned_sum -= (unsigned char) header->header.chksum[i];
      signed_sum -= (signed char) (header->header.chksum[i]);
    }
  unsigned_sum += ' ' * sizeof header->header.chksum;
  signed_sum += ' ' * sizeof header->header.chksum;

  parsed_sum = from_header (header->header.chksum,
			    sizeof header->header.chksum, 0,
			    0, INT_MAX, true, silent);
  if (parsed_sum < 0)
    return HEADER_FAILURE;

  recorded_sum = parsed_sum;

  if (unsigned_sum != recorded_sum && signed_sum != recorded_sum)
    return HEADER_FAILURE;

  return HEADER_SUCCESS;
}