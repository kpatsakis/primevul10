BOOL convert_password_file(char *file_name) {
   char name[256], password[256], full_name[256], email[256], email_notify[256];
   int i, len, fh, status;
   char *buf, *p;
   PMXML_NODE root, list, node, npwd;

   printf("Converting password file \"%s\" to new XML format ... ", file_name);

   fh = open(file_name, O_RDONLY | O_BINARY);
   if (fh < 0)
      return FALSE;
   len = lseek(fh, 0, SEEK_END);
   lseek(fh, 0, SEEK_SET);
   buf = xmalloc(len + 1);
   assert(buf);
   i = my_read(fh, buf, len);
   buf[i] = 0;
   close(fh);

   /* create backup */
   strlcpy(name, file_name, sizeof(name));
   strlcat(name, "_bak", sizeof(name));
   fh = open(name, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, 0644);
   if (fh > 0) {
      write(fh, buf, len);
      close(fh);
   }

   p = buf;

   /* skip leading spaces or new lines */
   while (*p && isspace(*p))
      p++;

   root = mxml_create_root_node();
   list = mxml_add_node(root, "list", NULL);

   while (*p) {

      /* skip comment lines */
      if (*p != ';' && *p != '#') {
         for (i = 0; i < (int) sizeof(name) - 1 && *p && *p != ':'; i++)
            name[i] = *p++;
         name[i] = 0;
         if (*p++ != ':') {
            xfree(buf);
            return FALSE;
         }

         for (i = 0; i < (int) sizeof(password) - 1 && *p && *p != ':'; i++)
            password[i] = *p++;
         password[i] = 0;
         if (*p++ != ':') {
            xfree(buf);
            return FALSE;
         }

         for (i = 0; i < (int) sizeof(full_name) - 1 && *p && *p != ':'; i++)
            full_name[i] = *p++;
         full_name[i] = 0;
         if (*p++ != ':') {
            xfree(buf);
            return FALSE;
         }

         for (i = 0; i < (int) sizeof(email) - 1 && *p && *p != ':'; i++)
            email[i] = *p++;
         email[i] = 0;
         if (*p++ != ':') {
            xfree(buf);
            return FALSE;
         }

         for (i = 0; i < (int) sizeof(email_notify) - 1 && *p && *p != '\r' && *p != '\n'; i++)
            email_notify[i] = *p++;
         email_notify[i] = 0;
         if (*p && *p != '\n' && *p != '\r') {
            xfree(buf);
            return FALSE;
         }

         while (*p && (*p == '\r' || *p == '\n'))
            p++;

         node = mxml_add_node(list, "user", NULL);
         mxml_add_node(node, "name", name);
         npwd = mxml_add_node(node, "password", password);
         mxml_add_attribute(npwd, "encoding", "SHA256");
         mxml_add_node(node, "full_name", full_name);
         mxml_add_node(node, "last_logout", "0");
         mxml_add_node(node, "last_activity", "0");
         mxml_add_node(node, "email", email);
         mxml_add_node(node, "email_notify", email_notify);
         mxml_add_node(node, "inactive", "0");
      }

      while (*p && isspace(*p))
         p++;
   }

   status = mxml_write_tree(file_name, root);
   mxml_free_tree(root);
   xfree(buf);

   if (status)
      printf("Ok\n");
   else {
      printf("Error writing to password file\n");
      return FALSE;
   }

   return TRUE;
}