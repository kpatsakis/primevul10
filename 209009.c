char *getparam(char *param) {
   int i;

   if (strieq(param, "text"))
      return _mtext;

   if (strieq(param, "cmdline"))
      return _cmdline;

   for (i = 0; i < MAX_PARAM && _param[i][0]; i++)
      if (strieq(param, _param[i]))
         break;

   if (i < MAX_PARAM && _param[i][0])
      return _value[i];

   return NULL;
}