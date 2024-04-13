int getcfg_simple(char *group, char *param, char *value, int vsize, int conditional) {
   int i, j, status;
   char uparam[256];

   if (strlen(param) >= sizeof(uparam))
      return 0;

   for (i = 0; i < (int) strlen(param); i++)
      uparam[i] = toupper(param[i]);
   uparam[i] = 0;
   value[0] = 0;

   for (i = 0; i < n_lb_config; i++)
      if (strieq(lb_config[i].section_name, group))
         break;

   if (i == n_lb_config)
      return 0;

   for (j = 0; j < lb_config[i].n_params; j++)
      if (match_param(lb_config[i].config_param[j].uparam, uparam, conditional)) {
         status = strchr(lb_config[i].config_param[j].uparam, '{') ? 2 : 1;
         strlcpy(value, lb_config[i].config_param[j].value, vsize);
         return status;
      }

   return 0;
}