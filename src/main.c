#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    printk("Outdoor navigation device started\n");

    while (1) {
        printk("Application running\n");
        k_msleep(1000);
    }

    return 0;
}
