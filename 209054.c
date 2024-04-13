void el_decode(char *message, char *key, char *result, int size) {
   char *pc, *ph;
   int i;

   if (result == NULL)
      return;

   *result = 0;

   ph = strstr(message, "========================================");
   if (ph == NULL)
      return;

   do {
      if (ph[40] == '\r' || ph[40] == '\n')
         break;
      ph = strstr(ph + 40, "========================================");
      if (ph == NULL)
         return;

   } while (1);

   /* go through all lines */
   for (pc = message; pc < ph;) {

      if (strncmp(pc, key, strlen(key)) == 0) {
         pc += strlen(key);
         for (i = 0; *pc != '\n' && *pc != '\r' && i < size - 1; i++)
            result[i] = *pc++;
         result[i] = 0;
         return;
      }

      pc = strchr(pc, '\n');
      if (pc == NULL)
         return;
      while (*pc && (*pc == '\n' || *pc == '\r'))
         pc++;
   }
}