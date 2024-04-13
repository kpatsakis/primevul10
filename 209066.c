int el_search_message(LOGBOOK *lbs, int mode, int message_id, BOOL head_only)
/********************************************************************
 Routine: el_search_message

 Purpose: Search for a specific message in a logbook

 Input:
 int   mode              Search mode, EL_FIRST, EL_LAST, EL_NEXT, EL_PREV
 int   message_id        Message id for EL_NEXT and EL_PREV

 Function value:
 int                     New message id

 \********************************************************************/
{
   int i;

   if (lbs->n_el_index == 0)
      return 0;

   if (mode == EL_FIRST) {
      if (head_only) {
         for (i = 0; i < *lbs->n_el_index; i++)
            if (lbs->el_index[i].in_reply_to == 0)
               return lbs->el_index[i].message_id;

         return 0;
      }
      if (*lbs->n_el_index == 0)
         return 0;
      return lbs->el_index[0].message_id;
   }

   if (mode == EL_LAST) {
      if (head_only) {
         for (i = *lbs->n_el_index - 1; i >= 0; i--)
            if (lbs->el_index[i].in_reply_to == 0)
               return lbs->el_index[i].message_id;

         return 0;
      }
      if (*lbs->n_el_index == 0)
         return 0;
      return lbs->el_index[*lbs->n_el_index - 1].message_id;
   }

   if (mode == EL_NEXT) {
      for (i = 0; i < *lbs->n_el_index; i++)
         if (lbs->el_index[i].message_id == message_id)
            break;

      if (i == *lbs->n_el_index)
         return 0;              // message not found

      if (i == *lbs->n_el_index - 1)
         return 0;              // last message

      if (head_only) {
         for (i++; i < *lbs->n_el_index; i++)
            if (lbs->el_index[i].in_reply_to == 0)
               return lbs->el_index[i].message_id;

         return 0;
      }

      return lbs->el_index[i + 1].message_id;
   }

   if (mode == EL_PREV) {
      for (i = 0; i < *lbs->n_el_index; i++)
         if (lbs->el_index[i].message_id == message_id)
            break;

      if (i == *lbs->n_el_index)
         return 0;              // message not found

      if (i == 0)
         return 0;              // first message

      if (head_only) {
         for (i--; i >= 0; i--)
            if (lbs->el_index[i].in_reply_to == 0)
               return lbs->el_index[i].message_id;

         return 0;
      }

      return lbs->el_index[i - 1].message_id;
   }

   return 0;
}