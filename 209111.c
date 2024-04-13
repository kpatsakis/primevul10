BOOL is_logbook_in_group(LBLIST pgrp, char *logbook)
/* test if "logbook" is in group node plb */
{
   int i;

   if (strieq(logbook, pgrp->name))
      return TRUE;

   for (i = 0; i < pgrp->n_members; i++) {
      if (strieq(logbook, pgrp->member[i]->name))
         return TRUE;

      if (pgrp->member[i]->n_members > 0 && is_logbook_in_group(pgrp->member[i], logbook))
         return TRUE;
   }

   return FALSE;
}