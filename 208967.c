void el_decode_int(char *message, char *key, char *result, int size) {
   char str[80];

   if (result == NULL)
      return;

   *result = 0;
   el_decode(message, key, str, sizeof(str));
   if (str[0])
      sprintf(str, "%d", atoi(str));
   strlcpy(result, str, size);
}