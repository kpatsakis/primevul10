std::string find_dtd(LibofxContextPtr ctx, const std::string& dtd_filename)
{
  string dtd_path_filename;
  char *env_dtd_path;

  dtd_path_filename = reinterpret_cast<const LibofxContext*>(ctx)->dtdDir();
  if (!dtd_path_filename.empty())
  {
    dtd_path_filename.append(dtd_filename);
    ifstream dtd_file(dtd_path_filename.c_str());
    if (dtd_file)
    {
      message_out(STATUS, "find_dtd():DTD found: " + dtd_path_filename);
      return dtd_path_filename;
    }
  }

#ifdef OS_WIN32
  dtd_path_filename = get_dtd_installation_directory();
  if (!dtd_path_filename.empty())
  {
    dtd_path_filename.append(DIRSEP);
    dtd_path_filename.append(dtd_filename);
    ifstream dtd_file(dtd_path_filename.c_str());
    if (dtd_file)
    {
      message_out(STATUS, "find_dtd():DTD found: " + dtd_path_filename);
      return dtd_path_filename;
    }
  }
#endif
  /* Search in environement variable OFX_DTD_PATH */
  env_dtd_path = getenv("OFX_DTD_PATH");
  if (env_dtd_path)
  {
    dtd_path_filename.append(env_dtd_path);
    dtd_path_filename.append(DIRSEP);
    dtd_path_filename.append(dtd_filename);
    ifstream dtd_file(dtd_path_filename.c_str());
    if (!dtd_file)
    {
      message_out(STATUS, "find_dtd():OFX_DTD_PATH env variable was was present, but unable to open the file " + dtd_path_filename);
    }
    else
    {
      message_out(STATUS, "find_dtd():DTD found: " + dtd_path_filename);
      return dtd_path_filename;
    }
  }

  for (int i = 0; i < DTD_SEARCH_PATH_NUM; i++)
  {
    dtd_path_filename = DTD_SEARCH_PATH[i];
    dtd_path_filename.append(DIRSEP);
    dtd_path_filename.append(dtd_filename);
    ifstream dtd_file(dtd_path_filename.c_str());
    if (!dtd_file)
    {
      message_out(DEBUG, "find_dtd():Unable to open the file " + dtd_path_filename);
    }
    else
    {
      message_out(STATUS, "find_dtd():DTD found: " + dtd_path_filename);
      return dtd_path_filename;
    }
  }

  /* Last resort, look in source tree relative path (useful for development) */
  dtd_path_filename = "";
  dtd_path_filename.append("..");
  dtd_path_filename.append(DIRSEP);
  dtd_path_filename.append("dtd");
  dtd_path_filename.append(DIRSEP);
  dtd_path_filename.append(dtd_filename);
  ifstream dtd_file(dtd_path_filename.c_str());
  if (!dtd_file)
  {
    message_out(DEBUG, "find_dtd(): Unable to open the file " + dtd_path_filename + ", most likely we are not in the source tree.");
  }
  else
  {
    message_out(STATUS, "find_dtd():DTD found: " + dtd_path_filename);
    return dtd_path_filename;
  }


  message_out(ERROR, "find_dtd():Unable to find the DTD named " + dtd_filename);
  return "";
}