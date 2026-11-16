#include "navigation.h"
#include <math.h>
#include <zephyr/sys/printk.h>
#include <stddef.h>

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

    position.latitude_deg = 91.0;
    actual = position_is_valid(position);

    printk("Invalid latitude: %s\n", !actual ? "PASS" : "FAIL");

    position.latitude_deg = 0.0;
    position.longitude_deg = 181.0;
    actual = position_is_valid(position);

    printk("Invalid longitude: %s\n", !actual ? "PASS" : "FAIL");
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

const double deg_to_rad = 3.14159265358979323846 / 180.0;

bool navigation_distance_m(struct nav_position current, struct nav_position destination, double *distance_m)
{
    if (!position_is_valid(current) || !position_is_valid(destination) || distance_m == NULL) {
        return false;
    }

    /* Calculate the distance using the Haversine formula. */
    double delta_lat_rad = (destination.latitude_deg - current.latitude_deg) * deg_to_rad;
    double delta_lon_rad = (destination.longitude_deg - current.longitude_deg) * deg_to_rad;
    double radius_earth_m = 6371000.0; /* Average radius of Earth in meters */

    double distance_a;
    double distance_c;
    
    distance_a = sin(delta_lat_rad / 2) * sin(delta_lat_rad / 2) +
                        cos(current.latitude_deg * deg_to_rad) *
                        cos(destination.latitude_deg * deg_to_rad) *
                        sin(delta_lon_rad / 2) * sin(delta_lon_rad / 2);

    if (distance_a < 0.0){
        distance_a = 0.0;
    } else if (distance_a > 1.0){
        distance_a = 1.0;
    }

    distance_c = 2 * atan2(sqrt(distance_a), sqrt(1 -distance_a));
    *distance_m = radius_earth_m * distance_c;  

    return true;
}

bool navigation_initial_bearing_deg(struct nav_position current,struct nav_position destination, double *bearing_deg)
{
    if (!position_is_valid(current) || !position_is_valid(destination) || bearing_deg == NULL) {
        return false;
    }

    if (current.latitude_deg == destination.latitude_deg &&
        current.longitude_deg == destination.longitude_deg) {
        return false;
    } 
    
    double current_lat_rad = current.latitude_deg * deg_to_rad;
    double destination_lat_rad = destination.latitude_deg * deg_to_rad;
    double delta_lon_rad = (destination.longitude_deg - current.longitude_deg) * deg_to_rad;

    double east_component = sin(delta_lon_rad) * cos(destination_lat_rad);
    double north_component = cos(current_lat_rad) * sin(destination_lat_rad) -
                             sin(current_lat_rad) * cos(destination_lat_rad) * cos(delta_lon_rad);

    *bearing_deg = atan2(east_component, north_component) / deg_to_rad;

    if (*bearing_deg < 0.0) {
        *bearing_deg += 360.0;
    }
    if (*bearing_deg >= 360.0) {
        *bearing_deg -= 360.0;
    }

    return true;
}