BOOL is_cond_attr(int index) {
   int i;

   for (i = 0; i < MAX_N_LIST && attr_options[index][i][0]; i++)
      if (strchr(attr_options[index][i], '{') && strchr(attr_options[index][i], '}'))
         return TRUE;

   return FALSE;
}