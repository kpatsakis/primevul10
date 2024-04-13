int remove_user(LOGBOOK *lbs, char *user) {
   char file_name[256], str[1000], str2[1000];
   PMXML_NODE node;

   if (lbs->pwd_xml_tree == NULL) {
      show_error("No password file loaded");
      return FALSE;
   }

   sprintf(str, "/list/user[name=%s]", user);
   node = mxml_find_node(lbs->pwd_xml_tree, str);
   if (node == NULL) {
      sprintf(str, loc("User \"%s\" not found in password file"), user);
      strencode2(str2, str, sizeof(str2));
      show_error(str2);
      return FALSE;
   }

   mxml_delete_node(node);

   if (get_password_file(lbs, file_name, sizeof(file_name))) {
      if (!mxml_write_tree(file_name, lbs->pwd_xml_tree)) {
         sprintf(str, loc("Cannot write to file <b>%s</b>"), file_name);
         strcat(str, ": ");
         strcat(str, strerror(errno));
         show_error(str);
         return FALSE;
      }
   }

   return TRUE;
}