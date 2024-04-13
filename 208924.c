LBLIST *get_subgroup(LBLIST pgrp, char *logbook)
/* retrieve parent of group member "logbook" (which might be group by itself) */
{
   int i;

   for (i = 0; i < pgrp->n_members; i++) {
      /* check if logbook is current member */
      if (strieq(logbook, pgrp->member[i]->name))
         return &(pgrp->member[i]);

      /* check if logbook is in subgroup of current member */
      if (pgrp->member[i]->n_members > 0 && get_subgroup(pgrp->member[i], logbook))
         return get_subgroup(pgrp->member[i], logbook);
   }

   return NULL;
}