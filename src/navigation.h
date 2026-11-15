#ifndef NAVIGATION_H
#define NAVIGATION_H

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

#endif