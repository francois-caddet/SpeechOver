#ifndef _SO_FOCUS_H_
#define _SO_FOCUS_H_

#include <atspi/atspi.h>

G_BEGIN_DECLS

#define SO_TYPE_FOCUS so_focus_get_type ()
G_DECLARE_DERIVABLE_TYPE (SoFocus, so_focus, SO, FOCUS, AtspiAccessible)

     struct _SoFocus {
       AtspiAccessible parent_interface;
       glong index;
       
     }

struct _SoFocusClass {
  AtspiAccessibleClass parent_class;
}

SoFocus*
_so_focus_new(void);

G_END_DECLS

#endif
