int is_group(char *group) {
   int i;

   for (i = 0; i < n_lb_config; i++)
      if (strieq(group, lb_config[i].section_name))
         return 1;
   return 0;
}