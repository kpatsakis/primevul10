fetchmode (char const *str)
{
   const char *s;
   mode_t mode;

   while (ISSPACE ((unsigned char) *str))
     str++;

   for (s = str, mode = 0; s < str + 6; s++)
     {
       if (*s >= '0' && *s <= '7')
	mode = (mode << 3) + (*s - '0');
       else
	{
	 mode = 0;
	 break;
	}
     }
   if (*s == '\r')
     s++;
   if (*s != '\n')
     mode = 0;

    /* NOTE: The "diff --git" format always sets the file mode permission
       bits of symlinks to 0.  (On Linux, symlinks actually always have
       0777 permissions, so this is not even consistent.)  */

   return mode;
}