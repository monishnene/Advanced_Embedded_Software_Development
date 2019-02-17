#include <linux/timer.h>
#include <linux/stat.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Monish Nene");
MODULE_DESCRIPTION("Kernel Timer to run a timer with a defined period in background.");
MODULE_VERSION("1.0");
struct timer_list ktime;
static char *name = "Monish Nene";
static unsigned int period = 500;
static unsigned int total_calls = 0;

module_param(name,charp,S_IRUGO);
MODULE_PARM_DESC(name,"Username -> ");
module_param(period,uint,S_IRUGO);
MODULE_PARM_DESC(period,"Period(ms) -> ");

void kernel_timer_log(struct timer_list* timer)
{
	total_calls++;
	mod_timer(&ktime,jiffies+msecs_to_jiffies(period));
	printk(KERN_INFO "my name= %s, counter has fired %d times\n",name,total_calls);
}

static int __init kernel_timer_init(void)
{
	printk(KERN_INFO "Timer Module inserted\n");
	timer_setup(&ktime,kernel_timer_log,0);
	add_timer(&ktime);
	return 0;
}

static void __exit kernel_timer_exit(void)
{
	total_calls=0;
	del_timer(&ktime);
	printk(KERN_INFO "Timer Module exited\n");
}

module_init(kernel_timer_init);
module_exit(kernel_timer_exit);
