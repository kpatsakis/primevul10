void btou(char *str)
/* convert all blanks to underscores in a string */
{
   int i;

   for (i = 0; i < (int) strlen(str); i++)
      if (str[i] == ' ')
         str[i] = '_';
}