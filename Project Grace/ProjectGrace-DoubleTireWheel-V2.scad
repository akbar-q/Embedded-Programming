// Project Grace - V2 custom double-tire wheel for the N20-style gearmotor.
// Dimensions are in millimetres. The running surface follows the measured stepped N-profile.

$fn = 96;

// Measured N-profile for each tyre. The smaller outer lands and larger central
// ledge form the stepped running surface visible in the reference wheel.
outer_land_diameter = 27.4;
centre_ledge_diameter = 30.88;
wheel_profile_width = 6.5;
centre_ledge_width = 3.5;
outer_land_width = (wheel_profile_width - centre_ledge_width) / 2;
n_profile_width = wheel_profile_width;
wheel_pair_gap = 9;
wheel_width = 2 * n_profile_width + wheel_pair_gap;
core_diameter = outer_land_diameter;

// Measured D-shaft: 2.85 mm circular diameter and 2.3 mm from the flat
// to the opposite round edge. A small allowance keeps the printed bore usable.
shaft_diameter = 2.85;
shaft_clearance = 0.25;
shaft_flat_to_round = 2.3;
shaft_flat_from_center = shaft_flat_to_round - shaft_diameter / 2 + shaft_clearance / 2;

module d_shaft_bore() {
    bore_diameter = shaft_diameter + shaft_clearance;
    bore_radius = bore_diameter / 2;

    difference() {
        cylinder(d = bore_diameter, h = wheel_width + 2, center = true);
        translate([shaft_flat_from_center, -bore_radius - 1, -wheel_width / 2 - 2])
            cube([bore_radius + 2, 2 * bore_radius + 2, wheel_width + 4]);
    }
}

module n_profile_tire(z_position) {
    outer_radius = outer_land_diameter / 2;
    centre_radius = centre_ledge_diameter / 2;
    centre_half_width = centre_ledge_width / 2;

    // One measured N-profile: two 27.4 mm lands around a 30.88 mm centre ledge.
    translate([0, 0, z_position])
        rotate_extrude($fn = 96)
            polygon(points = [
                [0, -n_profile_width / 2],
                [outer_radius, -n_profile_width / 2],
                [outer_radius, -centre_half_width],
                [centre_radius, -centre_half_width],
                [centre_radius, centre_half_width],
                [outer_radius, centre_half_width],
                [outer_radius, n_profile_width / 2],
                [0, n_profile_width / 2]
            ]);
}

module double_tire_wheel_v2() {
    tyre_offset = (n_profile_width + wheel_pair_gap) / 2;

    difference() {
        union() {
            n_profile_tire(-tyre_offset);
            n_profile_tire(tyre_offset);
            // Full-width core prevents the two tyre sections shearing at the centre joint.
            cylinder(d = core_diameter, h = wheel_width, center = true);
        }

        d_shaft_bore();
    }
}

color("orange")
    double_tire_wheel_v2();
