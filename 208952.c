void str_escape(char *ps, int size)
/********************************************************************\
Encode the given string in-place by adding \\ escapes for `$"\
\********************************************************************/
{
   unsigned char *pd, *p;
   unsigned char str[NAME_LENGTH];

   pd = str;
   p = (unsigned char *) ps;
   while (*p && pd < str + 250) {
      if (strchr("'$\"\\", *p)) {
         *pd++ = '\\';
         *pd++ = *p++;
      } else {
         *pd++ = *p++;
      }
   }
   *pd = '\0';
   strlcpy(ps, (char *) str, size);
}