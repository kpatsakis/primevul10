int exist_top_group() {
   int i;
   char str[256];

   for (i = 0;; i++) {
      if (!enumcfg("global", str, sizeof(str), NULL, 0, i))
         break;
      str[9] = 0;
      if (strieq(str, "top group"))
         return 1;
   }

   return 0;
}