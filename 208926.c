void compare_attributes(LOGBOOK *lbs, int message_id, char attrib[MAX_N_ATTR][NAME_LENGTH], int *n) {
   int status, i, n_reply;
   char reply_to[MAX_REPLY_TO * 10], *attr, *list;

   attr = xmalloc(MAX_N_ATTR * NAME_LENGTH);
   assert(attr);
   status = el_retrieve(lbs, message_id, NULL, attr_list, (char (*)[NAME_LENGTH]) attr, lbs->n_attr,
                        NULL, NULL, NULL, reply_to, NULL, NULL, NULL, NULL);
   if (status != EL_SUCCESS) {
      xfree(attr);
      return;
   }

   if (*n == 0)
      memcpy(attrib, attr, sizeof(MAX_N_ATTR * NAME_LENGTH));
   else {
      for (i = 0; i < lbs->n_attr; i++)
         if (!strieq(attrib[i], attr + i * NAME_LENGTH))
            sprintf(attrib[i], "- %s -", loc("keep original values"));
   }
   (*n)++;
   if (isparam("elmode") && strieq(getparam("elmode"), "threaded")) {

      list = xmalloc(MAX_N_ATTR * NAME_LENGTH);
      assert(list);

      // go through all replies in threaded mode
      n_reply = strbreak(reply_to, (char (*)[NAME_LENGTH]) list, MAX_N_ATTR, ",", FALSE);
      for (i = 0; i < n_reply; i++) {
         compare_attributes(lbs, atoi(list + i * NAME_LENGTH), attrib, n);

         xfree(list);
      }
   }

   xfree(attr);
}