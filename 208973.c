BOOL isparam(char *param) {
   int i;

   if (strieq(param, "text"))
      return _mtext[0] != 0;

   for (i = 0; i < MAX_PARAM && _param[i][0]; i++)
      if (strieq(param, _param[i]))
         break;

   if (i < MAX_PARAM && _param[i][0])
      return TRUE;

   return FALSE;
}