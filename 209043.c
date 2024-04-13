int check_edit_time(LOGBOOK *lbs, int message_id) {
   char str[256];
   int i;

   if (is_admin_user(lbs, getparam("unm"))) {
      if (getcfg(lbs->name, "Admin Restrict edit time", str, sizeof(str))) {
         for (i = 0; i < *lbs->n_el_index; i++)
            if (lbs->el_index[i].message_id == message_id)
               break;

         if (i < *lbs->n_el_index && time(NULL) > lbs->el_index[i].file_time + atof(str) * 3600 && atof(str) > 0) {
            sprintf(str, loc("Entry can only be edited %1.2lg hours after creation"), atof(str));
            show_error(str);
            return 0;
         }
      }
   } else {
      if (getcfg(lbs->name, "Restrict edit time", str, sizeof(str))) {
         for (i = 0; i < *lbs->n_el_index; i++)
            if (lbs->el_index[i].message_id == message_id)
               break;

         if (i < *lbs->n_el_index && time(NULL) > lbs->el_index[i].file_time + atof(str) * 3600) {
            sprintf(str, loc("Entry can only be edited %1.2lg hours after creation"), atof(str));
            show_error(str);
            return 0;
         }
      }
   }

   return 1;
}