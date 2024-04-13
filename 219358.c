string_format_size(int size, uschar *buffer)
{
if (size == 0) Ustrcpy(CS buffer, "     ");
else if (size < 1024) sprintf(CS buffer, "%5d", size);
else if (size < 10*1024)
  sprintf(CS buffer, "%4.1fK", (double)size / 1024.0);
else if (size < 1024*1024)
  sprintf(CS buffer, "%4dK", (size + 512)/1024);
else if (size < 10*1024*1024)
  sprintf(CS buffer, "%4.1fM", (double)size / (1024.0 * 1024.0));
else
  sprintf(CS buffer, "%4dM", (size + 512 * 1024)/(1024*1024));
return buffer;
}