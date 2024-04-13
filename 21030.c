extract_to_data_new (FrWindow *window,
		     GFile    *extract_to_dir)
{
	return extract_data_new (window,
				 NULL,
				 extract_to_dir,
				 NULL,
				 FALSE,
				 TRUE,
				 FALSE,
				 FALSE,
				 FALSE);
}