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

    navigation_run_validation_tests();

    struct nav_position origin = {
        .latitude_deg = 0.0,
        .longitude_deg = 0.0
    };
    double metres = -1.0;

    bool ok = navigation_distance_m(origin, origin, &metres);
    printk("Same position: %s\n", (ok && metres == 0.0) ? "PASS" : "FAIL");

    const struct nav_position east = {
        .latitude_deg = 0.0,
        .longitude_deg = 1.0
    };
    metres = -1.0;

    ok = navigation_distance_m(origin, east, &metres);
    printk("One degree east: %s\n",
           (ok && metres >= 111194.0 && metres <= 111196.0)
               ? "PASS" : "FAIL");

    struct nav_position invalid_destination = {
        .latitude_deg = 91.0,
        .longitude_deg = 0.0
    };
    metres = -1.0;

    ok = navigation_distance_m(origin, invalid_destination, &metres);
    printk("Invalid destination latitude: %s\n",
           (!ok && metres == -1.0) ? "PASS" : "FAIL");

    double bearing_deg = -1.0;
    bool bearing_ok = navigation_initial_bearing_deg(origin, east, &bearing_deg);

    printk("Bearing east: %s\n",
           (bearing_ok && bearing_deg >= 89.999 && bearing_deg <= 90.001)
               ? "PASS" : "FAIL");

    const struct nav_position west = {
        .latitude_deg = 0.0,
        .longitude_deg = -1.0
    };
    bearing_deg = -1.0;
    bearing_ok = navigation_initial_bearing_deg(origin, west, &bearing_deg);

    printk("Bearing west: %s\n",
           (bearing_ok && bearing_deg >= 269.999 && bearing_deg <= 270.001)
               ? "PASS" : "FAIL");

    bearing_deg = -1.0;
    bearing_ok = navigation_initial_bearing_deg(origin, origin, &bearing_deg);

    printk("Bearing same position: %s\n",
           (!bearing_ok && bearing_deg == -1.0) ? "PASS" : "FAIL");
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