#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include<linux/miscdevice.h>
#include<asm/uaccess.h>
#include<linux/fs.h>
#include<linux/slab.h>


#define DEVICE_NAME "mydevShaishav"
#define bufferSize 256


static int open_device(struct inode*,struct file *);
static int close_device(struct inode*, struct file *);
static ssize_t read_device(struct file *file, char __user * out, size_t size, loff_t * off);


static struct file_operations mydev_fops = {
	.owner = THIS_MODULE,	
	.open = open_device,
	.read = read_device,
	.release = close_device
};

static struct miscdevice mydev_device  = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &mydev_fops
};

char * get_task_state(long state)  /* Function to return task state */
{
    switch (state) {
        case TASK_RUNNING:
            return "TASK_RUNNING";
        case TASK_INTERRUPTIBLE:
            return "TASK_INTERRUPTIBLE";
        case TASK_UNINTERRUPTIBLE:
            return "TASK_UNINTERRUPTIBLE";
        case __TASK_STOPPED:
            return "__TASK_STOPPED";
        case __TASK_TRACED:
            return "__TASK_TRACED";
        case TASK_PARKED:
            return "TASK_PARKED";
        case TASK_DEAD:
            return "TASK_DEAD";
        case TASK_WAKEKILL:
            return "TASK_WAKEKILL";
        case TASK_WAKING:
            return "TASK_WAKING";
        case TASK_NOLOAD:
            return "TASK_NOLOAD";
        case TASK_NEW:
            return "TASK_NEW";
        case TASK_KILLABLE:
            return "TASK_KILLABLE";
        case TASK_STOPPED:
            return "TASK_STOPPED";
        case TASK_TRACED:
            return "TASK_TRACED";
        case TASK_IDLE:
            return "TASK_IDLE";
        case TASK_NORMAL:
            return "TASK_NORMAL";
        default:
        {
            return "INVALID";
        }
    }
}

static struct task_struct * curr;        /*    Structure needed to iterate through current task    */

int mydev_init(void)                    /*    Init Module    */
{
    printk(KERN_INFO "%s","Device initializing...\n");
    if(misc_register(&mydev_device)<0){
        printk(KERN_ALERT "Device registration failed\n");
        return 0;
    }
    curr = next_task(&init_task);
    return 0;
}

static int open_device(struct inode * inode,struct file * file) {
	printk(KERN_INFO "Opening device\n");
	curr = next_task(&init_task);
	return 0;
}

static ssize_t read_device(struct file *file, char *out, size_t size, loff_t * off)
{
	int status;
	char proc[bufferSize];
	struct task_struct * task;  /*    Structure defined in sched.h for tasks/processes    */
	
	memset(proc,0,sizeof(char)*bufferSize);  /* Setting memory to some value */
	printk(KERN_INFO "Reading\n");

	for_each_process(task){
		if(curr == task){
			char* state = get_task_state(task->state);
			memset(proc,0,sizeof(char)*bufferSize);
			sprintf(proc, "PID=%d PPID=%d CPU=%d STATE=%s",task->pid,task->parent->pid,task->on_cpu,state);
			printk(KERN_INFO "Sending: %s",proc);
			status = raw_copy_to_user(out, proc, strlen(proc));
			if(status !=0)
			{	
				printk(KERN_ALERT "Error in copying data to user!");
				return -EFAULT;
			}
	curr = next_task(task);
			break;
		}	
	}
	return strlen(proc);
}

static int close_device(struct inode * inode,struct file *file) {
	printk(KERN_INFO "Device is now closed\n");
	curr = &init_task;
	return 0;
}

void mydev_exit(void)        /*    Exit Module    */
{
    printk(KERN_INFO "%s","REMOVING MODULE\n");
    misc_deregister(&mydev_device);
}                /*    End of Exit Module    */

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Send all running process data to user process");
MODULE_AUTHOR("Shaishav/Jeet");