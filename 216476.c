void WebContents::Print(gin_helper::Arguments* args) {
  gin_helper::Dictionary options =
      gin::Dictionary::CreateEmpty(args->isolate());
  base::Value settings(base::Value::Type::DICTIONARY);

  if (args->Length() >= 1 && !args->GetNext(&options)) {
    args->ThrowError("webContents.print(): Invalid print settings specified.");
    return;
  }

  printing::CompletionCallback callback;
  if (args->Length() == 2 && !args->GetNext(&callback)) {
    args->ThrowError(
        "webContents.print(): Invalid optional callback provided.");
    return;
  }

  // Set optional silent printing
  bool silent = false;
  options.Get("silent", &silent);

  bool print_background = false;
  options.Get("printBackground", &print_background);
  settings.SetBoolKey(printing::kSettingShouldPrintBackgrounds,
                      print_background);

  // Set custom margin settings
  gin_helper::Dictionary margins =
      gin::Dictionary::CreateEmpty(args->isolate());
  if (options.Get("margins", &margins)) {
    printing::MarginType margin_type = printing::DEFAULT_MARGINS;
    margins.Get("marginType", &margin_type);
    settings.SetIntKey(printing::kSettingMarginsType, margin_type);

    if (margin_type == printing::CUSTOM_MARGINS) {
      base::Value custom_margins(base::Value::Type::DICTIONARY);
      int top = 0;
      margins.Get("top", &top);
      custom_margins.SetIntKey(printing::kSettingMarginTop, top);
      int bottom = 0;
      margins.Get("bottom", &bottom);
      custom_margins.SetIntKey(printing::kSettingMarginBottom, bottom);
      int left = 0;
      margins.Get("left", &left);
      custom_margins.SetIntKey(printing::kSettingMarginLeft, left);
      int right = 0;
      margins.Get("right", &right);
      custom_margins.SetIntKey(printing::kSettingMarginRight, right);
      settings.SetPath(printing::kSettingMarginsCustom,
                       std::move(custom_margins));
    }
  } else {
    settings.SetIntKey(printing::kSettingMarginsType,
                       printing::DEFAULT_MARGINS);
  }

  // Set whether to print color or greyscale
  bool print_color = true;
  options.Get("color", &print_color);
  int color_setting = print_color ? printing::COLOR : printing::GRAY;
  settings.SetIntKey(printing::kSettingColor, color_setting);

  // Is the orientation landscape or portrait.
  bool landscape = false;
  options.Get("landscape", &landscape);
  settings.SetBoolKey(printing::kSettingLandscape, landscape);

  // We set the default to the system's default printer and only update
  // if at the Chromium level if the user overrides.
  // Printer device name as opened by the OS.
  base::string16 device_name;
  options.Get("deviceName", &device_name);
  if (!device_name.empty() && !IsDeviceNameValid(device_name)) {
    args->ThrowError("webContents.print(): Invalid deviceName provided.");
    return;
  }

  int scale_factor = 100;
  options.Get("scaleFactor", &scale_factor);
  settings.SetIntKey(printing::kSettingScaleFactor, scale_factor);

  int pages_per_sheet = 1;
  options.Get("pagesPerSheet", &pages_per_sheet);
  settings.SetIntKey(printing::kSettingPagesPerSheet, pages_per_sheet);

  // True if the user wants to print with collate.
  bool collate = true;
  options.Get("collate", &collate);
  settings.SetBoolKey(printing::kSettingCollate, collate);

  // The number of individual copies to print
  int copies = 1;
  options.Get("copies", &copies);
  settings.SetIntKey(printing::kSettingCopies, copies);

  // Strings to be printed as headers and footers if requested by the user.
  std::string header;
  options.Get("header", &header);
  std::string footer;
  options.Get("footer", &footer);

  if (!(header.empty() && footer.empty())) {
    settings.SetBoolKey(printing::kSettingHeaderFooterEnabled, true);

    settings.SetStringKey(printing::kSettingHeaderFooterTitle, header);
    settings.SetStringKey(printing::kSettingHeaderFooterURL, footer);
  } else {
    settings.SetBoolKey(printing::kSettingHeaderFooterEnabled, false);
  }

  // We don't want to allow the user to enable these settings
  // but we need to set them or a CHECK is hit.
  settings.SetIntKey(printing::kSettingPrinterType,
                     static_cast<int>(printing::PrinterType::kLocal));
  settings.SetBoolKey(printing::kSettingShouldPrintSelectionOnly, false);
  settings.SetBoolKey(printing::kSettingRasterizePdf, false);

  // Set custom page ranges to print
  std::vector<gin_helper::Dictionary> page_ranges;
  if (options.Get("pageRanges", &page_ranges)) {
    base::Value page_range_list(base::Value::Type::LIST);
    for (auto& range : page_ranges) {
      int from, to;
      if (range.Get("from", &from) && range.Get("to", &to)) {
        base::Value range(base::Value::Type::DICTIONARY);
        range.SetIntKey(printing::kSettingPageRangeFrom, from);
        range.SetIntKey(printing::kSettingPageRangeTo, to);
        page_range_list.Append(std::move(range));
      } else {
        continue;
      }
    }
    if (page_range_list.GetList().size() > 0)
      settings.SetPath(printing::kSettingPageRange, std::move(page_range_list));
  }

  // Duplex type user wants to use.
  printing::mojom::DuplexMode duplex_mode =
      printing::mojom::DuplexMode::kSimplex;
  options.Get("duplexMode", &duplex_mode);
  settings.SetIntKey(printing::kSettingDuplexMode,
                     static_cast<int>(duplex_mode));

  // We've already done necessary parameter sanitization at the
  // JS level, so we can simply pass this through.
  base::Value media_size(base::Value::Type::DICTIONARY);
  if (options.Get("mediaSize", &media_size))
    settings.SetKey(printing::kSettingMediaSize, std::move(media_size));

  // Set custom dots per inch (dpi)
  gin_helper::Dictionary dpi_settings;
  int dpi = 72;
  if (options.Get("dpi", &dpi_settings)) {
    int horizontal = 72;
    dpi_settings.Get("horizontal", &horizontal);
    settings.SetIntKey(printing::kSettingDpiHorizontal, horizontal);
    int vertical = 72;
    dpi_settings.Get("vertical", &vertical);
    settings.SetIntKey(printing::kSettingDpiVertical, vertical);
  } else {
    settings.SetIntKey(printing::kSettingDpiHorizontal, dpi);
    settings.SetIntKey(printing::kSettingDpiVertical, dpi);
  }

  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_BLOCKING},
      base::BindOnce(&GetDefaultPrinterAsync),
      base::BindOnce(&WebContents::OnGetDefaultPrinter,
                     weak_factory_.GetWeakPtr(), std::move(settings),
                     std::move(callback), device_name, silent));
}