void rsputs3(const char *text) {
   int i;
   char str[2];

   str[1] = 0;
   for (i = 0; i < (int) strlen(text); i++) {
      switch (text[i]) {
         case '<':
            rsputs("&lt;");
            break;
         case '>':
            rsputs("&gt;");
            break;
         case '&':
            rsputs("&amp;");
            break;
         case '\"':
            rsputs("&quot;");
            break;

         default:
            str[0] = text[i];
            rsputs(str);
      }
   }
}