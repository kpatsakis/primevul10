void evaluate_conditions(LOGBOOK *lbs, char attrib[MAX_N_ATTR][NAME_LENGTH]) {
   char condition[256], str[256];
   int index, i;

   condition[0] = 0;
   set_condition("");
   for (index = 0; index < lbs->n_attr; index++) {
      for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++) {

         if (strchr(attr_options[index][i], '{') && strchr(attr_options[index][i], '}')) {

            strlcpy(str, attr_options[index][i], sizeof(str));
            *strchr(str, '{') = 0;

            if (strieq(str, attrib[index])) {
               strlcpy(str, strchr(attr_options[index][i], '{') + 1, sizeof(str));
               if (*strchr(str, '}'))
                  *strchr(str, '}') = 0;

               if (condition[0] == 0)
                  strlcpy(condition, str, sizeof(condition));
               else {
                  strlcat(condition, ",", sizeof(condition));
                  strlcat(condition, str, sizeof(condition));
               }

               set_condition(condition);
               scan_attributes(lbs->name);
            }
         }
      }
   }
}