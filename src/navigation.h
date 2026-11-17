#ifndef NAVIGATION_H
#define NAVIGATION_H
#include <stdbool.h>

/*
 * Calculate the shortest turn from current heading to target bearing.
 *
 * Inputs must be finite angles in [0, 360) degrees.
 * Positive result: turn right.
 * Negative result: turn left.
 * Zero: aligned.
 * Exactly opposite: consistently choose right (+180 degrees).
 */
float navigation_heading_error(float current_heading_deg,
                               float target_bearing_deg);

struct nav_position {
    double latitude_deg; /* Decimal Degrees: [-90, 90] */
    double longitude_deg; /* Decimal Degrees: [-180, 180] */
};

struct nav_guidance {
    double distance_m;
    double target_bearing_deg;
    float heading_error_deg;
};

/*
 * Calculate distance in metres between two geographic positions.
 *
 * Returns true on success and writes the distance to *distance_m.
 * Returns false for invalid coordinates or a NULL output pointer.
 * On failure, the output is left unchanged.
 */
bool navigation_distance_m(struct nav_position current,
                           struct nav_position destination,
                           double *distance_m);

bool navigation_initial_bearing_deg(struct nav_position current,
                               struct nav_position destination,
                               double *bearing_deg);

/*
 * Calculate guidance using a heading measured from true north.
 *
 * current_heading_deg must be finite and in [0, 360).
 * Returns true and writes *guidance on success.
 * Returns false and leaves *guidance unchanged on failure.
 */
bool navigation_calculate_guidance(struct nav_position current,
                                   struct nav_position destination,
                                   float current_heading_deg,
                                   struct nav_guidance *guidance);

/* Temporary test entry point. */
void navigation_run_validation_tests(void);

#endif