#include <linux/module.h>   // includes the module related data structures
#include <linux/version.h>  // for version compatibility of the module
#include <linux/kernel.h>   // kernelspace header
#include <linux/fs.h>       // for filesystem operations
#include <linux/uaccess.h>  // contains functions for kernel to access userspace memory safely
#include <linux/seq_file.h> // for read operations via sequence files
#include <linux/proc_fs.h>  // for creation of kernel windows under /proc
#include <linux/jiffies.h>  // contains definitions related to the kernel time unit 'jiffy'

#define STR_PRINTF_RET(len, str, args...) seq_printf(m, str, ## args) // from seq_file.h
// used to print strings into the /proc files

static struct proc_dir_entry *parent, *file, *link;
// pointers to structures that contain information for each /proc entry
static int state = 0; // specifies format for output in read() operation

/* File Operation Read
 * Uses the API seq_printf from seq_file.h
 * Displays the information in the specified /proc file
 * Contains 'state' and time since last boot in various units.
 */
static int time_read(struct seq_file *m, void *v)
// seq_file is an 'iterator object' that helps to read the /proc files
{
  int len=0, val;
  unsigned long act_jiffies; // stores number of jiffies since last boot
  STR_PRINTF_RET(len, "state = %d\n", state);
  act_jiffies = jiffies - INITIAL_JIFFIES;
  // on boot-up, jiffies is initialized to INITIAL_JIFFIES instead of zero
  val = jiffies_to_msecs(act_jiffies); // from linux/jiffies.h
  switch (state)
  {// time output format
    case 0:// jiffies
      STR_PRINTF_RET(len,"time = %ld jiffies\n",act_jiffies);
      break;
    case 1:// milliseconds
      STR_PRINTF_RET(len,"time = %d msecs\n",val);
      break;
    case 2:// seconds and milliseconds
      STR_PRINTF_RET(len,"time = %ds %dms\n",val/1000,val%1000);
      break;
    case 3:// hh:mm:ss
      val/=1000;
      STR_PRINTF_RET(len,"time = %02d:%02d:%02d\n",val/3600,(val/60)%60,val%60);
      break;
    default:
      STR_PRINTF_RET(len, "<Undefined State>\n");
      break;
  }
  return len;
}

/* File Operation Write
 * Allows the user to set the 'state' variable
 */
static ssize_t time_write(struct file *file, const char __user *buffer, size_t count, loff_t *off)
{
  char kbuf[2]; // buffer for taking data from command argument
  if (count > 2)
   return count;
  if (copy_from_user(kbuf, buffer, count)) // copies data from userspace
   return -EFAULT;
  if ((count == 2) && (kbuf[1] != '\n')) // invalid argument
   return count;
  if ((kbuf[0] < '0') || ('9' < kbuf[0])) // invalid argument
   return count;
  state = kbuf[0] - '0'; // convert from char to integer
  return count;
}

static int time_open(struct inode *inode, struct file *file)
{
    return single_open(file, time_read, NULL);
// The single_open function opens the file, and the function
// passed as 2nd parameter is called at output time.
}

static struct file_operations fops ={
    .owner = THIS_MODULE, //macro from <linux/module.h>
    .open = time_open,
    .read = seq_read,
    .write = time_write
}; //structure defined in <linux/fs.h>

static int __init proc_win_init(void) /* Constructor */
{
  if ((parent = proc_mkdir("anil", NULL)) == NULL) // Create directory under /proc
    return -1;
  if ((file = proc_create("rel_time", 0666, parent, &fops)) == NULL) // 0666 specifies the file permissions
  {// Create file under /proc/anil directory, with above defined file operations
    remove_proc_entry("anil", NULL);
    return -1;
  }
  if ((link = proc_symlink("rel_time_l", parent, "rel_time")) == NULL)
  {// Create symlink to the file rel_time under same directory
    remove_proc_entry("rel_time", parent);
    remove_proc_entry("anil", NULL);
    return -1;
  }
  proc_set_user(link, KUIDT_INIT(0), KGIDT_INIT(100)); // Set the user and group IDs
  return 0;
}

static void __exit proc_win_exit(void) /* Destructor */
{
    remove_proc_entry("rel_time_l", parent); // remove symlink
    remove_proc_entry("rel_time", parent);   // remove file
    remove_proc_entry("anil", NULL);         // remove directory
}

module_init(proc_win_init);
module_exit(proc_win_exit); // Specify module init and exit functions

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akshay Aserkar");
MODULE_DESCRIPTION("Kernel Window /proc Demonstration Driver");
