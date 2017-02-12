device_listener_test (const AtspiDeviceEvent *stroke, void* user_data)
{

        printf ("\n%s: %i\n", stroke->event_string, stroke->hw_code);
        gchar* toSay = NULL;
	AtspiText* text = NULL;
        AtspiAccessible** focus = (AtspiAccessible**)user_data;
	
	if (text = atspi_accessible_get_text_iface(*focus)) {
		atspi_deregister_keystroke_listener (test_listener, NULL, 0, ATSPI_KEY_RELEASED_EVENT,  ATSPI_KEYLISTENER_CANCONSUME | ATSPI_KEYLISTENER_SYNCHRONOUS, error);
		atspi_register_keystroke_listener (test_listener, NULL, 0, ATSPI_KEY_RELEASED_EVENT,  ATSPI_KEYLISTENER_CANCONSUME | ATSPI_KEYLISTENER_SYNCHRONOUS, error);
	}

        switch (stroke->hw_code){
        case 9:
                atspi_event_quit();
                return TRUE;
        case 116:
                if (next = SO_interact(*focus)) *focus = next;
                break;
        case 111:
                if (next = SO_uninteract(*focus)) *focus = next;
                break;
        case 113:
                if (next = SO_move(*focus, -1)) *focus = next;
                break;
        case 114:
                if (next = SO_move(*focus, 1)) *focus = next;
                break;
        case 65:
                SO_perform_action(*focus);
                break;
        case 40:
                toSay = atspi_accessible_get_description(*focus, error);
                break;
	}
ø
