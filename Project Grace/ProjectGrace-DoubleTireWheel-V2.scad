// Project Grace - V2 lightweight dual-tyre wheel for the N20-style gearmotor.
// Dimensions are in millimetres. Measure the actual tyre and D-shaft before printing.

$fn = 96;

// The IMechE brief specifies a commercial 22 mm copper pipe.
pipe_outer_diameter = 22;

// Wheel envelope.
wheel_diameter = 42;
wheel_width = 18;
hub_diameter = 12;

// Double-cone running face: its narrow waist clears the pipe between the tyres.
double_cone_waist_diameter = 34;

// Two continuous tyre seats retain the rubber tyres.
tire_groove_width = 5;
tire_groove_depth = 1.5;
tire_groove_spacing = 4;
tire_band_edge_margin = 1.5;
tire_ring_inner_diameter = 28;

// Five curved spokes make the centre genuinely open and keep the load path symmetrical.
spoke_count = 5;
spoke_width = 4;
spoke_depth = 5;
spoke_outer_radius = 15;
spoke_sweep = 4.5;

// N20-style gearmotor output. The D-bore needs print clearance.
shaft_diameter = 3;
shaft_clearance = 0.25;
shaft_flat_from_center = 0.95;

module d_shaft_bore() {
    bore_diameter = shaft_diameter + shaft_clearance;
    bore_radius = bore_diameter / 2;

    difference() {
        cylinder(d = bore_diameter, h = wheel_width + 2, center = true);
        translate([shaft_flat_from_center, -bore_radius - 1, -wheel_width / 2 - 2])
            cube([bore_radius + 2, 2 * bore_radius + 2, wheel_width + 4]);
    }
}

function double_cone_surface_radius(z_position) =
    double_cone_waist_diameter / 2
    + (wheel_diameter - double_cone_waist_diameter) / 2
        * abs(z_position) / (wheel_width / 2);

module tire_groove(z_position) {
    translate([0, 0, z_position])
        rotate_extrude()
            translate([
                double_cone_surface_radius(z_position) - tire_groove_depth / 2,
                0
            ])
                circle(d = tire_groove_width, $fn = 48);
}

module conical_tire_ring(z_position) {
    band_half_width = tire_groove_width / 2 + tire_band_edge_margin;
    band_start_z = z_position - band_half_width;
    band_end_z = z_position + band_half_width;

    difference() {
        translate([0, 0, band_start_z])
            cylinder(
                d1 = 2 * double_cone_surface_radius(band_start_z),
                d2 = 2 * double_cone_surface_radius(band_end_z),
                h = 2 * band_half_width
            );
        cylinder(d = tire_ring_inner_diameter, h = wheel_width + 2, center = true);
    }
}

module curved_spoke(angle) {
    rotate([0, 0, angle])
        hull() {
            translate([hub_diameter / 3, 0, -spoke_depth / 2])
                cylinder(d = spoke_width, h = spoke_depth, $fn = 32);
            translate([spoke_outer_radius, spoke_sweep, -spoke_depth / 2])
                cylinder(d = spoke_width, h = spoke_depth, $fn = 32);
        }
}

module double_tire_wheel_v2() {
    groove_offset = (tire_groove_width + tire_groove_spacing) / 2;

    difference() {
        union() {
            // Annular tyre rings preserve the two double-cone tyre seats.
            conical_tire_ring(-groove_offset);
            conical_tire_ring(groove_offset);
            cylinder(d = hub_diameter, h = wheel_width + 2, center = true);

            // Each spoke overlaps both rings through the narrow central gap.
            for (angle = [0 : 360 / spoke_count : 359])
                curved_spoke(angle);
        }

        d_shaft_bore();
        tire_groove(-groove_offset);
        tire_groove(groove_offset);
    }
}

color("orange")
    double_tire_wheel_v2();
