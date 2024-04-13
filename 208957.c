void strextract(const char *str, char delim, char *extr, int size)
/* extract a substinr "extr" from "str" until "delim" is found */
{
   int i;

   for (i = 0; str[i] != delim && i < size - 1; i++)
      extr[i] = str[i];
   extr[i] = 0;
}