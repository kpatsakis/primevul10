void url_slash_encode(char *ps, int size)
/********************************************************************\
Do the same including '/' characters
 \********************************************************************/
{
   unsigned char *pd, *p;
   unsigned char str[NAME_LENGTH];

   pd = str;
   p = (unsigned char *) ps;
   while (*p && pd < str + 250) {
      if (strchr("%&=#?+<>/", *p) || *p > 127) {
         sprintf((char *) pd, "%%%02X", *p);
         pd += 3;
         p++;
      } else if (*p == ' ') {
         *pd++ = '+';
         p++;
      } else {
         *pd++ = *p++;
      }
   }
   *pd = '\0';
   strlcpy(ps, (char *) str, size);
}