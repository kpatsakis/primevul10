void xmlencode(char *text) {
   int i;

   for (i = 0; i < (int) strlen(text); i++) {
      switch (text[i]) {
         case '<':
            rsprintf("&lt;");
            break;
         case '>':
            rsprintf("&gt;");
            break;
         case '&':
            rsprintf("&amp;");
            break;
         case '\"':
            rsprintf("&quot;");
            break;

         default:
            rsprintf("%c", text[i]);
      }
   }
}