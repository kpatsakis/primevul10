void stou(char *str)
/* convert all special characters to underscores in a string */
{
   int i;

   for (i = 0; i < (int) strlen(str); i++)
      if (!isalnum(str[i]))
         str[i] = '_';
}