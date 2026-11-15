#include "navigation.h"

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