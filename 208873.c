void strencode(char *text) {
   int i;

   for (i = 0; i < (int) strlen(text); i++) {
      switch (text[i]) {
         case '\n':
            rsprintf("<br>\n");
            break;
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
         case ' ':
            rsprintf("&nbsp;");
            break;

            /* the translation for the search highliting */

         case '\001':
            rsprintf("<");
            break;
         case '\002':
            rsprintf(">");
            break;
         case '\003':
            rsprintf("\"");
            break;
         case '\004':
            rsprintf(" ");
            break;

         default:
            rsprintf("%c", text[i]);
      }
   }
}