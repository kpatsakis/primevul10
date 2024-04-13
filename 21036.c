type_column_sort_func (GtkTreeModel *model,
		       GtkTreeIter  *a,
		       GtkTreeIter  *b,
		       gpointer      user_data)
{
	FileData    *fdata1;
	FileData    *fdata2;
	GtkSortType  sort_order;
	int          result;

	gtk_tree_sortable_get_sort_column_id (GTK_TREE_SORTABLE (model), NULL, &sort_order);

	gtk_tree_model_get (model, a, COLUMN_FILE_DATA, &fdata1, -1);
	gtk_tree_model_get (model, b, COLUMN_FILE_DATA, &fdata2, -1);

	if (file_data_is_dir (fdata1) == file_data_is_dir (fdata2)) {
        	if (file_data_is_dir (fdata1)) {
                	result = strcmp (fdata1->sort_key, fdata2->sort_key);
                	if (sort_order == GTK_SORT_DESCENDING)
                		result = -1 * result;
        	}
        	else {
        		const char  *desc1, *desc2;

        		desc1 = g_content_type_get_description (fdata1->content_type);
        		desc2 = g_content_type_get_description (fdata2->content_type);
        		result = strcasecmp (desc1, desc2);
        		if (result == 0)
        			result = strcmp (fdata1->sort_key, fdata2->sort_key);
        	}
        }
        else {
        	result = file_data_is_dir (fdata1) ? -1 : 1;
        	if (sort_order == GTK_SORT_DESCENDING)
        		result = -1 * result;
        }

	return result;
}