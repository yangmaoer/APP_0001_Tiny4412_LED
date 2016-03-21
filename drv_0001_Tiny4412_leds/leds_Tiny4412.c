#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>
 
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

static struct class *led_class;

static int led_gpios[] = {
	EXYNOS4212_GPM4(0),
	EXYNOS4212_GPM4(1),
	EXYNOS4212_GPM4(2),
	EXYNOS4212_GPM4(3),
};


static int led_open(struct inode *inode, struct file *file)
{
	/* ����GPIOΪ������� */
	int i;
	for (i = 0; i < 4; i++)
	{
		s3c_gpio_cfgpin(led_gpios[i], S3C_GPIO_OUTPUT);
	}
	
	return 0;
}

/*app : ioctl(fd, cmd, arg)*/
static long led_ioctl(struct file *filp, unsigned int cmd,unsigned long arg)
{
	/* ���ݴ���Ĳ�������GPIO�ĵ�ƽ */
	/* cmd : 0-off 1-on */
	/* arg: 0-3 which led */
	if ((cmd != 0) && (cmd != 1))
		return -EINVAL;
	if (arg > 4 )
		return -EINVAL;
	
	gpio_set_value(led_gpios[arg], !cmd);
}


static struct file_operations led_fops = {
	.owner = THIS_MODULE,	/*����һ���꣬�������ģ��ʱ�Զ�������_this_module����*/
	.open  = led_open,
	.unlocked_ioctl	= led_ioctl,
};

static int major;
static int led_init(void)
{
	major = register_chrdev(0, "ledTiny4412", &led_fops);

	/* Ϊ����ϵͳudev,mdev�����Ǵ����豸��� */
	/* �����࣬�����´����豸 : /sys/ */
	led_class = class_create(THIS_MODULE, "ledTiny4412");
	if (IS_ERR(led_class))
		return PTR_ERR(led_class);

	device_create(led_class, NULL, MKDEV(major, 0), NULL, "ledTiny4412"); 
	if (unlikely(IS_ERR(led_class)))
		return PTR_ERR(led_class);
	return 0;
}

static void led_exit(void)
{
	unregister_chrdev(major, "ledTiny4412");
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("tinyLed writes fome Grey");
