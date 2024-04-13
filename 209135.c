void show_date_selector(int day, int month, int year, char *index) {
   int i;

   rsprintf("<select name=\"m%s\">\n", index);

   if (isparam("nsel"))
      rsprintf("<option value=\"<keep>\">- %s -\n", loc("keep original values"));
   else
      rsprintf("<option value=\"\">\n");
   for (i = 0; i < 12; i++)
      if (i + 1 == month)
         rsprintf("<option selected value=\"%d\">%s\n", i + 1, month_name(i));
      else
         rsprintf("<option value=\"%d\">%s\n", i + 1, month_name(i));
   rsprintf("</select>\n");

   rsprintf("<select name=\"d%s\">", index);

   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 31; i++)
      if (i + 1 == day)
         rsprintf("<option selected value=%d>%d\n", i + 1, i + 1);
      else
         rsprintf("<option value=%d>%d\n", i + 1, i + 1);
   rsprintf("</select>\n");

   rsprintf("&nbsp;%s: ", loc("Year"));
   if (year)
      rsprintf("<input type=\"text\" size=5 maxlength=5 name=\"y%s\" value=\"%d\">", index, year);
   else
      rsprintf("<input type=\"text\" size=5 maxlength=5 name=\"y%s\">", index);

   rsprintf("\n<script language=\"javascript\" type=\"text/javascript\">\n");
   rsprintf("<!--\n");
   rsprintf("function opencal(i)\n");
   rsprintf("{\n");
   rsprintf("  window.open(\"cal.html?i=\"+i, \"\",\n");
   rsprintf("  \"top=280,left=350,width=300,height=195,dependent=yes,menubar=no,status=no,");
   rsprintf("scrollbars=no,location=no,resizable=yes\");\n");
   rsprintf("}\n\n");

   rsprintf("  document.write(\"&nbsp;&nbsp;\");\n");
   rsprintf("  document.write(\"<a href=\\\"javascript:opencal('%s')\\\">\");\n", index);
   rsprintf("  document.writeln(\"<img src=\\\"cal.png\\\" align=\\\"middle\\\" border=\\\"0\\\"");
   rsprintf("alt=\\\"%s\\\"></a>\");\n", loc("Pick a date"));

   rsprintf("//-->\n");
   rsprintf("</script>&nbsp;&nbsp;\n");
}