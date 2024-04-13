std::vector<std::vector<float>> DefaultColorTable(int depth) {
  std::vector<std::vector<float>> color_table;
  color_table.emplace_back(std::vector<float>({1, 1, 0, 1}));      // 0: yellow
  color_table.emplace_back(std::vector<float>({0, 0, 1, 1}));      // 1: blue
  color_table.emplace_back(std::vector<float>({1, 0, 0, 1}));      // 2: red
  color_table.emplace_back(std::vector<float>({0, 1, 0, 1}));      // 3: lime
  color_table.emplace_back(std::vector<float>({0.5, 0, 0.5, 1}));  // 4: purple
  color_table.emplace_back(std::vector<float>({0.5, 0.5, 0, 1}));  // 5: olive
  color_table.emplace_back(std::vector<float>({0.5, 0, 0, 1}));    // 6: maroon
  color_table.emplace_back(std::vector<float>({0, 0, 0.5, 1}));  // 7: navy blue
  color_table.emplace_back(std::vector<float>({0, 1, 1, 1}));    // 8: aqua
  color_table.emplace_back(std::vector<float>({1, 0, 1, 1}));    // 9: fuchsia

  if (depth == 1) {
    for (int64 i = 0; i < color_table.size(); i++) {
      color_table[i][0] = 1;
    }
  }
  return color_table;
}