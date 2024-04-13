void show_browser(char *br) {
   if (stristr(br, "opera"))
      rsprintf("var browser = \"Opera\";\n");
   else if (stristr(br, "konqueror"))
      rsprintf("var browser = \"Konqueror\";\n");
   else if (stristr(br, "Safari"))
      rsprintf("var browser = \"Safari\";\n");
   else if (stristr(br, "MSIE"))
      rsprintf("var browser = \"MSIE\";\n");
   else if (stristr(br, "Mozilla"))
      rsprintf("var browser = \"Mozilla\";\n");
   else
      rsprintf("var browser = \"Other\";\n");
}