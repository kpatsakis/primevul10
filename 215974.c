    Options(const GraphConstructorOptions& in)  // NOLINT(runtime/explicit)
        : allow_internal_ops(in.allow_internal_ops),
          expect_device_spec(in.expect_device_spec),
          importing(false),
          validate_nodes(in.validate_nodes),
          validate_colocation_constraints(false),
          add_default_attributes(in.add_default_attributes) {}