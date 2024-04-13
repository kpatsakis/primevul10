int fnmatch1(const char *pattern, const char *string) {
   char c, test;

   for (;;)
      switch (c = *pattern++) {
         case EOS:
            return (*string == EOS ? 0 : 1);
         case '?':
            if (*string == EOS)
               return (1);
            ++string;
            break;
         case '*':
            c = *pattern;
            /* Collapse multiple stars. */
            while (c == '*')
               c = *++pattern;

            /* Optimize for pattern with * at end or before /. */
            if (c == EOS)
               return (0);

            /* General case, use recursion. */
            while ((test = *string) != EOS) {
               if (!fnmatch1(pattern, string))
                  return (0);
               ++string;
            }
            return (1);
            /* FALLTHROUGH */
         default:
            if (c != *string)
               return (1);
            string++;
            break;
      }
}