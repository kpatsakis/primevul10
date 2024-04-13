void free_logbook_hierarchy(LBLIST root) {
   int i;

   for (i = 0; i < root->n_members; i++) {
      if (root->member[i]) {
         free_logbook_hierarchy(root->member[i]);
         root->member[i] = NULL;
      }
   }

   xfree(root->member);
   xfree(root);
}