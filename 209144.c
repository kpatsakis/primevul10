BOOL convert_password_encoding(LOGBOOK *lbs) {
   PMXML_NODE node, pwd;
   int i;
   char str[256], oldpwd[256], file_name[256];

   if (lbs->pwd_xml_tree == NULL)
      return FALSE;

   if ((node = mxml_find_node(lbs->pwd_xml_tree, "/list/user[1]/password")) == NULL)
      return FALSE;

   str[0] = 0;
   if (mxml_get_attribute(node, "encoding") != NULL)
      strlcpy(str, mxml_get_attribute(node, "encoding"), sizeof(str));

   if (!strieq(str, "SHA256")) {
      if ((node = mxml_find_node(lbs->pwd_xml_tree, "/list")) == NULL)
         return FALSE;

      printf("Converting password file for logbook \"%s\" to new encoding ... ", lbs->name);

      for (i = 0; i < mxml_get_number_of_children(node); i++) {
         sprintf(str, "/list/user[%d]/password", i + 1);
         pwd = mxml_find_node(lbs->pwd_xml_tree, str);

         if (pwd && mxml_get_value(pwd)) {
            strlcpy(str, mxml_get_value(pwd), sizeof(str));

            /* assume base64 encoding, might be wrong if HAVE_CRYPT was used */
            base64_decode(str, oldpwd);
            do_crypt(oldpwd, str, sizeof(str));

            mxml_replace_node_value(pwd, str);
            mxml_add_attribute(pwd, "encoding", "SHA256");
         }
      }

      if (get_password_file(lbs, file_name, sizeof(file_name)))
         mxml_write_tree(file_name, lbs->pwd_xml_tree);

      printf("ok\n");
   }

   return TRUE;
}