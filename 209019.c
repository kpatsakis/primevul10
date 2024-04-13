void free_config() {
   int i, j;

   for (i = 0; i < n_lb_config; i++) {
      for (j = 0; j < lb_config[i].n_params; j++) {
         xfree(lb_config[i].config_param[j].param);
         xfree(lb_config[i].config_param[j].uparam);
         xfree(lb_config[i].config_param[j].value);
      }
      if (lb_config[i].config_param)
         xfree(lb_config[i].config_param);
      xfree(lb_config[i].section_name);
   }
   xfree(lb_config);
   lb_config = NULL;
   n_lb_config = 0;
}