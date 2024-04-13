int is_draft(LOGBOOK *lbs, int message_id) {
   char draft[256];

   el_retrieve(lbs, message_id, NULL, NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL,
               NULL, NULL, draft);
   return draft[0];
}