void unsetparam(char *param) {
   int i;

   for (i = 0; i < MAX_PARAM; i++)
      if (strieq(param, _param[i]))
         break;

   if (i < MAX_PARAM) {
      for (; i < MAX_PARAM - 1; i++) {
         strlcpy(_param[i], _param[i + 1], NAME_LENGTH);
         strlcpy(_value[i], _value[i + 1], NAME_LENGTH);
      }
      _param[MAX_PARAM - 1][0] = 0;
      _value[MAX_PARAM - 1][0] = 0;
   }
}