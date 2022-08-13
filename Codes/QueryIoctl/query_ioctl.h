#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H

#include <linux/ioctl.h>
// contains ioctl() function prototype and related macros

typedef struct //structure for querying driver variables
{
  int status, dignity, ego;

} query_arg_t;

/* Following functions are macros, defined in <linux/ioctl.h>
 * They act as the command parameter to ioctl()
 * They contain the command code and additional information,
 * such as direction of operation, size of argument, etc.
 */

#define QUERY_GET_VARIABLES _IOR('q', 1, query_arg_t *) // Direction: Read
#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *) // Direction: Write
#define QUERY_CLR_VARIABLES _IO('q', 2) // Direction: None

#endif
