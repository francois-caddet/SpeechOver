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

// Device listeners prototypes
//AtspiAccessible* SOFocus;
static GError **error = NULL;
static SPDConnection* spdCo;
static glong SO_index(0);

static gchar*
SO_get_label(AtspiAccessible *accessible)
{
  GArray *relations;
  AtspiRelation *relation;
  gint i;
  gchar *result = "";

  relations = atspi_accessible_get_relation_set (accessible, NULL);
  if (relations == NULL) {
    return "";
  }

  for (i = 0; i < relations->len; i++) {
    relation = g_array_index (relations, AtspiRelation*, i);

    if (atspi_relation_get_relation_type (relation) == ATSPI_RELATION_LABELLED_BY) {
      result = atspi_accessible_get_name (atspi_relation_get_target (relation, 0), NULL);
    }
  }

  if (relations != NULL)
    g_array_free (relations, TRUE);

  return result;
}

gchar* SO_get_description(AtspiAccessible* node) {
        gchar* r = "";
        gchar* name = atspi_accessible_get_name(node, error);
        gchar* description = atspi_accessible_get_description(node, error);
        if (!g_strcmp0(name, "")) name = SO_get_label(node);
	if (!g_strcmp0(name, "")) return description;
        else return name;
        return r;
}

gboolean
SO_is_traversable(AtspiAccessible* node, gboolean checkChild = TRUE) {
	if (!checkChild || atspi_accessible_get_child_count(node, error)) {
		if (!g_strcmp0(SO_get_description(node), "")) {
			AtspiRole role = atspi_accessible_get_role(node, error);

			if (role == ATSPI_ROLE_FILLER) return TRUE;
			if (role == ATSPI_ROLE_PANEL) return TRUE;
			if (role == ATSPI_ROLE_SECTION) return TRUE;
			if (role == ATSPI_ROLE_INTERNAL_FRAME) return TRUE;
		}
	}
	return FALSE;
}

gboolean
SO_is_invalid(AtspiAccessible* node) {
	if (node) {
		AtspiStateSet* states = atspi_accessible_get_state_set(node);
		if (atspi_accessible_get_role(node, error) == ATSPI_ROLE_INVALID
			|| (!atspi_state_set_contains(states, ATSPI_STATE_VISIBLE)
				&& atspi_accessible_get_component_iface(node))
			|| (SO_is_traversable(node, FALSE)
				&& !atspi_accessible_get_child_count(node, error))
			) return TRUE;
		return FALSE;
	}
	return TRUE;
}

AtspiAccessible* SO_move(AtspiAccessible* node, glong to);

AtspiAccessible*
SO_interact(AtspiAccessible* node, glong i = 0, gboolean cyclic = false) {
	glong nbchild = atspi_accessible_get_child_count(node, error);
	glong sens = 0;
	if (nbchild) {
		if (i < 0) {
			sens = -1;
			while (i < 0) i += nbchild;
		}	
		else {
			sens = 1;
			i %= nbchild;
		}
		printf("sens: %i\n", sens);
		AtspiAccessible* child = NULL;
		glong j = i;
		while (i < nbchild && i >= 0
			&& SO_is_invalid(child)) {
			child = atspi_accessible_get_child_at_index(node, i, error);
			i += sens;
		}
		if (!SO_is_traversable(node) && SO_is_invalid(child) && cyclic) {
			i += -sens * nbchild;
			while (i != j && SO_is_invalid(child)) {
				child = atspi_accessible_get_child_at_index(node, i, error);
                        	i += sens;
			}
		}
		if (!SO_is_invalid(child)) {
			if ((SO_index = atspi_accessible_get_index_in_parent(child, error)) < 0)
				SO_index = i - sens;
			printf("interact to index: %i / %i\n", SO_index, nbchild - 1);
			if (SO_is_traversable(child)){
				if (AtspiAccessible* r = SO_interact(child, (sens - 1) / 2)) return r;
				else return SO_move(child, sens);
			}
			return child;
		}
	}
	printf("interact NULL");
	return NULL;
}

