add_rotation_items_for_output (GsdXrandrManager *manager, GnomeOutputInfo *output)
{
        int num_rotations;
        GnomeRRRotation rotations;

        get_allowed_rotations_for_output (manager, output, &num_rotations, &rotations);

        if (num_rotations == 1)
                add_unsupported_rotation_item (manager);
        else
                add_items_for_rotations (manager, output, rotations);
}