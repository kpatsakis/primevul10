void strencode2(char *b, const char *text, int size) {
   int i;

   *b = 0;
   for (i = 0; i < (int) strlen(text); i++) {
      switch (text[i]) {
         case '\n':
            if (strlen(b) + 5 >= (unsigned int) size)
               return;
            strcat(b, "<br>\n");
            break;
         case '<':
            if (strlen(b) + 4 >= (unsigned int) size)
               return;
            strcat(b, "&lt;");
            break;
         case '>':
            if (strlen(b) + 4 >= (unsigned int) size)
               return;
            strcat(b, "&gt;");
            break;
         case '\"':
            if (strlen(b) + 6 >= (unsigned int) size)
               return;
            strcat(b, "&quot;");
            break;
         default:
            if (strlen(b) + 1 >= (unsigned int) size)
               return;
            sprintf(b + strlen(b), "%c", text[i]);
      }
   }
}