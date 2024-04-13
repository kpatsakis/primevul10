parse_uuid (const char *str, uint32_t *uuid)
{
  size_t hit = 0; /* number of Hex digIT */
  char c;
  size_t i;
  int n;

  for (i = 0; i < 4; i++)
    uuid[i] = 0;

  while ((c = *str++)) {
    if (c >= '0' && c <= '9')
      n = c - '0';
    else if (c >= 'a' && c <= 'f')
      n = 10 + c - 'a';
    else if (c >= 'A' && c <= 'F')
      n = 10 + c - 'A';
    else if (strchr (":. -", c))
      continue;
    else
      return -1;

    if (hit < 32) {
      uuid[hit / 8] <<= 4;
      uuid[hit / 8] += n;
    }
    hit++;
  }
  if (hit == 32) return 0;

  return -1;
}