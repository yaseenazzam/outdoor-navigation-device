#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "navigation.h"

struct heading_test {
    float current;
    float target;
    float expected;
};

static const struct heading_test tests[] = {
    {350.0f,  10.0f,   20.0f},  /* Right across north */
    { 10.0f, 350.0f,  -20.0f},  /* Left across north */
    { 90.0f,  90.0f,    0.0f},  /* Already aligned */
    {  0.0f,  90.0f,   90.0f},  /* Ordinary right turn */
    { 90.0f,   0.0f,  -90.0f},  /* Ordinary left turn */
    {  0.0f, 180.0f,  180.0f},  /* Opposite: choose right */
    {180.0f,   0.0f,  180.0f},  /* Same rule in reverse */
    {  0.0f, 359.0f,   -1.0f},  /* Just below north */
    {359.0f,   0.0f,    1.0f},
    { 10.0f,  10.5f,    0.5f},  /* Fractional angle */
};

int main(void)
{
    const size_t test_count = sizeof(tests) / sizeof(tests[0]);
    size_t passed = 0;

    /* Give the serial monitor time to connect after reset. */
    k_msleep(1000);

    printk("\nNavigation heading tests\n");

    for (size_t i = 0; i < test_count; ++i) {
        float actual = navigation_heading_error(
            tests[i].current, tests[i].target);

        float difference = actual - tests[i].expected;

        /* Compare with a tolerance rather than exact float equality. */
        int matches = (difference >= -0.001f &&
                       difference <=  0.001f);

        if (matches) {
            ++passed;
        }

        /*
         * Print millidegrees as integers to avoid needing floating-point
         * formatting support in printk. 1000 millidegrees = 1 degree.
         */
        printk("Test %u: %s | expected %d, got %d millidegrees\n",
               (unsigned int)(i + 1),
               matches ? "PASS" : "FAIL",
               (int)(tests[i].expected * 1000.0f),
               (int)(actual * 1000.0f));
    }

    printk("Result: %u/%u passed\n",
           (unsigned int)passed,
           (unsigned int)test_count);

    return 0;
}