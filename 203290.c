process_args(int argc, char **argv, int multi)
{
  int total_length, i, ret, quit;
  char *line;

  total_length = 0;
  for(i=0; i<argc; i++) {
    total_length += strlen(argv[i]) + 1;
  }

  line = (char *) malloc((2 + total_length) * sizeof(char));

  for (i = 0; i < argc; i++) {
    line[0] = '\0';
    if (multi) {
      strcat(line, argv[i]);
    } else {
      for (; i < argc; i++) {
        strcat(line, argv[i]);
        if (i + 1 < argc)
          strcat(line, " ");
      }
    }

    ret = process_line(line, &quit);
    if (!ret || quit)
      break;
  }

  free(line);

  return ret;
}