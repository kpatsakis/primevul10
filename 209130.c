void synchronize_logbook(LOGBOOK *lbs, int mode, BOOL sync_all) {
   int index, i, j, i_msg, i_remote, i_cache, n_remote, n_cache, nserver, remote_id, exist_remote,
           exist_cache, message_id, max_id, ssl;
   int all_identical, n_delete;
   char str[2000], url[256], loc_ref[256], rem_ref[256], pwd[256], locked_by[256], draft[256];
   MD5_INDEX *md5_remote, *md5_cache;
   char list[MAX_N_LIST][NAME_LENGTH], error_str[256], *buffer;
   unsigned char digest[16];

   if (!getcfg(lbs->name, "Mirror server", str, sizeof(str))) {
      show_error(loc("No mirror server defined in configuration file"));
      return;
   }

   nserver = strbreak(str, list, MAX_N_LIST, ",", FALSE);

   for (index = 0; index < nserver; index++) {

      if (mode == SYNC_HTML) {
         rsprintf("<table width=\"100%%\" cellpadding=\"1\" cellspacing=\"0\"");

         if (getcfg_topgroup())
            sprintf(loc_ref, "<a href=\"../%s/\">%s</a>", lbs->name_enc, lbs->name);
         else if (sync_all)
            sprintf(loc_ref, "<a href=\"%s/\">%s</a>", lbs->name_enc, lbs->name);
         else
            sprintf(loc_ref, "<a href=\".\">%s</a>", lbs->name);

         sprintf(str, loc("Synchronizing logbook %s with server \"%s\""), loc_ref, list[index]);
         rsprintf("<tr><td class=\"title1\">%s</td></tr>\n", str);
         rsprintf("</table><p>\n");

         rsprintf("<pre>");
      } else if (mode == SYNC_CLONE) {
         if (list[index][strlen(list[index]) - 1] != '/')
            eprintf("\nRetrieving entries from \"%s/%s\"...\n", list[index], lbs->name);
         else
            eprintf("\nRetrieving entries from \"%s%s\"...\n", list[index], lbs->name);
      }

      /* send partial return buffer */
      flush_return_buffer();

      do {

         n_remote = retrieve_remote_md5(lbs, list[index], &md5_remote, error_str);
         if (n_remote <= 0) {

            if ((n_remote == -2 || n_remote == -3) && mode == SYNC_CLONE) {

               if (n_remote == -3)
                  eprintf("\nInvalid username or password.");

               combine_url(lbs, list[index], "", url, sizeof(url), NULL);
               /* ask for username and password */
               eprintf("\nPlease enter username to access\n%s: ", url);
               fgets(str, sizeof(str), stdin);
               while (str[strlen(str) - 1] == '\r' || str[strlen(str) - 1] == '\n')
                  str[strlen(str) - 1] = 0;
               setparam("unm", str);

               eprintf("\nPlease enter password to access\n%s: ", url);
               read_password(str, sizeof(str));
               eprintf("\n");
               while (str[strlen(str) - 1] == '\r' || str[strlen(str) - 1] == '\n')
                  str[strlen(str) - 1] = 0;
               do_crypt(str, pwd, sizeof(pwd));
               setparam("upwd", pwd);

            } else {

               mprint(lbs, mode, error_str);

               if (md5_remote)
                  xfree(md5_remote);

               if (mode == SYNC_HTML)
                  rsprintf("</pre>\n");

               break;
            }
         }

      } while (n_remote <= 0);

      if (n_remote <= 0)
         continue;

      /* load local copy of remote MD5s from file */
      n_cache = load_md5(lbs, list[index], &md5_cache);

      all_identical = TRUE;

      /*---- check for configuration file ----*/

      if (getcfg(lbs->name, "Mirror config", str, sizeof(str)) && atoi(str) == 1 && md5_cache && mode
                                                                                                 !=
                                                                                                 SYNC_CLONE) {

         load_config_section(lbs->name, &buffer, error_str);
         if (error_str[0])
            mprint(lbs, mode, error_str);
         else {
            remove_crlf(buffer);
            MD5_checksum(buffer, strlen(buffer), digest);
         }

         /* compare MD5s */
         if (get_verbose() >= VERBOSE_INFO) {
            eprintf("CONFIG : ");
            for (j = 0; j < 16; j++)
               eprintf("%02X", digest[j]);
            eprintf("\nCache  : ");
            for (j = 0; j < 16; j++)
               eprintf("%02X", md5_cache[0].md5_digest[j]);
            eprintf("\nRemote : ");
            for (j = 0; j < 16; j++)
               eprintf("%02X", md5_remote[0].md5_digest[j]);
            eprintf("\n\n");
         }

         if (n_remote > 0) {
            /* if config has been changed on this server, but not remotely, send it */
            if (!equal_md5(md5_cache[0].md5_digest, digest) && equal_md5(md5_cache[0].md5_digest,
                                                                         md5_remote[0].md5_digest)) {

               all_identical = FALSE;

               if (_logging_level > 1)
                  write_logfile(lbs, "MIRROR send config");

               /* submit configuration section */
               if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                  submit_config(lbs, list[index], buffer, error_str);

                  if (error_str[0])
                     mprint(lbs, mode, error_str);
                  else
                     mprint(lbs, mode, "Local config submitted");
                  md5_cache[0].message_id = -1;
               } else
                  mprint(lbs, mode, "Local config should be submitted");

            } else
               /* if config has been changed remotely, but not on this server, receive it */
            if (!equal_md5(md5_cache[0].md5_digest, md5_remote[0].md5_digest)
                && equal_md5(md5_cache[0].md5_digest, digest)) {

               all_identical = FALSE;

               if (_logging_level > 1)
                  write_logfile(lbs, "MIRROR receive config");

               if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                  receive_config(lbs, list[index], error_str);

                  if (error_str[0])
                     mprint(lbs, mode, error_str);
                  else
                     mprint(lbs, mode, "Remote config received");

                  md5_cache[0].message_id = -1;
               } else
                  mprint(lbs, mode, loc("Remote config should be received"));

            } else
               /* if config has been changed remotely and on this server, show conflict */
            if (!equal_md5(md5_cache[0].md5_digest, md5_remote[0].md5_digest)
                && !equal_md5(md5_cache[0].md5_digest, digest)
                && !equal_md5(md5_remote[0].md5_digest, digest)) {

               if (_logging_level > 1)
                  write_logfile(lbs, "MIRROR config conflict");

               sprintf(str, "%s. ", loc("Configuration has been changed locally and remotely"));
               strcat(str, loc("Please merge manually to resolve conflict"));
               strcat(str, ".");

               mprint(lbs, mode, str);

            } else {
               /* configs are identical */
               md5_cache[0].message_id = -1;
            }
         } else {               /* n_remote == 0 */

            sprintf(str, loc("Logbook \"%s\" does not exist on remote server"), lbs->name);

            mprint(lbs, mode, str);
            continue;

         }

         flush_return_buffer();

         if (buffer)
            xfree(buffer);
      }

      /*---- loop through logbook entries ----*/

      n_delete = 0;

      for (i_msg = 0; i_msg < *lbs->n_el_index; i_msg++) {

         message_id = lbs->el_index[i_msg].message_id;

         /* check if message is locked */
         el_retrieve(lbs, message_id, NULL, NULL, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL, locked_by, draft);
         if (locked_by[0]) {
            sprintf(str, "ID%d:\t%s", message_id,
                    loc("Entry is locked on local server and therefore skipped"));
            mprint(lbs, mode, str);
            all_identical = FALSE;
            continue;
         }

         /* look for message id in MD5s */
         for (i_remote = 0; i_remote < n_remote; i_remote++)
            if (md5_remote[i_remote].message_id == message_id)
               break;
         exist_remote = i_remote < n_remote;

         for (i_cache = 0; i_cache < n_cache; i_cache++)
            if (md5_cache[i_cache].message_id == message_id)
               break;
         exist_cache = i_cache < n_cache;

         /* if message exists in both lists, compare MD5s */
         if (exist_remote && exist_cache) {

            /* compare MD5s */
            if (get_verbose() >= VERBOSE_INFO) {
               eprintf("ID%-5d: ", message_id);
               for (j = 0; j < 16; j++)
                  eprintf("%02X", lbs->el_index[i_msg].md5_digest[j]);
               eprintf("\nCache  : ");
               for (j = 0; j < 16; j++)
                  eprintf("%02X", md5_cache[i_cache].md5_digest[j]);
               eprintf("\nRemote : ");
               for (j = 0; j < 16; j++)
                  eprintf("%02X", md5_remote[i_remote].md5_digest[j]);
               eprintf("\n\n");
            }

            /* if message has been changed on this server, but not remotely, send it */
            if (!equal_md5(md5_cache[i_cache].md5_digest, lbs->el_index[i_msg].md5_digest)
                && equal_md5(md5_cache[i_cache].md5_digest, md5_remote[i_remote].md5_digest)) {

               all_identical = FALSE;

               if (_logging_level > 1) {
                  sprintf(str, "MIRROR send entry #%d", message_id);
                  write_logfile(lbs, str);
               }

               /* submit local message */
               if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                  submit_message(lbs, list[index], message_id, error_str);

                  /* not that submit_message() may have changed attr_list !!! */

                  if (error_str[0])
                     sprintf(str, "%s: %s", loc("Error sending local entry"), error_str);
                  else
                     sprintf(str, "ID%d:\t%s", message_id, loc("Local entry submitted"));
                  mprint(lbs, mode, str);

                  md5_cache[i_cache].message_id = -1;
               } else {
                  sprintf(str, "ID%d:\t%s", message_id, loc("Local entry should be submitted"));
                  mprint(lbs, mode, str);
               }

            } else
               /* if message has been changed remotely, but not on this server, receive it */
            if (!equal_md5(md5_cache[i_cache].md5_digest, md5_remote[i_remote].md5_digest)
                && equal_md5(md5_cache[i_cache].md5_digest, lbs->el_index[i_msg].md5_digest)) {

               all_identical = FALSE;

               if (mode == SYNC_CLONE) {
                  eprintf("ID%d:\t", message_id);
               } else if (mode == SYNC_HTML) {
                  if (getcfg_topgroup())
                     rsprintf("<a href=\"../%s/%d\">ID%d:</a>\t", lbs->name_enc, message_id, message_id);
                  else if (sync_all)
                     rsprintf("<a href=\"%s/%d\">ID%d:</a>\t", lbs->name_enc, message_id, message_id);
                  else
                     rsprintf("<a href=\"%d\">ID%d:</a>\t", message_id, message_id);

                  flush_return_buffer();
               }

               if (_logging_level > 1) {
                  sprintf(str, "MIRROR receive entry #%d", message_id);
                  write_logfile(lbs, str);
               }

               if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                  receive_message(lbs, list[index], message_id, error_str, FALSE);

                  if (error_str[0]) {
                     sprintf(str, "%s: %s", loc("Error receiving message"), error_str);
                     mprint(lbs, mode, str);
                  } else if (mode == SYNC_HTML) {

                     rsprintf("%s\n", loc("Remote entry received"));

                  } else if (mode == SYNC_CLONE) {
                     eprintf("%s\n", loc("Remote entry received"));
                  } else {
                     sprintf(str, "ID%d:\t%s", message_id, loc("Remote entry received"));
                     mprint(lbs, mode, str);
                  }

                  if (!error_str[0])
                     md5_cache[i_cache].message_id = -1;

               } else {
                  sprintf(str, "ID%d:\t%s", message_id, loc("Remote entry should be received"));
                  mprint(lbs, mode, str);
               }

            } else
               /* if message has been changed remotely and on this server, show conflict */
            if (!equal_md5(md5_cache[i_cache].md5_digest, md5_remote[i_remote].md5_digest)
                && !equal_md5(md5_cache[i_cache].md5_digest, lbs->el_index[i_msg].md5_digest)
                && !equal_md5(md5_remote[i_remote].md5_digest, lbs->el_index[i_msg].md5_digest)) {

               all_identical = FALSE;

               if (mode == SYNC_CLONE) {

                  eprintf("Warning: Entry #%d has been changed locally and remotely, will not be retrieved\n",
                          message_id);

               } else {

                  if (_logging_level > 1) {
                     sprintf(str, "MIRROR conflict entry #%d", message_id);
                     write_logfile(lbs, str);
                  }

                  combine_url(lbs, list[index], "", str, sizeof(str), NULL);

                  if (getcfg_topgroup())
                     sprintf(loc_ref, "<a href=\"../%s/%d\">%s</a>", lbs->name_enc, message_id, loc("local"));
                  else if (sync_all)
                     sprintf(loc_ref, "<a href=\"%s/%d\">%s</a>", lbs->name_enc, message_id, loc("local"));
                  else
                     sprintf(loc_ref, "<a href=\"%d\">%s</a>", message_id, loc("local"));

                  sprintf(rem_ref, "<a href=\"http://%s%d\">%s</a>", str, message_id, loc("remote"));

                  sprintf(str, "ID%d:\t%s. ", message_id, loc("Entry has been changed locally and remotely"));
                  sprintf(str + strlen(str), loc("Please delete %s or %s entry to resolve conflict"),
                          loc_ref, rem_ref);
                  strcat(str, ".");
                  mprint(lbs, mode, str);
               }

            } else {

               /* messages are identical */
               md5_cache[i_cache].message_id = -1;
            }
         }

         if (exist_cache && !exist_remote) {

            /* if message has been changed locally, send it */
            if (!equal_md5(md5_cache[i_cache].md5_digest, lbs->el_index[i_msg].md5_digest)) {

               /* compare MD5s */
               if (get_verbose() >= VERBOSE_INFO) {
                  eprintf("ID%-5d: ", message_id);
                  for (j = 0; j < 16; j++)
                     eprintf("%02X", lbs->el_index[i_msg].md5_digest[j]);
                  eprintf("\nCache  : ");
                  for (j = 0; j < 16; j++)
                     eprintf("%02X", md5_cache[i_cache].md5_digest[j]);
                  eprintf("\nRemote : none");
                  eprintf("\n\n");
               }

               all_identical = FALSE;

               if (_logging_level > 1) {
                  sprintf(str, "MIRROR send entry #%d", message_id);
                  write_logfile(lbs, str);
               }

               /* submit local message */
               if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                  submit_message(lbs, list[index], message_id, error_str);

                  /* not that submit_message() may have changed attr_list !!! */

                  if (error_str[0])
                     sprintf(str, "%s: %s", loc("Error sending local message"), error_str);
                  else
                     sprintf(str, "ID%d:\t%s", message_id, loc("Local entry submitted"));
                  mprint(lbs, mode, str);

                  md5_cache[i_cache].message_id = -1;

               } else {
                  sprintf(str, "ID%d:\t%s", message_id, loc("Local entry should be submitted"));
                  mprint(lbs, mode, str);
               }

            } else {

               /* if message exists only in cache, but not remotely,
                  it must have been deleted remotely, so remove it locally */

               if (!isparam("confirm") && mode == SYNC_HTML) {

                  combine_url(lbs, list[index], "", str, sizeof(str), NULL);

                  if (getcfg_topgroup())
                     sprintf(loc_ref, "<a href=\"../%s/%d\">%s</a>", lbs->name_enc, message_id, loc("local"));
                  else if (sync_all)
                     sprintf(loc_ref, "<a href=\"%s/%d\">%s</a>", lbs->name_enc, message_id,
                             loc("Local entry"));
                  else
                     sprintf(loc_ref, "<a href=\"%d\">%s</a>", message_id, loc("Local entry"));

                  sprintf(str, loc("%s should be deleted"), loc_ref);
                  rsprintf("ID%d:\t%s\n", message_id, str);
                  n_delete++;

               }
               if (!isparam("confirm") && mode == SYNC_CLONE) {

                  sprintf(str, "ID%d:\t%s", message_id, loc("Entry should be deleted locally"));
                  mprint(lbs, mode, str);

               } else {

                  all_identical = FALSE;

                  if (mode == SYNC_CLONE) {

                     el_delete_message(lbs, message_id, TRUE, NULL, TRUE, TRUE);

                     sprintf(str, "ID%d:\t%s", message_id, loc("Entry deleted locally"));
                     mprint(lbs, mode, str);

                     /* message got deleted from local message list, so redo current index */
                     i_msg--;

                  } else {

                     if (_logging_level > 1) {
                        sprintf(str, "MIRROR delete local entry #%d", message_id);
                        write_logfile(lbs, str);
                     }

                     if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                        el_delete_message(lbs, message_id, TRUE, NULL, TRUE, TRUE);

                        sprintf(str, "ID%d:\t%s", message_id, loc("Entry deleted locally"));
                        mprint(lbs, mode, str);

                        /* message got deleted from local message list, so redo current index */
                        i_msg--;
                     } else {
                        sprintf(str, "ID%d:\t%s", message_id, loc("Entry should be deleted locally"));
                        mprint(lbs, mode, str);
                     }
                  }

                  /* mark message non-conflicting */
                  md5_cache[i_cache].message_id = -1;
               }
            }
         }

         /* if message does not exist in cache and remotely,
            it must be new, so send it  */
         if (!exist_cache && !exist_remote) {

            all_identical = FALSE;

            if (_logging_level > 1) {
               sprintf(str, "MIRROR send entry #%d", message_id);
               write_logfile(lbs, str);
            }
            remote_id = 0;
            if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
               remote_id = submit_message(lbs, list[index], message_id, error_str);

               if (error_str[0])
                  sprintf(str, "%s: %s", loc("Error sending local entry"), error_str);
               else if (remote_id != message_id)
                  sprintf(str, "Error: Submitting entry #%d resulted in remote entry #%d\n", message_id,
                          remote_id);
               else
                  sprintf(str, "ID%d:\t%s", message_id, loc("Local entry submitted"));
               mprint(lbs, mode, str);
            } else {
               sprintf(str, "ID%d:\t%s", message_id, loc("Local entry should be submitted"));
               mprint(lbs, mode, str);
            }
         }

         /* if message does not exist in cache but remotely,
            messages were added on both sides, so resubmit local one and retrieve remote one
            if messages are different */
         if (!exist_cache && exist_remote && !equal_md5(md5_remote[i_remote].md5_digest,
                                                        lbs->el_index[i_msg].md5_digest)) {

            /* compare MD5s */
            if (get_verbose() >= VERBOSE_INFO) {
               eprintf("ID%-5d: ", message_id);
               for (j = 0; j < 16; j++)
                  eprintf("%02X", lbs->el_index[i_msg].md5_digest[j]);
               eprintf("\nCache  : none");
               eprintf("\nRemote : ");
               for (j = 0; j < 16; j++)
                  eprintf("%02X", md5_remote[i_remote].md5_digest[j]);
               eprintf("\n\n");
            }

            all_identical = FALSE;

            /* find max id both locally and remotely */
            max_id = 1;
            for (i = 0; i < *lbs->n_el_index; i++)
               if (lbs->el_index[i].message_id > max_id)
                  max_id = lbs->el_index[i].message_id;

            for (i = 0; i < n_remote; i++)
               if (md5_remote[i].message_id > max_id)
                  max_id = md5_remote[i].message_id;

            if (_logging_level > 1) {
               sprintf(str, "MIRROR change entry #%d to #%d", message_id, max_id + 1);
               write_logfile(lbs, str);
            }

            /* rearrange local message not to conflict with remote message */
            if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
               el_move_message(lbs, message_id, max_id + 1);

               sprintf(str, "ID%d:\t", message_id);
               sprintf(str + strlen(str), loc("Changed local entry ID to %d"), max_id + 1);
               mprint(lbs, mode, str);

               /* current message has been changed, so start over */
               i_msg--;
            } else {
               sprintf(str, "ID%d:\t", message_id);
               sprintf(str + strlen(str), loc("Local entry ID should be changed to %d"), max_id + 1);
               mprint(lbs, mode, str);
            }
         }

         flush_return_buffer();
      }

      /* go through remote message which do not exist locally */
      for (i_remote = 0; i_remote < n_remote; i_remote++)
         if (md5_remote[i_remote].message_id) {

            message_id = md5_remote[i_remote].message_id;

            for (i_msg = 0; i_msg < *lbs->n_el_index; i_msg++)
               if (message_id == lbs->el_index[i_msg].message_id)
                  break;

            if (i_msg == *lbs->n_el_index) {

               for (i_cache = 0; i_cache < n_cache; i_cache++)
                  if (md5_cache[i_cache].message_id == message_id)
                     break;
               exist_cache = i_cache < n_cache;

               if (!exist_cache) {

                  all_identical = FALSE;

                  if (mode == SYNC_HTML) {
                     if (getcfg_topgroup())
                        rsprintf("<a href=\"../%s/%d\">ID%d:</a>\t", lbs->name_enc, message_id, message_id);
                     else if (sync_all)
                        rsprintf("<a href=\"%s/%d\">ID%d:</a>\t", lbs->name_enc, message_id, message_id);
                     else
                        rsprintf("<a href=\"%d\">ID%d:</a>\t", message_id, message_id);

                     flush_return_buffer();
                  } else if (mode == SYNC_CLONE) {
                     eprintf("ID%d:\t", message_id);
                  }

                  /* if message does not exist locally and in cache, it is new, so retrieve it */
                  if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                     receive_message(lbs, list[index], message_id, error_str, TRUE);

                     if (error_str[0]) {
                        sprintf(str, "Error receiving message: %s", error_str);
                        mprint(lbs, mode, str);
                     } else if (mode == SYNC_HTML) {
                        rsprintf("%s\n", loc("Remote entry received"));
                     } else if (mode == SYNC_CLONE) {
                        eprintf("%s\n", loc("Remote entry received"));
                     } else {
                        sprintf(str, "ID%d:\t%s", message_id, loc("Remote entry received"));
                        mprint(lbs, mode, str);
                     }
                  } else {
                     sprintf(str, "ID%d:\t%s", message_id, loc("Remote entry should be received"));
                     mprint(lbs, mode, str);
                  }

               } else {

                  if (!equal_md5(md5_cache[i_cache].md5_digest, md5_remote[i_remote].md5_digest)) {

                     /* compare MD5s */
                     if (get_verbose() >= VERBOSE_INFO) {
                        eprintf("ID-%5: none", message_id);
                        eprintf("\nCache  : ");
                        for (j = 0; j < 16; j++)
                           eprintf("%02X", md5_cache[i_cache].md5_digest[j]);
                        eprintf("\nRemote : ");
                        for (j = 0; j < 16; j++)
                           eprintf("%02X", md5_remote[i_remote].md5_digest[j]);
                        eprintf("\n\n");
                     }

                     all_identical = FALSE;

                     /* if message has changed remotely, receive it */
                     if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                        receive_message(lbs, list[index], message_id, error_str, TRUE);

                        if (error_str[0]) {
                           sprintf(str, "Error receiving message: %s", error_str);
                           mprint(lbs, mode, str);
                        } else if (mode == SYNC_HTML) {

                           if (getcfg_topgroup())
                              rsprintf("<a href=\"../%s/%d\">ID%d:</a>\t", lbs->name_enc, message_id,
                                       message_id);
                           else if (sync_all)
                              rsprintf("<a href=\"%s/%d\">ID%d:</a>\t", lbs->name_enc, message_id,
                                       message_id);
                           else
                              rsprintf("<a href=\"%d\">ID%d:</a>\t", message_id, message_id);

                           rsprintf("%s\n", loc("Remote entry received"));
                        }
                     } else {
                        sprintf(str, "ID%d:\t%s", message_id, loc("Remote entry should be received"));
                        mprint(lbs, mode, str);
                     }

                  } else {

                     /* if message does not exist locally but in cache, delete remote message */

                     all_identical = FALSE;

                     if (!isparam("confirm") && mode == SYNC_HTML) {

                        combine_url(lbs, list[index], "", str, sizeof(str), NULL);
                        sprintf(rem_ref, "<a href=\"http://%s%d\">%s</a>", str, message_id,
                                loc("Remote entry"));

                        sprintf(str, loc("%s should be deleted"), rem_ref);
                        rsprintf("ID%d:\t%s\n", message_id, str);
                        n_delete++;

                     } else if (!isparam("confirm") && mode == SYNC_CLONE) {

                        sprintf(str, "ID%d:\t%s", message_id, loc("Entry should be deleted remotely"));
                        mprint(lbs, mode, str);

                     } else {

                        if (_logging_level > 1) {
                           sprintf(str, "MIRROR delete remote entry #%d", message_id);
                           write_logfile(lbs, str);
                        }

                        sprintf(str, "%d?cmd=%s&confirm=%s", message_id, loc("Delete"), loc("Yes"));
                        combine_url(lbs, list[index], str, url, sizeof(url), &ssl);

                        if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0) {
                           retrieve_url(lbs, url, ssl, &buffer, TRUE);

                           if (strstr(buffer, "Location: ")) {
                              if (mode == SYNC_HTML)
                                 rsprintf("ID%d:\t%s\n", message_id, loc("Entry deleted remotely"));
                           } else {

                              if (mode == SYNC_HTML && isparam("debug"))
                                 rsputs(buffer);

                              mprint(lbs, mode, loc("Error deleting remote entry"));
                           }

                           md5_cache[i_cache].message_id = -1;
                           xfree(buffer);
                        } else {
                           sprintf(str, "ID%d:\t%s", message_id, loc("Entry should be deleted remotely"));
                           mprint(lbs, mode, str);
                        }
                     }
                  }
               }
            }

            flush_return_buffer();
         }

      xfree(md5_remote);

      /* save remote MD5s in file */
      if (!all_identical) {
         n_remote = retrieve_remote_md5(lbs, list[index], &md5_remote, error_str);
         if (n_remote < 0)
            rsprintf("%s\n", error_str);

         /* keep conflicting messages in cache */
         for (i = 0; i < n_cache; i++)
            if (md5_cache[i].message_id != -1) {

               if (i == 0)
                  memcpy(md5_remote[0].md5_digest, md5_cache[0].md5_digest, 16);
               else
                  for (j = 0; j < n_remote; j++)
                     if (md5_remote[j].message_id == md5_cache[i].message_id) {
                        memcpy(md5_remote[j].md5_digest, md5_cache[i].md5_digest, 16);
                        break;
                     }
            }

         if (!getcfg(lbs->name, "Mirror simulate", str, sizeof(str)) || atoi(str) == 0)
            save_md5(lbs, list[index], md5_remote, n_remote);

         if (md5_remote)
            xfree(md5_remote);
      }

      if (md5_cache)
         xfree(md5_cache);

      if (mode == SYNC_HTML && n_delete) {

         if (getcfg_topgroup())
            rsprintf("<br><b><a href=\"../%s/?cmd=Synchronize&confirm=1\">", lbs->name_enc);
         else if (sync_all)
            rsprintf("<br><b><a href=\"%s/?cmd=Synchronize&confirm=1\">", lbs->name_enc);
         else
            rsprintf("<br><b><a href=\"../%s/?cmd=Synchronize&confirm=1\">", lbs->name_enc);

         if (n_delete > 1)
            rsprintf(loc("Click here to delete %d entries"), n_delete);
         else
            rsprintf(loc("Click here to delete this entry"));

         rsprintf("</a></b>\n");
      }

      if (mode == SYNC_HTML && all_identical)
         rsprintf(loc("All entries identical"));

      if (mode == SYNC_CLONE && all_identical)
         mprint(lbs, mode, loc("All entries identical"));

      if (mode == SYNC_HTML)
         rsprintf("</pre>\n");
   }

   flush_return_buffer();
   keep_alive = FALSE;
}