int line_break(char *str, char *encoding) {
   if (strieq(encoding, "plain") || strieq(encoding, "ELCode")) {
      return str[0] == '\n';
   }
   // HTML encoding
   if (strncmp(str, "</p>", 4) == 0 || strncmp(str, "<br>", 4) == 0 || strncmp(str, "<br />", 6) == 0)
      return 1;

   return 0;
}