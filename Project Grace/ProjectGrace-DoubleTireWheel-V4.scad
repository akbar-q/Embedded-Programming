// Project Grace - V4 twin N-profile tire wheel with deep split-spoke faces.
// The tire seats and measured N20 D-shaft fit are retained from V2.

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

// Recessed five-way Y spokes leave a solid hub and a continuous outer rim.
spoke_count = 5;
spoke_hub_radius = 5.7;
spoke_pocket_outer_radius = 12.85;
spoke_recess_depth = 2.8;
spoke_fork_radius = 8.0;
spoke_tip_radius = 12.45;
spoke_branch_angle = 14;
spoke_main_width = 3.6;
spoke_branch_width = 2.8;
spoke_tip_width = 2.5;

lug_circle_radius = 4.15;
lug_recess_diameter = 1.8;
lug_recess_depth = 1.0;

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

module spoke_rib_segment_2d(start_radius, start_angle, start_width, end_radius, end_angle, end_width) {
    hull() {
        translate([start_radius * cos(start_angle), start_radius * sin(start_angle)])
            circle(d = start_width, $fn = 24);
        translate([end_radius * cos(end_angle), end_radius * sin(end_angle)])
            circle(d = end_width, $fn = 24);
    }
}

module split_spoke_web_2d() {
    union() {
        circle(r = spoke_hub_radius, $fn = 96);

        for (index = [0 : spoke_count - 1]) {
            angle = index * 360 / spoke_count;

            spoke_rib_segment_2d(
                spoke_hub_radius - 0.8,
                angle,
                spoke_main_width,
                spoke_fork_radius,
                angle,
                spoke_main_width
            );
            for (branch = [-1, 1])
                spoke_rib_segment_2d(
                    spoke_fork_radius,
                    angle,
                    spoke_branch_width,
                    spoke_tip_radius,
                    angle + branch * spoke_branch_angle,
                    spoke_tip_width
                );
        }
    }
}

module split_spoke_recess_mask_2d() {
    difference() {
        circle(r = spoke_pocket_outer_radius, $fn = 128);
        split_spoke_web_2d();
    }
}

module split_spoke_face_recesses() {
    half_width = wheel_width / 2;

    translate([0, 0, half_width - spoke_recess_depth])
        linear_extrude(height = spoke_recess_depth + 0.01)
            split_spoke_recess_mask_2d();
    translate([0, 0, -half_width - 0.01])
        linear_extrude(height = spoke_recess_depth + 0.01)
            split_spoke_recess_mask_2d();
}

module lug_recesses() {
    half_width = wheel_width / 2;

    for (side = [-1, 1])
        for (index = [0 : spoke_count - 1]) {
            angle = index * 360 / spoke_count;
            z_start = side > 0 ? half_width - lug_recess_depth : -half_width - 0.01;

            translate([
                lug_circle_radius * cos(angle),
                lug_circle_radius * sin(angle),
                z_start
            ])
                cylinder(d = lug_recess_diameter, h = lug_recess_depth + 0.02, $fn = 32);
        }
}

module double_tire_wheel_v4() {
    difference() {
        union() {
            twin_n_profile_tire_seats();
            // Continuous waist core joins both tire seats and reinforces the web.
            cylinder(d = waist_diameter, h = wheel_width, center = true);
        }

        d_shaft_bore();
        coin_edge_reliefs();
        split_spoke_face_recesses();
        lug_recesses();
    }
}

color("orange")
    double_tire_wheel_v4();