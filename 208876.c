int enumgrp(int index, char *group) {
   if (index < n_lb_config) {
      strcpy(group, lb_config[index].section_name);
      return 1;
   }

   return 0;
}