#include "navigation.h"
#include <math.h>
#include <zephyr/sys/printk.h>

static bool position_is_valid(struct nav_position position)
{
    /* Reject non-finite coordinates and values outside valid ranges. */
    return isfinite(position.latitude_deg) && isfinite(position.longitude_deg) && 
           position.latitude_deg >= -90.0 && position.latitude_deg <= 90.0 && 
           position.longitude_deg >= -180.0 && position.longitude_deg <= 180.0;
}

void navigation_run_validation_tests(void)
{
    struct nav_position position = {
        .latitude_deg = 0.0,
        .longitude_deg = 0.0
    };

    bool actual = position_is_valid(position);

    printk("Valid origin: %s\n", actual ? "PASS" : "FAIL");
}

float navigation_heading_error(float current_heading_deg,
                               float target_bearing_deg)
{
    float error_deg = target_bearing_deg - current_heading_deg;

    if (error_deg > 180.0f) {
        error_deg -= 360.0f;
    } else if (error_deg <= -180.0f) {
        error_deg += 360.0f;
    }

    return error_deg;
}