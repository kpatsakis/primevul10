fr_archive_progress_cb (FrArchive *archive,
		        double     fraction,
		        FrWindow  *window)
{
	window->priv->progress_pulse = (fraction < 0.0);
	if (! window->priv->progress_pulse) {
		fraction = CLAMP (fraction, 0.0, 1.0);
		if (window->priv->progress_dialog != NULL)
			gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (window->priv->pd_progress_bar), fraction);
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (window->priv->progress_bar), fraction);

		if ((archive != NULL) && (fr_archive_progress_get_total_files (archive) > 0)) {
			char *message = NULL;
			int   remaining_files;

			remaining_files = fr_archive_progress_get_total_files (archive) - fr_archive_progress_get_completed_files (archive);

			switch (window->priv->action) {
			case FR_ACTION_ADDING_FILES:
			case FR_ACTION_EXTRACTING_FILES:
			case FR_ACTION_DELETING_FILES:
			case FR_ACTION_UPDATING_FILES:
				if (remaining_files > 0)
					message = g_strdup_printf (ngettext ("%d file remaining",
									     "%'d files remaining",
									     remaining_files), remaining_files);
				else
					message = g_strdup (_("Please wait…"));
				break;
			default:
				break;
			}

			if (message != NULL) {
				fr_archive_message (archive, message);
				g_free (message);
			}
		}

		if (fraction == 1.0)
			gtk_widget_hide (window->priv->pd_progress_box);
		else
			gtk_widget_show (window->priv->pd_progress_box);

		window->priv->pd_last_fraction = fraction;

		g_signal_emit (G_OBJECT (window),
			       fr_window_signals[PROGRESS],
			       0,
			       window->priv->pd_last_fraction,
			       window->priv->pd_last_message);

#ifdef LOG_PROGRESS
		g_print ("progress > %2.2f\n", fraction);
#endif
	}
	return TRUE;
}