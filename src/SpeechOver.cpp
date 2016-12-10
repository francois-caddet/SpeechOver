/*
 * AT-SPI 2 Examples. Simple AT.
 *
 * Copyright (c) 2011 Alejandro Leiva <aleiva@emergya.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Author: Alejandro Leiva <aleiva@emergya.com>
 *
 * Ported from CSPI examples.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <libspeechd.h>
#include <atspi/atspi.h>
#include <wx/defs.h>

// Device listeners prototypes
AtspiAccessible* SOFocus;
GError **error = NULL;
SPDConnection* spdCo;
glong index = 0;

gboolean
SO_interact(){
	glong nbchild = atspi_accessible_get_child_count(SOFocus, error)
	for (index = 0; index < nbchild 
		&& !(AtspiAccessible* child = atspi_accessible_get_child_at_index(SOFocus, index, error))
		&& atspi_accessible_get_role(child) == ATSPI_ROLE_INVALID)
	{
		SOFocus = child;
	}
	return FALSE;
}

gboolean
SO_uninteract(){
        SOFocus = atspi_accessible_get_parent(SOFocus, error);
	index = 0;
	return TRUE;
}

gboolean
SO_move(glong to){
	if (AtspiAccessible* parent = atspi_accessible_get_parent(SOFocus, error)){
		index = (index + to) % atspi_accessible_get_child_count(parent, error);
		if (index < 0) index += atspi_accessible_get_child_count(parent, error);
		SOFocus = atspi_accessible_get_child_at_index(parent,
				index,
				error);
		return TRUE;
	}
	return FALSE;
}

gboolean
device_listener_test (const AtspiDeviceEvent *stroke, void *user_data);

void
device_listener_test_destroy (void *data);

// Event listeners prototypes
void
event_listener_generic (const AtspiEvent *event);

void
event_listener_generic_destroy (void *data);
/*
void
print_childs_tree(AtspiAccessible *node,AtspiMatchRule *rule, GError **error)
{
	gint n_childs = atspi_accessible_get_child_count(node, error);
	GArray *childs = atspi_collection_get_matches(node, rule, ATSPI_Collection_SORT_ORDER_CANONICAL, n_childs, TRUE, error);
	gchar *child_name;
	AtspiAccessible *child;

	for (gint i = 0; i < childs->len; i++)
	{
		child = &g_array_index(childs, AtspiAccessible, i);
		child_name = atspi_accessible_get_name (child, error);
		if (child_name[0] != '\0') printf ("[%i] %s\n", i, child_name);
		print_childs_tree(child, rule, error);
	}

}
*/
int
main (int argc, char **argv)
{

	gint atspi_status = 1;
	spdCo = spd_open("SpeechOver", "spd", "fcaddet", SPD_MODE_THREADED);
	gint i;
	AtspiAccessible* desktop = SOFocus;
	gint n_desktops;
	gchar *desktop_name;
	AtspiStateType active = ATSPI_STATE_ACTIVE;
	GArray *states_array = g_array_new (FALSE, FALSE, sizeof(AtspiStateType));
	g_array_append_val(states_array, active);
/*	AtspiMatchRule *rule = atspi_match_rule_new(
		atspi_state_set_new(states_array), ATSPI_Collection_MATCH_ANY,
		NULL, 0,
		NULL, 0,
		NULL, 0,
		TRUE);*/
	int *user_data = NULL;
	AtspiDeviceListener *test_listener;

    //AtspiEventListener *generic_listener;

	atspi_status = atspi_init ();
	
	printf ("Starting [atspi status = %i]\n", atspi_status);

	// Create device listeners
	test_listener = atspi_device_listener_new (&device_listener_test, user_data, &device_listener_test_destroy);

	// Register device listeners
	atspi_register_keystroke_listener (test_listener, NULL, 0, ATSPI_KEY_RELEASED_EVENT,  ATSPI_KEYLISTENER_CANCONSUME | ATSPI_KEYLISTENER_SYNCHRONOUS, error);

	// Create event listeners
    //generic_listener = atspi_event_listener_new_simple (event_listener_generic, event_listener_generic_destroy);
    //atspi_event_listener_register (generic_listener, "object:", error);

	n_desktops = atspi_get_desktop_count ();
	//GArray *a_desktops = atspi_get_desktop_list ();

	for (i = 0; i < n_desktops; i++)
	{

		desktop = atspi_get_desktop (i);		
		desktop_name = atspi_accessible_get_name (desktop, error);
		if (spdCo != NULL)
			spd_sayf (spdCo, SPD_TEXT, "Desktop [%i]: %s\n", i, desktop_name);
		else printf("no spd");
		printf ("Desktop [%i]: %s\n", i, desktop_name);
		//print_childs_tree(desktop, rule, error);
	}
	SOFocus = desktop;

	atspi_event_main();

	atspi_status = atspi_exit ();
	spd_close(spdCo);
	printf ("Stopping atspi2 [status = %i]\n", atspi_status);

	return atspi_status; 
}

gboolean
device_listener_test (const AtspiDeviceEvent *stroke, void *user_data)
{

	printf ("%s: %i\n", stroke->event_string, stroke->hw_code);
	gchar* toSay = NULL;
	switch (stroke->hw_code){
	case 9: 
		atspi_event_quit();
		return TRUE;
	case 116:
		SO_interact();
		break;
	case 111:
		SO_uninteract();
		break;
	case 113:
		SO_move(-1);
		break;
	case 114:
		SO_move(1);
		break;
	case 40:
		toSay = atspi_accessible_get_description(SOFocus, error);
		break;
	}
	if (toSay)
		spd_sayf (spdCo, SPD_TEXT, "description: %s\n", toSay);
	else
		spd_sayf (spdCo, SPD_TEXT, "%s, %s\n", atspi_accessible_get_name(SOFocus, error), atspi_accessible_get_role_name(SOFocus, error));
	/*GArray* relations = atspi_accessible_get_relation_set(SOFocus, error);
	for (gint i = 0; i < relations->len; ++i){
		AtspiRelation* r = g_array_indew(atr, AtspiRelation*, i);
	}*/
	
	if (GArray* ifaces = atspi_accessible_get_interfaces(SOFocus)) {
		printf ("\nINTERFACES\n");
		for (gint i = 0; i < ifaces->len; ++i)
			printf("\t%s\n", g_array_index(ifaces, gchar*, i));
	}

	if (AtspiAction* act = atspi_accessible_get_action(SOFocus)) {
		printf ("\nACTIONS\n");
		for (gint i = 0; i < atspi_action_get_n_actions(act, error); ++i)
			printf ("\t%s (%s): %s\n",
				atspi_action_get_action_name(act, i, error),
				atspi_action_get_key_binding(act, i, error),
				atspi_action_get_action_description(act, i, error)
			);
	}
	
	if (atspi_state_set_contains(
			atspi_accessible_get_state_set(SOFocus),
			ATSPI_STATE_ACTIVE
		)) printf ("\nACTIVE\n");
	else if (atspi_state_set_contains(
                        atspi_accessible_get_state_set(SOFocus),
                        ATSPI_STATE_SELECTED
                )) printf ("\nSELECTED\n");
	return TRUE;
}

void
device_listener_test_destroy (void *data)
{
	printf ("Somebody wants to destroy me :-/\n");
}

void
event_listener_generic (const AtspiEvent *event)
{
    printf ("focus: type = %s\n", event->type);
    // object:children-changed:add
    // source: desktop frame | main
    // application: none
}

void
event_listener_generic_destroy (void *data)
{
    printf ("event_listener_focus_destroy\n");
}

