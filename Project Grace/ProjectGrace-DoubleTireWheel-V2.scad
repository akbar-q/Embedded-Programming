// Project Grace - V2 custom twin N-profile tire wheel for the N20 gearmotor.
// Square shoulders retain both tires; the reduced waist clears the pipe.

$fn = 96;

// Tire-seat profile: 27.4 mm outer lands and 31.88 mm retaining ledges.
waist_diameter = 25.4;
outer_land_diameter = 27.4;
inner_ledge_diameter = 31.88;
seat_profile_width = 6.5;
inner_ledge_width = 3.5;
outer_land_width = seat_profile_width - inner_ledge_width;
waist_width = 9;
wheel_width = 2 * seat_profile_width + waist_width;
coin_edge_groove_count = 72;
coin_edge_groove_depth = 0.5;
coin_edge_groove_width = 0.65;
coin_edge_end_margin = 0;
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

module twin_n_profile_tire_seats() {
    waist_radius = waist_diameter / 2;
    outer_land_radius = outer_land_diameter / 2;
    inner_ledge_radius = inner_ledge_diameter / 2;
    half_width = wheel_width / 2;
    waist_half_width = waist_width / 2;
    inner_ledge_outer_z = waist_half_width + inner_ledge_width;

    // Each tire spans a flat outer land and a raised inner ledge, forming an N seat.
    rotate_extrude($fn = 128)
        polygon(points = [
            [0, -half_width],
            [outer_land_radius, -half_width],
            [outer_land_radius, -inner_ledge_outer_z],
            [inner_ledge_radius, -inner_ledge_outer_z],
            [inner_ledge_radius, -waist_half_width],
            [waist_radius, -waist_half_width],
            [waist_radius, waist_half_width],
            [inner_ledge_radius, waist_half_width],
            [inner_ledge_radius, inner_ledge_outer_z],
            [outer_land_radius, inner_ledge_outer_z],
            [outer_land_radius, half_width],
            [0, half_width]
        ]);
}

module coin_edge_band_relief(radius, center_offset, band_width) {
    for (side = [-1, 1])
        for (index = [0 : coin_edge_groove_count - 1])
            rotate([0, 0, index * 360 / coin_edge_groove_count])
                translate([
                    radius - coin_edge_groove_depth / 2 + 0.02,
                    0,
                    side * center_offset
                ])
                    cube([
                        coin_edge_groove_depth + 0.04,
                        coin_edge_groove_width,
                        band_width - 2 * coin_edge_end_margin
                    ], center = true);
}

module coin_edge_reliefs() {
    inner_ledge_center_offset = waist_width / 2 + inner_ledge_width / 2;
    outer_land_center_offset = waist_width / 2 + inner_ledge_width + outer_land_width / 2;

    coin_edge_band_relief(
        inner_ledge_diameter / 2,
        inner_ledge_center_offset,
        inner_ledge_width
    );
    coin_edge_band_relief(
        outer_land_diameter / 2,
        outer_land_center_offset,
        outer_land_width
    );
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
            twin_n_profile_tire_seats();
            // Continuous waist core keeps both N-profile tire seats joined.
            cylinder(d = waist_diameter, h = wheel_width, center = true);
        }

        d_shaft_bore();
        coin_edge_reliefs();

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
