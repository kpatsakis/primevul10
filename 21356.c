static void do_key_event(VncState *vs, int down, int keycode, int sym)
{
    QKeyCode qcode = qemu_input_key_number_to_qcode(keycode);

    /* QEMU console switch */
    switch (qcode) {
    case Q_KEY_CODE_1 ... Q_KEY_CODE_9: /* '1' to '9' keys */
        if (vs->vd->dcl.con == NULL && down &&
            qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_CTRL) &&
            qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_ALT)) {
            /* Reset the modifiers sent to the current console */
            qkbd_state_lift_all_keys(vs->vd->kbd);
            console_select(qcode - Q_KEY_CODE_1);
            return;
        }
    default:
        break;
    }

    /* Turn off the lock state sync logic if the client support the led
       state extension.
    */
    if (down && vs->vd->lock_key_sync &&
        !vnc_has_feature(vs, VNC_FEATURE_LED_STATE) &&
        keycode_is_keypad(vs->vd->kbd_layout, keycode)) {
        /* If the numlock state needs to change then simulate an additional
           keypress before sending this one.  This will happen if the user
           toggles numlock away from the VNC window.
        */
        if (keysym_is_numlock(vs->vd->kbd_layout, sym & 0xFFFF)) {
            if (!qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_NUMLOCK)) {
                trace_vnc_key_sync_numlock(true);
                press_key(vs, Q_KEY_CODE_NUM_LOCK);
            }
        } else {
            if (qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_NUMLOCK)) {
                trace_vnc_key_sync_numlock(false);
                press_key(vs, Q_KEY_CODE_NUM_LOCK);
            }
        }
    }

    if (down && vs->vd->lock_key_sync &&
        !vnc_has_feature(vs, VNC_FEATURE_LED_STATE) &&
        ((sym >= 'A' && sym <= 'Z') || (sym >= 'a' && sym <= 'z'))) {
        /* If the capslock state needs to change then simulate an additional
           keypress before sending this one.  This will happen if the user
           toggles capslock away from the VNC window.
        */
        int uppercase = !!(sym >= 'A' && sym <= 'Z');
        bool shift = qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_SHIFT);
        bool capslock = qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_CAPSLOCK);
        if (capslock) {
            if (uppercase == shift) {
                trace_vnc_key_sync_capslock(false);
                press_key(vs, Q_KEY_CODE_CAPS_LOCK);
            }
        } else {
            if (uppercase != shift) {
                trace_vnc_key_sync_capslock(true);
                press_key(vs, Q_KEY_CODE_CAPS_LOCK);
            }
        }
    }

    qkbd_state_key_event(vs->vd->kbd, qcode, down);
    if (!qemu_console_is_graphic(NULL)) {
        bool numlock = qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_NUMLOCK);
        bool control = qkbd_state_modifier_get(vs->vd->kbd, QKBD_MOD_CTRL);
        /* QEMU console emulation */
        if (down) {
            switch (keycode) {
            case 0x2a:                          /* Left Shift */
            case 0x36:                          /* Right Shift */
            case 0x1d:                          /* Left CTRL */
            case 0x9d:                          /* Right CTRL */
            case 0x38:                          /* Left ALT */
            case 0xb8:                          /* Right ALT */
                break;
            case 0xc8:
                kbd_put_keysym(QEMU_KEY_UP);
                break;
            case 0xd0:
                kbd_put_keysym(QEMU_KEY_DOWN);
                break;
            case 0xcb:
                kbd_put_keysym(QEMU_KEY_LEFT);
                break;
            case 0xcd:
                kbd_put_keysym(QEMU_KEY_RIGHT);
                break;
            case 0xd3:
                kbd_put_keysym(QEMU_KEY_DELETE);
                break;
            case 0xc7:
                kbd_put_keysym(QEMU_KEY_HOME);
                break;
            case 0xcf:
                kbd_put_keysym(QEMU_KEY_END);
                break;
            case 0xc9:
                kbd_put_keysym(QEMU_KEY_PAGEUP);
                break;
            case 0xd1:
                kbd_put_keysym(QEMU_KEY_PAGEDOWN);
                break;

            case 0x47:
                kbd_put_keysym(numlock ? '7' : QEMU_KEY_HOME);
                break;
            case 0x48:
                kbd_put_keysym(numlock ? '8' : QEMU_KEY_UP);
                break;
            case 0x49:
                kbd_put_keysym(numlock ? '9' : QEMU_KEY_PAGEUP);
                break;
            case 0x4b:
                kbd_put_keysym(numlock ? '4' : QEMU_KEY_LEFT);
                break;
            case 0x4c:
                kbd_put_keysym('5');
                break;
            case 0x4d:
                kbd_put_keysym(numlock ? '6' : QEMU_KEY_RIGHT);
                break;
            case 0x4f:
                kbd_put_keysym(numlock ? '1' : QEMU_KEY_END);
                break;
            case 0x50:
                kbd_put_keysym(numlock ? '2' : QEMU_KEY_DOWN);
                break;
            case 0x51:
                kbd_put_keysym(numlock ? '3' : QEMU_KEY_PAGEDOWN);
                break;
            case 0x52:
                kbd_put_keysym('0');
                break;
            case 0x53:
                kbd_put_keysym(numlock ? '.' : QEMU_KEY_DELETE);
                break;

            case 0xb5:
                kbd_put_keysym('/');
                break;
            case 0x37:
                kbd_put_keysym('*');
                break;
            case 0x4a:
                kbd_put_keysym('-');
                break;
            case 0x4e:
                kbd_put_keysym('+');
                break;
            case 0x9c:
                kbd_put_keysym('\n');
                break;

            default:
                if (control) {
                    kbd_put_keysym(sym & 0x1f);
                } else {
                    kbd_put_keysym(sym);
                }
                break;
            }
        }
    }
}