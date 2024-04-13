int getcfg(char *group, char *param, char *value, int vsize)
/*
 Read parameter from configuration file.

 - if group == [global] and top group exists, read
 from [global <top group>]

 - if parameter not in [global <top group>], read from [global]

 - if group is logbook, read from logbook section

 - if parameter not in [<logbook>], read from [global <top group>]
 or [global]
 */
{
   char str[256];
   int status;

   /* if group is [global] and top group exists, read from there */
   if (strieq(group, "global") && getcfg_topgroup()) {
      sprintf(str, "global %s", getcfg_topgroup());
      status = getcfg(str, param, value, vsize);
      if (status)
         return status;
   }

   /* first check if parameter is under condition */
   if (_condition[0]) {
      status = getcfg_simple(group, param, value, vsize, TRUE);
      if (status)
         return status;
   }

   status = getcfg_simple(group, param, value, vsize, FALSE);
   if (status)
      return status;

   /* if parameter not found in logbook, look in [global] section */
   if (!group || is_logbook(group))
      return getcfg("global", param, value, vsize);

   return 0;
}