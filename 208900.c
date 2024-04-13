BOOL enumparam(int n, char *param, char *value) {
   param[0] = value[0] = 0;

   if (n >= MAX_PARAM)
      return FALSE;

   if (_param[n][0] == 0)
      return FALSE;

   strcpy(param, _param[n]);
   strcpy(value, _value[n]);

   return TRUE;
}