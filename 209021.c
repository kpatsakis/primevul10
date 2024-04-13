void call_image_magick(LOGBOOK *lbs) {
   char str[1024], cmd[1024], file_name[256], thumb_name[256], subdir[256];
   int cur_width, cur_height, new_size, cur_rot, new_rot, thumb_status;

   if (!isparam("req") || !isparam("img")) {
      show_error("Unknown IM request received");
      return;
   }

   strlcpy(file_name, lbs->data_dir, sizeof(file_name));
   generate_subdir_name(getparam("img"), subdir, sizeof(subdir));
   strlcat(file_name, subdir, sizeof(file_name));
   strlcat(file_name, getparam("img"), sizeof(file_name));
   thumb_status = get_thumb_name(file_name, thumb_name, sizeof(thumb_name), 0);

   sprintf(cmd, "%s -format '%%wx%%h %%c' '%s'", _identify_cmd, thumb_name);
#ifdef OS_WINNT
                                                                                                                           {
   int i;
   for (i = 0; i < (int) strlen(cmd); i++)
      if (cmd[i] == '\'')
         cmd[i] = '\"';
   }
#endif
   my_shell(cmd, str, sizeof(str));

   if (atoi(str) > 0) {
      cur_width = atoi(str);
      if (strchr(str, 'x')) {
         cur_height = atoi(strchr(str, 'x') + 1);
      } else
         cur_height = cur_width;
      if (strchr(str, ' ')) {
         cur_rot = atoi(strchr(str, ' ') + 1);
      } else
         cur_rot = 0;
   } else {
      show_http_header(NULL, FALSE, NULL);
      rsputs(str);
      return;
   }

   if (thumb_status == 2)
      strsubst(thumb_name, sizeof(thumb_name), "-0", "-%d");

   cmd[0] = 0;
   if (strieq(getparam("req"), "rotleft")) {
      new_rot = (cur_rot + 360 - 90) % 360;
      sprintf(cmd, "%s '%s' -rotate %d -thumbnail %d -set comment ' %d' '%s'", _convert_cmd, file_name,
              new_rot,
              cur_height, new_rot, thumb_name);
   }

   if (strieq(getparam("req"), "rotright")) {
      new_rot = (cur_rot + 90) % 360;
      sprintf(cmd, "%s '%s' -rotate %d -thumbnail %d -set comment ' %d' '%s'", _convert_cmd, file_name,
              new_rot,
              cur_height, new_rot, thumb_name);
   }

   if (strieq(getparam("req"), "original")) {
      new_size = (int) (cur_width / 1.5);
      sprintf(cmd, "%s '%s' '%s'", _convert_cmd, file_name, thumb_name);
   }

   if (strieq(getparam("req"), "smaller")) {
      new_size = (int) (cur_width / 1.5);
      sprintf(cmd, "%s '%s' -rotate %d -thumbnail %d -set comment ' %d' '%s'", _convert_cmd, file_name,
              cur_rot,
              new_size, cur_rot, thumb_name);
   }

   if (strieq(getparam("req"), "larger")) {
      new_size = (int) (cur_width * 1.5);
      sprintf(cmd, "%s '%s' -rotate %d -thumbnail %d -set comment ' %d' '%s'", _convert_cmd, file_name,
              cur_rot,
              new_size, cur_rot, thumb_name);
   }

   if (cmd[0]) {
#ifdef OS_WINNT
                                                                                                                              int i;
      for (i = 0; i < (int) strlen(cmd); i++)
         if (cmd[i] == '\'')
            cmd[i] = '\"';
#endif
      my_shell(cmd, str, sizeof(str));
      show_http_header(NULL, TRUE, NULL);
      rsputs(str);
   }
   return;
}