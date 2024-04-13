    Options(const ImportGraphDefOptions& in)  // NOLINT(runtime/explicit)
        : allow_internal_ops(false),
          expect_device_spec(false),
          prefix(in.prefix.empty() || str_util::EndsWith(in.prefix, "/")
                     ? in.prefix
                     : in.prefix + "/"),
          uniquify_names(in.uniquify_names),
          uniquify_prefix(in.uniquify_prefix),
          input_map(in.input_map.begin(), in.input_map.end()),
          skip_mapped_nodes(in.skip_mapped_nodes),
          control_dependencies(in.control_dependencies),
          return_tensors(in.return_tensors.begin(), in.return_tensors.end()),
          return_nodes(in.return_nodes),
          importing(true),
          validate_nodes(true),
          validate_colocation_constraints(in.validate_colocation_constraints),
          validate_shape(in.validate_shape),
          default_device(in.default_device) {}