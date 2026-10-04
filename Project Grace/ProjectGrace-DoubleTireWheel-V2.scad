// Project Grace - V2 custom double-cone wheel for the N20-style gearmotor.
// Dimensions are in millimetres. The narrow waist provides clearance around the pipe.

$fn = 96;

// Double-cone profile: a reduced 25.4 mm waist across the centre ledge,
// flaring to 30.88 mm at both outer edges for a pipe-relief groove.
waist_diameter = 25.4;
outer_edge_diameter = 30.88;
cone_flank_width = 6.5;
waist_width = 3.5;
wheel_width = 2 * cone_flank_width + waist_width;

// Circumferential stepped seats for the two rubber tires, one per cone flank.
tire_groove_width = 3.5;
tire_groove_depth = 1.2;
tire_groove_floor_width = 3.0;
tire_gap_reduction = 1;
tire_groove_offset = waist_width / 2 + cone_flank_width / 2 - tire_gap_reduction / 2;
side_emblem_radius = 14.3;
side_emblem_line_width = 1.0;
side_emblem_depth = 0.6;
pentagram_radius = 12.5;

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

function cone_surface_radius(z_position) =
    waist_diameter / 2
    + (outer_edge_diameter - waist_diameter) / 2
        * min(1, max(0, (abs(z_position) - waist_width / 2) / cone_flank_width));

module tire_seat_groove(z_position) {
    half_groove_width = tire_groove_width / 2;
    half_floor_width = tire_groove_floor_width / 2;
    z_start = z_position - half_groove_width;
    z_end = z_position + half_groove_width;
    floor_radius = cone_surface_radius(z_position) - tire_groove_depth;
    cutter_outer_radius = outer_edge_diameter / 2 + 1;

    rotate_extrude($fn = 128)
        polygon(points = [
            [floor_radius, z_position - half_floor_width],
            [cone_surface_radius(z_start) + 0.2, z_start],
            [cutter_outer_radius, z_start],
            [cutter_outer_radius, z_end],
            [cone_surface_radius(z_end) + 0.2, z_end],
            [floor_radius, z_position + half_floor_width]
        ]);
}

module double_cone_body() {
    waist_radius = waist_diameter / 2;
    outer_edge_radius = outer_edge_diameter / 2;
    half_width = wheel_width / 2;
    waist_half_width = waist_width / 2;

    rotate_extrude($fn = 128)
        polygon(points = [
            [0, -half_width],
            [outer_edge_radius, -half_width],
            [waist_radius, -waist_half_width],
            [waist_radius, waist_half_width],
            [outer_edge_radius, half_width],
            [0, half_width]
        ]);
}

function pentagram_point(index) = [
    pentagram_radius * cos(-90 + 72 * index),
    pentagram_radius * sin(-90 + 72 * index)
];

module pentagram_side_emblem_2d() {
    path_order = [0, 2, 4, 1, 3, 0];

    union() {
        difference() {
            circle(r = side_emblem_radius, $fn = 64);
            circle(r = side_emblem_radius - side_emblem_line_width, $fn = 64);
        }

        for (edge = [0 : 4])
            hull() {
                translate(pentagram_point(path_order[edge]))
                    circle(d = side_emblem_line_width, $fn = 16);
                translate(pentagram_point(path_order[edge + 1]))
                    circle(d = side_emblem_line_width, $fn = 16);
            }
    }
}

module double_tire_wheel_v2() {
    difference() {
        union() {
            double_cone_body();
            // Continuous waist core keeps both conical sides strongly joined.
            cylinder(d = waist_diameter, h = wheel_width, center = true);
        }

        d_shaft_bore();
        tire_seat_groove(-tire_groove_offset);
        tire_seat_groove(tire_groove_offset);

        translate([0, 0, wheel_width / 2 - side_emblem_depth])
            linear_extrude(height = side_emblem_depth)
                pentagram_side_emblem_2d();
        mirror([0, 0, 1])
            translate([0, 0, wheel_width / 2 - side_emblem_depth])
                linear_extrude(height = side_emblem_depth)
                    pentagram_side_emblem_2d();
    }
}

color("orange")
    double_tire_wheel_v2();
