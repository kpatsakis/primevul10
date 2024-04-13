load_cache (GeglProperties *op_magick_load)
{
  if (!op_magick_load->user_data)
    {
      gchar    *filename;
      GeglNode *graph, *sink, *loader;
      GeglBuffer *newbuf = NULL;

      /* ImageMagick backed fallback FIXME: make this robust.
       * maybe use pipes in a manner similar to the raw loader,
       * or at least use a properly unique filename  */
      char     *argv[4]  = {"convert", NULL, NULL, NULL};

      filename = g_build_filename (g_get_tmp_dir (), "gegl-magick.png", NULL);

      argv[1] = g_strdup_printf ("%s[0]", op_magick_load->path);
      argv[2] = filename;
      if (!g_spawn_sync (NULL, argv, NULL, G_SPAWN_DEFAULT, 
                         NULL, NULL, NULL, NULL, NULL, NULL))
        g_warning ("Error executing ImageMagick convert program");

      g_free (argv[1]);

      graph = gegl_node_new ();
      sink = gegl_node_new_child (graph,
                                 "operation", "gegl:buffer-sink",
                                 "buffer", &newbuf, NULL);
      loader = gegl_node_new_child (graph,
                                    "operation", "gegl:png-load",
                                    "path", filename, NULL);
      gegl_node_link_many (loader, sink, NULL);
      gegl_node_process (sink);
      op_magick_load->user_data = (gpointer) newbuf;
      g_object_unref (graph);
      g_free (filename);
    }
}