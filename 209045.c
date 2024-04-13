int propagate_attrib(LOGBOOK *lbs, int message_id, char attrib[MAX_N_ATTR][NAME_LENGTH]) {
   int n, i, j, status;
   char str[NAME_LENGTH], att_file[MAX_ATTACHMENTS][256], *attr, *list, reply_to[MAX_REPLY_TO * 10];

   list = (char *) xmalloc(MAX_N_ATTR * NAME_LENGTH);
   attr = (char *) xmalloc(MAX_N_ATTR * NAME_LENGTH);

   status = el_retrieve(lbs, message_id, NULL, attr_list, (char (*)[NAME_LENGTH]) attr, lbs->n_attr,
                        NULL, NULL, NULL, reply_to, att_file, NULL, NULL, NULL);
   if (status != EL_SUCCESS) {
      xfree(list);
      xfree(attr);
      return status;
   }

   getcfg(lbs->name, "Propagate attributes", str, sizeof(str));
   n = strbreak(str, (char (*)[1500]) list, MAX_N_ATTR, ",", FALSE);
   for (i = 0; i < n; i++) {
      for (j = 0; j < lbs->n_attr; j++)
         if (stricmp(attr_list[j], list + i * NAME_LENGTH) == 0) {
            strlcpy(attr + j * NAME_LENGTH, attrib[j], NAME_LENGTH);
            break;
         }
   }

   message_id = el_submit(lbs, message_id, TRUE, "<keep>", attr_list, (char (*)[1500]) attr, lbs->n_attr,
                          "<keep>",
                          "<keep>", "<keep>", "<keep>", att_file, TRUE, NULL, NULL);
   if (message_id < 0) {
      xfree(list);
      xfree(attr);
      return 0;
   }

   // go through all replies of this entry
   n = strbreak(reply_to, (char (*)[1500]) list, MAX_N_ATTR, ",", FALSE);
   for (i = 0; i < n; i++)
      propagate_attrib(lbs, atoi(list + i * NAME_LENGTH), attrib);

   xfree(list);
   xfree(attr);

   return EL_SUCCESS;
}