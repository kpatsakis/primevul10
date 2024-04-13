int el_index_logbooks() {
   char str[256], data_dir[256], logbook[256], cwd[256], *p;
   int i, j, n, status = 0;

   if (lb_list) {
      for (i = 0; lb_list[i].name[0]; i++) {
         if (lb_list[i].el_index != NULL) {
            xfree(lb_list[i].el_index);
            xfree(lb_list[i].n_el_index);

            /* check if other logbook uses same index */
            for (j = i + 1; lb_list[j].name[0]; j++) {
               /* mark that logbook already freed */
               if (lb_list[j].el_index == lb_list[i].el_index)
                  lb_list[j].el_index = NULL;
            }
         }
      }
      xfree(lb_list);
   }

   /* count logbooks */
   for (i = n = 0;; i++) {
      if (!enumgrp(i, str))
         break;

      if (!is_logbook(str))
         continue;

      n++;
   }

   lb_list = xcalloc(sizeof(LOGBOOK), n + 1);
   for (i = n = 0;; i++) {
      if (!enumgrp(i, logbook))
         break;

      if (!is_logbook(logbook))
         continue;

      /* check for duplicate name */
      for (j = 0; j < i && lb_list[j].name[0]; j++)
         if (strieq(lb_list[j].name, logbook)) {
            eprintf("Error in configuration file: Duplicate logbook \"%s\"\n", logbook);
            return EL_DUPLICATE;
         }

      /* store logbook in list */
      strcpy(lb_list[n].name, logbook);
      strcpy(lb_list[n].name_enc, logbook);
      url_encode(lb_list[n].name_enc, sizeof(lb_list[n].name_enc));

      /* get data dir from configuration file (old method) */
      if (getcfg(logbook, "Data dir", str, sizeof(str))) {
         if (str[0] == DIR_SEPARATOR || str[1] == ':')
            strlcpy(data_dir, str, sizeof(data_dir));
         else {
            strlcpy(data_dir, resource_dir, sizeof(data_dir));
            strlcat(data_dir, str, sizeof(data_dir));
         }
      } else {
         /* use logbook_dir + "Subdir" (new method) */
         strlcpy(data_dir, logbook_dir, sizeof(data_dir));
         if (data_dir[strlen(data_dir) - 1] != DIR_SEPARATOR)
            strlcat(data_dir, DIR_SEPARATOR_STR, sizeof(data_dir));

         if (getcfg(logbook, "Subdir", str, sizeof(str))) {
            if (str[0] == DIR_SEPARATOR)
               strlcpy(data_dir, str, sizeof(data_dir));
            else
               strlcat(data_dir, str, sizeof(data_dir));
         } else
            strlcat(data_dir, logbook, sizeof(data_dir));       /* use logbook name as default */
      }

      if (data_dir[strlen(data_dir) - 1] != DIR_SEPARATOR)
         strlcat(data_dir, DIR_SEPARATOR_STR, sizeof(data_dir));

      /* create data directory if not existing */
      getcwd(cwd, sizeof(cwd));

      j = chdir(data_dir);
      if (j < 0) {
         p = data_dir;
         if (*p == DIR_SEPARATOR) {
            chdir(DIR_SEPARATOR_STR);
            p++;
         }
         if (p[1] == ':') {
            strcpy(str, p);
            if (str[2] == DIR_SEPARATOR)
               str[3] = 0;
            else
               str[2] = 0;

            chdir(str);
            p += strlen(str);
         }

         do {
            if (strchr(p, DIR_SEPARATOR)) {
               strlcpy(str, p, sizeof(str));
               *strchr(str, DIR_SEPARATOR) = 0;
               p = strchr(p, DIR_SEPARATOR) + 1;
            } else {
               strlcpy(str, p, sizeof(str));
               p = NULL;
            }

            j = chdir(str);
            if (j < 0) {

#ifdef OS_WINNT
               j = mkdir(str);
#else
               j = mkdir(str, 0755);
#endif

               if (j == 0) {
                  if (get_verbose() >= VERBOSE_INFO)
                     eprintf("Created directory \"%s\"\n", str);
               } else {
                  eprintf("el_index_logbooks: %s\n", strerror(errno));
                  eprintf("Cannot create directory \"%s\"\n", str);
               }

               chdir(str);
            }

         } while (p && *p);
      }

      chdir(cwd);
      strcpy(lb_list[n].data_dir, data_dir);
      lb_list[n].el_index = NULL;

      /* check if other logbook uses the same directory */
      for (j = 0; j < n; j++)
         if (strcmp(lb_list[j].data_dir, lb_list[n].data_dir) == 0) {
            if (get_verbose() >= VERBOSE_INFO)
               eprintf("Logbook \"%s\" uses same directory as logbook \"%s\"\n", logbook, lb_list[j].name);
            lb_list[n].el_index = lb_list[j].el_index;
            lb_list[n].n_el_index = lb_list[j].n_el_index;
            break;
         }

      if (j == n) {
         if (get_verbose() >= VERBOSE_INFO)
            eprintf("Indexing logbook \"%s\" in \"%s\" ... ", logbook, lb_list[n].data_dir);
         eflush();
         status = el_build_index(&lb_list[n], FALSE);
         if (get_verbose() >= VERBOSE_INFO)
            if (status == EL_SUCCESS)
               eprintf("ok\n");
      }

      if (status == EL_EMPTY) {
         if (get_verbose() >= VERBOSE_INFO)
            eprintf("Found empty logbook \"%s\"\n", logbook);
      } else if (status != EL_SUCCESS) {
         eprintf("Error generating index.\n");
         return status;
      }

      n++;
   }

   /* if top groups defined, set top group in logbook */
   if (exist_top_group()) {
      LBLIST phier;

      phier = get_logbook_hierarchy();
      for (i = 0; i < phier->n_members; i++)
         if (phier->member[i]->is_top)
            for (j = 0; lb_list[j].name[0]; j++)
               if (is_logbook_in_group(phier->member[i], lb_list[j].name))
                  strcpy(lb_list[j].top_group, phier->member[i]->name);

      free_logbook_hierarchy(phier);
   }

   if (!load_password_files())
      return EL_INVAL_FILE;

   return EL_SUCCESS;
}