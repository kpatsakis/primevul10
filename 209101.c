void show_time_selector(int hour, int min, int sec, char *index) {
   int i;

   rsprintf("<select name=\"h%s\">\n", index);

   if (isparam("nsel"))
      rsprintf("<option value=\"<keep>\">- %s -\n", loc("keep original values"));
   else
      rsprintf("<option value=\"\">\n");
   for (i = 0; i < 24; i++)
      if (i == hour)
         rsprintf("<option selected value=\"%d\">%02d\n", i, i);
      else
         rsprintf("<option value=\"%d\">%02d\n", i, i);
   rsprintf("</select>&nbsp;<b>:</b>&nbsp;\n");

   rsprintf("<select name=\"n%s\">", index);
   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 60; i++)
      if (i == min)
         rsprintf("<option selected value=%d>%02d\n", i, i);
      else
         rsprintf("<option value=%d>%02d\n", i, i);
   rsprintf("</select>&nbsp;<b>:</b>&nbsp;\n");

   rsprintf("<select name=\"c%s\">", index);
   rsprintf("<option value=\"\">\n");
   for (i = 0; i < 60; i++)
      if (i == sec)
         rsprintf("<option selected value=%d>%02d\n", i, i);
      else
         rsprintf("<option value=%d>%02d\n", i, i);
   rsprintf("</select>\n");

   rsprintf("\n<script language=\"javascript\" type=\"text/javascript\">\n");
   rsprintf("<!--\n");
   rsprintf("function settime_%s()\n", index);
   rsprintf("{\n");
   rsprintf("  var d = new Date();\n");
   rsprintf("  document.form1.d%s.value = d.getDate();\n", index);
   rsprintf("  document.form1.m%s.value = d.getMonth()+1;\n", index);
   rsprintf("  year = d.getYear();\n");
   rsprintf("  if (year < 1900)\n");
   rsprintf("     year += 1900;\n");
   rsprintf("  document.form1.y%s.value = year;\n", index);
   rsprintf("  document.form1.h%s.value = d.getHours();\n", index);
   rsprintf("  document.form1.n%s.value = d.getMinutes();\n", index);
   rsprintf("  document.form1.c%s.value = d.getSeconds();\n", index);
   rsprintf("}\n\n");

   rsprintf("  document.write(\"&nbsp;&nbsp;\");\n");
   rsprintf("  document.write(\"<a href=\\\"javascript:settime_%s()\\\">\");\n", index);
   rsprintf("  document.writeln(\"<img src=\\\"clock.png\\\" align=\\\"middle\\\" border=\\\"0\\\" ");
   rsprintf("alt=\\\"%s\\\"></a>\");\n", loc("Insert current time"));

   rsprintf("//-->\n");
   rsprintf("</script>\n");

}