// Project Grace - draft dual-tyre wheel for the N20-style gearmotor.
// Dimensions are in millimetres. Measure the actual tyre and D-shaft before printing.

$fn = 96;

// The IMechE brief specifies a commercial 22 mm copper pipe.
pipe_outer_diameter = 22;

// Wheel envelope. The outside diameter matches the existing wheel concept.
wheel_diameter = 42;
wheel_width = 18;
hub_diameter = 16;

// Double-cone running face: its narrow waist clears the pipe between the tyres.
double_cone_waist_diameter = 34;

// Two independent circumferential seats for rubber tyres.
tire_groove_width = 5;
tire_groove_depth = 1.5;
tire_groove_spacing = 4;

// N20-style gearmotor output. The D-bore needs print clearance.
shaft_diameter = 3;
shaft_clearance = 0.25;
shaft_flat_from_center = 0.95;

module d_shaft_bore() {
    bore_diameter = shaft_diameter + shaft_clearance;
    bore_radius = bore_diameter / 2;

    difference() {
        cylinder(d = bore_diameter, h = wheel_width + 2, center = true);
        // Removes one side of the circular bore to match the flat on the D-shaft.
        translate([shaft_flat_from_center, -bore_radius - 1, -wheel_width / 2 - 2])
            cube([bore_radius + 2, 2 * bore_radius + 2, wheel_width + 4]);
    }
}

function double_cone_surface_radius(z_position) =
    double_cone_waist_diameter / 2
    + (wheel_diameter - double_cone_waist_diameter) / 2
        * abs(z_position) / (wheel_width / 2);

module tire_groove(z_position) {
    // A toroidal cut makes a rounded seat for a narrow rubber tyre or O-ring.
    translate([0, 0, z_position])
        rotate_extrude()
            translate([
                double_cone_surface_radius(z_position) - tire_groove_depth / 2,
                0
            ])
                circle(d = tire_groove_width, $fn = 48);
}

module double_cone_wheel_body() {
    half_width = wheel_width / 2;

    // Two cones meet at the narrow waist, creating the pipe-clearance profile.
    translate([0, 0, -half_width])
        cylinder(
            d1 = wheel_diameter,
            d2 = double_cone_waist_diameter,
            h = half_width
        );
    cylinder(
        d1 = double_cone_waist_diameter,
        d2 = wheel_diameter,
        h = half_width
    );
}

module double_tire_wheel() {
    groove_offset = (tire_groove_width + tire_groove_spacing) / 2;

    difference() {
        union() {
            double_cone_wheel_body();
            cylinder(d = hub_diameter, h = wheel_width + 2, center = true);
        }

        d_shaft_bore();
        tire_groove(-groove_offset);
        tire_groove(groove_offset);
    }
}

color("orange")
    double_tire_wheel();
