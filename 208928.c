void el_decode_intlist(char *message, char *key, char *result, int size) {
   int i;

   if (result == NULL)
      return;

   *result = 0;
   el_decode(message, key, result, size);

   /* remove any non allowed characters */
   for (i = 0; i < size && i < (int) strlen(result); i++)
      if (!isdigit(result[i]) && result[i] != ' ' && result[i] != ',')
         result[i] = ' ';
}