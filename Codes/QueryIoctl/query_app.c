/* Sysplay Doc Ex 4
 * 
 * Userspace Application query_app.c to query variables from
 * Device Driver query_ioctl.c (kernelspace to userspace)
 */

#include <stdio.h>     // Standard Input and Output
#include <sys/types.h> // Contains data types such as off_t and ssize_t
#include <fcntl.h>     // for open() function
#include <unistd.h>    // for close() function
#include <string.h>    // for string operations in main()
#include <sys/ioctl.h> // contains ioctl() function prototype and related macros

#include "query_ioctl.h" // Header file with type definitions

void get_vars(int fd) // Function to read the variables from the driver
{ // Parameter: file descriptor of device file

  query_arg_t q; // structure for querying driver variables

  if (ioctl(fd, QUERY_GET_VARIABLES, &q) == -1) // Call _IOR()
    perror("query_apps ioctl get"); // Display error if failed to query
  else // Display the queried variables
  {
    printf("Status : %d\n", q.status);
    printf("Dignity: %d\n", q.dignity);
    printf("Ego    : %d\n", q.ego);
  }
}

void set_vars(int fd) // Function to write the variables to the driver
{ // Parameter: file descriptor of device file

  query_arg_t q; //structure for querying driver variables
  int v; // Variable to take values from user

  printf("Enter the variables to be written to the driver\n");
  printf("\nEnter Status: ");
  scanf("%d", &v);
  q.status = v;
  printf("\nEnter Dignity: ");
  scanf("%d", &v);
  q.dignity = v;
  printf("\nEnter Ego: ");
  scanf("%d", &v);
  q.ego = v;
  printf("\nVariables set successfully.\n");

  if (ioctl(fd, QUERY_SET_VARIABLES, &q) == -1) // Call _IOW()
    perror("query_apps ioctl set"); // Display error if failed to query
}

void clr_vars(int fd) // Function to clear the variables in the driver
{// Parameter: file descriptor of device file

  if (ioctl(fd, QUERY_CLR_VARIABLES) == -1) // Call _IO()
    perror("query_apps ioctl clr"); // Display error if failed to query
  else
    printf("\nVariables cleared successfully.\n");
}

int main(int argc, char *argv[]) // Takes flags (-g/-s/-c) as arguments
{
  char *file_name = "/dev/QueryIoctl0"; // Device File
  int fd; // File descriptor of device file
  enum // Options for ioctl() command parameter
  {
    e_get,
    e_set,
    e_clr
  } option;
  if (argc == 1) // Default run of application (same as -g flag)
    option = e_get;
  else if (argc == 2) // Application was run with a flag (-g/-s/-c)
  {
    if (strcmp(argv[1], "-g") == 0)
      option = e_get;
    else if (strcmp(argv[1], "-s") == 0)
      option = e_set;
    else if (strcmp(argv[1], "-c") == 0)
      option = e_clr;
    else
    {// invalid flag used
      fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
      return 1;
    }
  }
  else
  {// invalid flag used
    fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
    return 1;
  }
  fd = open(file_name, O_RDWR); // Returns file descriptor of device file
  // O_RDWR is a flag passed to open(), allowing read and write operations
  if (fd == -1)
  {
    perror("query_apps open"); // Error in opening device file
    return 2;
  }
  switch (option) // Call the required query function
  {
    case e_get:
      get_vars(fd);
      break;
    case e_set:
      set_vars(fd);
      break;
    case e_clr:
      clr_vars(fd);
      break;
    default:
      break;
  }
  close (fd); // Close the device file
  return 0;
}
