// Project Grace - preloaded, clip-on tension spring for opposing arm pairs.
// Fit it after the climber is placed around the pipe, then lock the eyelets to the arm posts.

$fn = 48;

view_mode = "band";

// Match this to the centre-to-centre spacing of two opposing chassis anchor posts.
anchor_spacing = 94;
preload_extension = 3;

// Print flat so it flexes in the XY plane.
// Thick ends resist wear at the anchors; the full span between them is the spring.
band_thickness = 2.5;
mount_beam_width = 4;
spring_beam_width = 1.6;
serpentine_amplitude = 4;
serpentine_sections = 16;

// Closed eyelets cannot release from their posts once the locking cap is fitted.
anchor_post_diameter = 3;
eyelet_clearance = 0.35;
locking_cap_diameter = 6;
locking_cap_height = 2;
locking_cap_bore = 2.85;

function free_anchor_spacing() = anchor_spacing - preload_extension;
function eyelet_outer_radius() = (
    anchor_post_diameter + 2 * eyelet_clearance + 2 * mount_beam_width
) / 2;
function spring_start_x() = -free_anchor_spacing() / 2 + eyelet_outer_radius();
function spring_end_x() = free_anchor_spacing() / 2 - eyelet_outer_radius();
function spring_pitch() = (
    spring_end_x() - spring_start_x()
) / serpentine_sections;

function fold_y(section) =
    section % 2 == 0
            ? serpentine_amplitude
            : -serpentine_amplitude;

module beam(point_a, point_b, width) {
    hull() {
        translate([point_a[0], point_a[1], 0])
            cylinder(d = width, h = band_thickness, $fn = 24);
        translate([point_b[0], point_b[1], 0])
            cylinder(d = width, h = band_thickness, $fn = 24);
    }
}

module full_span_accordion() {
    // Thick necks meet the closed eyelets; every section between them is a folded flexure.
    beam(
        [-free_anchor_spacing() / 2, eyelet_outer_radius()],
        [spring_start_x(), eyelet_outer_radius()],
        mount_beam_width
    );
    beam(
        [spring_start_x(), eyelet_outer_radius()],
        [spring_start_x(), fold_y(0)],
        spring_beam_width
    );

    for (section = [1 : serpentine_sections]) {
        beam(
            [spring_start_x() + (section - 1) * spring_pitch(), fold_y(section - 1)],
            [spring_start_x() + section * spring_pitch(), fold_y(section - 1)],
            spring_beam_width
        );
        beam(
            [spring_start_x() + section * spring_pitch(), fold_y(section - 1)],
            [spring_start_x() + section * spring_pitch(), fold_y(section)],
            spring_beam_width
        );
    }
    beam(
        [spring_end_x(), fold_y(serpentine_sections)],
        [spring_end_x(), eyelet_outer_radius()],
        mount_beam_width
    );
    beam(
        [spring_end_x(), eyelet_outer_radius()],
        [free_anchor_spacing() / 2, eyelet_outer_radius()],
        mount_beam_width
    );
}

module closed_eyelet(x_position) {
    difference() {
        translate([x_position, 0, 0])
            cylinder(
                d = 2 * eyelet_outer_radius(),
                h = band_thickness,
                $fn = 48
            );
        translate([x_position, 0, -1])
            cylinder(
                d = anchor_post_diameter + 2 * eyelet_clearance,
                h = band_thickness + 2,
                $fn = 48
            );
    }
}

module locking_cap() {
    difference() {
        cylinder(d = locking_cap_diameter, h = locking_cap_height, $fn = 48);
        translate([0, 0, -1])
            cylinder(d = locking_cap_bore, h = locking_cap_height + 2, $fn = 48);
    }
}

module cross_spring_band() {
    union() {
        full_span_accordion();
        closed_eyelet(-free_anchor_spacing() / 2);
        closed_eyelet(free_anchor_spacing() / 2);
    }
}

if (view_mode == "cap")
    color("orange")
        locking_cap();
else
    color("orange")
        cross_spring_band();
