int parse_config_file(char *file_name)
/* parse whole config file and store options in sorted list */
{
   char *str, *buffer, *p, *pstr;
   int index, i, j, fh, length;

   str = xmalloc(20000);

   /* open configuration file */
   fh = open(file_name, O_RDONLY | O_BINARY);
   if (fh < 0)
      return 0;
   length = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   buffer = xmalloc(length + 1);
   read(fh, buffer, length);
   buffer[length] = 0;
   close(fh);

   /* release previously allocated memory */
   if (lb_config)
      free_config();

   /* search group */
   p = buffer;
   index = 0;
   do {
      if (*p == '#' || *p == ';') {
         /* skip comment */
         while (*p && *p != '\n' && *p != '\r')
            p++;
      } else if (*p == '[') {
         p++;
         pstr = str;
         while (*p && *p != ']' && *p != '\n' && *p != '\r' && pstr - str < 10000)
            *pstr++ = *p++;
         *pstr = 0;

         /* allocate new group */
         if (!lb_config)
            lb_config = xmalloc(sizeof(LB_CONFIG));
         else
            lb_config = xrealloc(lb_config, sizeof(LB_CONFIG) * (n_lb_config + 1));
         lb_config[n_lb_config].section_name = xmalloc(strlen(str) + 1);
         lb_config[n_lb_config].n_params = 0;
         lb_config[n_lb_config].config_param = NULL;
         strcpy(lb_config[n_lb_config].section_name, str);

         /* enumerate parameters */
         i = 0;
         p = strchr(p, '\n');
         if (p)
            p++;
         while (p && *p && *p != '[') {
            pstr = str;
            while (*p && *p != '=' && *p != '\n' && *p != '\r' && pstr - str < 10000)
               *pstr++ = *p++;
            *pstr-- = 0;
            while (pstr > str && (*pstr == ' ' || *pstr == '\t' || *pstr == '='))
               *pstr-- = 0;

            if (*p == '=') {

               if (lb_config[n_lb_config].n_params == 0)
                  lb_config[n_lb_config].config_param = xmalloc(sizeof(CONFIG_PARAM));
               else
                  lb_config[n_lb_config].config_param = xrealloc(lb_config[n_lb_config].config_param,
                                                                 sizeof(CONFIG_PARAM) *
                                                                 (lb_config[n_lb_config].n_params + 1));
               lb_config[n_lb_config].config_param[i].param = xmalloc(strlen(str) + 1);
               lb_config[n_lb_config].config_param[i].uparam = xmalloc(strlen(str) + 1);

               strcpy(lb_config[n_lb_config].config_param[i].param, str);
               for (j = 0; j < (int) strlen(str); j++)
                  lb_config[n_lb_config].config_param[i].uparam[j] = toupper(str[j]);
               lb_config[n_lb_config].config_param[i].uparam[j] = 0;

               p++;
               while (*p == ' ' || *p == '\t')
                  p++;
               pstr = str;
               while (*p && *p != '\n' && *p != '\r' && pstr - str < 10000)
                  *pstr++ = *p++;
               *pstr-- = 0;
               while (*pstr == ' ' || *pstr == '\t')
                  *pstr-- = 0;

               lb_config[n_lb_config].config_param[i].value = xmalloc(strlen(str) + 1);
               strcpy(lb_config[n_lb_config].config_param[i].value, str);

               i++;
               lb_config[n_lb_config].n_params = i;
            }

            /* search for next line beginning */
            while (*p && *p != '\r' && *p != '\n')
               p++;
            while (*p && (*p == '\r' || *p == '\n'))
               p++;
         }

         /* sort parameter */
         // outcommented: not needed, might screw up group ordering
         //qsort(lb_config[n_lb_config].config_param, lb_config[n_lb_config].n_params, sizeof(CONFIG_PARAM),
         //      param_compare);

         n_lb_config++;
         index++;
      }

      /* search for next line beginning */
      while (*p && *p != '\r' && *p != '\n' && *p != '[')
         p++;
      while (*p && (*p == '\r' || *p == '\n'))
         p++;

   } while (*p);

   xfree(str);
   xfree(buffer);
   return 0;
}