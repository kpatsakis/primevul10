int enumcfg(char *group, char *param, int psize, char *value, int vsize, int index) {
   int i;

   for (i = 0; i < n_lb_config; i++)
      if (strieq(group, lb_config[i].section_name)) {

         if (index < lb_config[i].n_params) {
            strlcpy(param, lb_config[i].config_param[index].param, psize);
            strlcpy(value, lb_config[i].config_param[index].value, vsize);
            return 1;
         }

         return 0;

      }

   return 0;
}