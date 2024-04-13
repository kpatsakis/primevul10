path_column_sort_func (GtkTreeModel *model,
		       GtkTreeIter  *a,
		       GtkTreeIter  *b,
		       gpointer      user_data)
{
	FileData *fdata1;
	FileData *fdata2;
	char     *path1;
	char     *path2;
	int       result;

	gtk_tree_model_get (model, a, COLUMN_FILE_DATA, &fdata1, COLUMN_PATH, &path1, -1);
	gtk_tree_model_get (model, b, COLUMN_FILE_DATA, &fdata2, COLUMN_PATH, &path2, -1);

	result = strcmp (path1, path2);
	if (result == 0)
		result = strcmp (fdata1->sort_key, fdata2->sort_key);

	g_free (path1);
	g_free (path2);

	return result;
}