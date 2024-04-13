void show_page_navigation(LOGBOOK *lbs, int n_msg, int page_n, int n_page) {
   int i, num_pages, skip, max_n_msg;
   char ref[256], str[256];

   if (!page_n || n_msg <= n_page)
      return;

   rsprintf("<tr><td class=\"menuframe\"><span class=\"menu3\">\n");

   rsprintf("%s \n", loc("Goto page"));

   if (page_n > 1) {
      sprintf(ref, "page%d", page_n - 1);
      build_ref(ref, sizeof(ref), "", "", "", "");

      rsprintf("<a href=\"%s\">%s</a>&nbsp;&nbsp;", ref, loc("Previous"));
   }

   if (page_n && n_msg > n_page) {
      /* number of pages */
      num_pages = (n_msg - 1) / n_page + 1;
      skip = FALSE;

      for (i = 1; i <= num_pages; i++) {
         sprintf(ref, "page%d", i);
         build_ref(ref, sizeof(ref), "", "", "", "");

         if (i <= 3 || (i >= page_n - 1 && i <= page_n + 1) || i >= num_pages - 2) {
            if (i > 1 && !skip)
               rsprintf(", \n");

            skip = FALSE;
         } else {
            if (!skip)
               rsprintf("&nbsp;...&nbsp;");

            skip = TRUE;
         }

         if (skip)
            continue;

         if (page_n == i)
            rsprintf("%d", i);
         else
            rsprintf("<a href=\"%s\">%d</a>", ref, i);

         /*
            if (i == num_pages )
            rsprintf("&nbsp;&nbsp;\n");
            else
            rsprintf(", ");
          */
      }

      rsprintf("&nbsp;&nbsp;\n");
   }

   if (page_n != -1 && n_page < n_msg && page_n * n_page < n_msg) {
      sprintf(ref, "page%d", page_n + 1);
      build_ref(ref, sizeof(ref), "", "", "", "");

      rsprintf("<a href=\"%s\">%s</a>&nbsp;&nbsp;", ref, loc("Next"));
   }

   if (getcfg(lbs->name, "All display limit", str, sizeof(str)))
      max_n_msg = atoi(str);
   else
      max_n_msg = 500;

   if (page_n != -1 && n_page < n_msg && n_msg < max_n_msg) {
      sprintf(ref, "page");
      build_ref(ref, sizeof(ref), "", "", "", "");

      rsprintf("<a href=\"%s\">%s</a>\n", ref, loc("All"));
   }

   rsprintf("</span></td></tr>\n");
}