AtspiAccessible*
SO_uninteract(AtspiAccessible* node){
        AtspiAccessible* parent = atspi_accessible_get_parent(node, error);
	if (parent) {
		SO_index = atspi_accessible_get_index_in_parent(parent, error);
		printf("uninteract at index: %i\n", SO_index);
		if (SO_index < 0) SO_index = 0;
		if (SO_is_traversable(parent)) return SO_uninteract(parent);
		return parent;
	}
	printf("uninteract NULL");
	return NULL;
}

AtspiAccessible*
SO_move(AtspiAccessible* node, glong to){
	if (AtspiAccessible* parent = atspi_accessible_get_parent(node, error)){
		glong go = atspi_accessible_get_index_in_parent(node, error);
		if (go < 0) go = SO_index;
		go += to;
		glong nbbrow = atspi_accessible_get_child_count(parent, error);;
		printf("uninteract at index: %i\n", atspi_accessible_get_index_in_parent(parent, error));
		if (go < 0 || go >= nbbrow){
			if (SO_is_traversable(parent)) {
				SO_index = atspi_accessible_get_index_in_parent(parent, error);
				
				return SO_move(parent, to);
			}
			else return NULL;
			go = (to - 1)/2;
		}
		else if (to < 0) go -= nbbrow;
		if (AtspiAccessible* r = SO_interact(parent, go)) return r;
		else return SO_move(parent, to); 
	}
	printf("qo to NULL");
	return NULL;
}

gchar*
SO_perform_action(AtspiAccessible* node, gint i = 0) {
	if (AtspiAction* act = atspi_accessible_get_action(node)) {
		atspi_action_do_action(act, i, error);
		return " ";
	}
	return NULL;
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
	AtspiAccessible* desktop = NULL;
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
	test_listener = atspi_device_listener_new (&device_listener_test, &desktop, &device_listener_test_destroy);

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

	atspi_event_main();

	atspi_status = atspi_exit ();
	spd_close(spdCo);
	printf ("Stopping atspi2 [status = %i]\n", atspi_status);

	return atspi_status; 
}

gboolean
device_listener_test (const AtspiDeviceEvent *stroke, void* user_data)
{

	printf ("\n%s: %i\n", stroke->event_string, stroke->hw_code);
	gchar* toSay = NULL;
	AtspiAccessible** focus = (AtspiAccessible**)user_data;
	AtspiAccessible* next = NULL;
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
	if (toSay)
		spd_sayf (spdCo, SPD_TEXT, "description: %s\n", toSay);
	else if (next)
		spd_sayf (spdCo, SPD_TEXT, "%s, %s\n", SO_get_description(*focus), atspi_accessible_get_role_name(*focus, error));
	else spd_sayf(spdCo, SPD_TEXT, "\a");
	/*GArray* relations = atspi_accessible_get_relation_set(*focus, error);
	for (gint i = 0; i < relations->len; ++i){
		AtspiRelation* r = g_array_indew(atr, AtspiRelation*, i);
	}*/
	
	if (GArray* ifaces = atspi_accessible_get_interfaces(*focus)) {
		printf ("\nINTERFACES\n");
		for (gint i = 0; i < ifaces->len; ++i)
			printf("\t%s\n", g_array_index(ifaces, gchar*, i));
	}

	if (AtspiAction* act = atspi_accessible_get_action(*focus)) {
		printf ("\nACTIONS\n");
		for (gint i = 0; i < atspi_action_get_n_actions(act, error); ++i)
			printf ("\t%s (%s): %s\n",
				atspi_action_get_action_name(act, i, error),
				atspi_action_get_key_binding(act, i, error),
				atspi_action_get_action_description(act, i, error)
			);
	}
	
	if (atspi_state_set_contains(
			atspi_accessible_get_state_set(*focus),
			ATSPI_STATE_ACTIVE
		)) printf ("\nACTIVE\n");
	else if (atspi_state_set_contains(
                        atspi_accessible_get_state_set(*focus),
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

