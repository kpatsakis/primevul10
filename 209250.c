int main(int argc, char *argv[]) {
  try {
    for (int i = 1; i < argc; i++) {
      string arg = argv[i];

      if (arg == "--extract" && i < argc - 1) {
        TarFileReader reader(argv[++i]);

        while (reader.hasMore())
          cout << reader.extract() << endl;

      } else THROWS("Invalid arg '" << arg << "'");
    }

    return 0;

  } catch (const Exception &e) {cerr << e.getMessage();}

  return 1;
}