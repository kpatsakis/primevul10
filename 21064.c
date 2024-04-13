size_column_sort_func (GtkTreeModel *model,
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
        	if (file_data_is_dir (fdata1))
                	result = fdata1->dir_size - fdata2->dir_size;
        	else
        		result = fdata1->size - fdata2->size;
        }
        else {
        	result = file_data_is_dir (fdata1) ? -1 : 1;
        	if (sort_order == GTK_SORT_DESCENDING)
        		result = -1 * result;
        }

	return result;
}