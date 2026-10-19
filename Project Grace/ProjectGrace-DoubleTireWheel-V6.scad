// Project Grace - V6 twin N-profile tire wheel with a one-sided shaft offset boss.
// The tire seats and measured N20 D-shaft fit are retained from V5.

$fn = $preview ? 48 : 96;

// Tire-seat profile: 27.4 mm outer lands and 31.88 mm retaining ledges.
waist_diameter = 25.4;
outer_land_diameter = 27.4;
inner_ledge_diameter = 31.88;
seat_profile_width = 6.5;
inner_ledge_width = 3.5;
outer_land_width = seat_profile_width - inner_ledge_width;
outer_retaining_lip_height = 1.85;
outer_retaining_lip_width = 1.2;
outer_retaining_lip_overlap = 0.8;
waist_width = 9;
wheel_width = 2 * seat_profile_width + waist_width;
shaft_offset_boss_length = 2.5;
shaft_offset_boss_overlap = 0.8;
shaft_offset_boss_diameter = 9.2;
coin_edge_groove_count = $preview ? 18 : 72;
coin_edge_groove_depth = 0.5;
coin_edge_groove_width = 0.65;
coin_edge_end_margin = 0;

// Five split spokes leave a solid hub and a continuous outer rim.
spoke_count = 5;
spoke_rotation = 90;
spoke_hub_radius = 4.6;
outer_rim_radial_width = 1.7;
spoke_pocket_outer_radius = outer_land_diameter / 2 - outer_rim_radial_width;
spoke_fork_radius = 7.2;
spoke_tip_radius = spoke_pocket_outer_radius - 0.2;
spoke_branch_angle = 15;
spoke_main_width = 2.2;
spoke_branch_width = 1.9;
spoke_tip_width = 1.8;
spoke_groove_width = 0.45;
spoke_groove_depth = 0.5;

lug_circle_radius = 2.35;
lug_recess_diameter = 0.9;
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
    bore_length = wheel_width + shaft_offset_boss_length + 4;

    difference() {
        translate([0, 0, shaft_offset_boss_length / 2])
            cylinder(d = bore_diameter, h = bore_length, center = true);
        translate([shaft_flat_from_center, -bore_radius - 1, -wheel_width / 2 - 2])
            cube([bore_radius + 2, 2 * bore_radius + 2, bore_length]);
    }
}

module shaft_offset_boss() {
    translate([0, 0, wheel_width / 2 - shaft_offset_boss_overlap])
        cylinder(
            d = shaft_offset_boss_diameter,
            h = shaft_offset_boss_length + shaft_offset_boss_overlap
        );
}

module twin_n_profile_tire_seats() {
    waist_radius = waist_diameter / 2;
    outer_land_radius = outer_land_diameter / 2;
    inner_ledge_radius = inner_ledge_diameter / 2;
    half_width = wheel_width / 2;
    waist_half_width = waist_width / 2;
    inner_ledge_outer_z = waist_half_width + inner_ledge_width;

    rotate_extrude($fn = $preview ? 64 : 128)
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

module outer_retaining_lips() {
    half_width = wheel_width / 2;
    lip_outer_diameter = outer_land_diameter + 2 * outer_retaining_lip_height;
    lip_inner_diameter = waist_diameter;
    lip_total_width = outer_retaining_lip_width;

    for (side = [-1, 1]) {
        lip_start_z = side > 0
            ? half_width - outer_retaining_lip_overlap
            : -half_width - (lip_total_width - outer_retaining_lip_overlap);

        translate([0, 0, lip_start_z])
            difference() {
                cylinder(d = lip_outer_diameter, h = lip_total_width);
                translate([0, 0, -0.01])
                    cylinder(d = lip_inner_diameter, h = lip_total_width + 0.02);
            }
    }
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
            angle = index * 360 / spoke_count + spoke_rotation;

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

module split_spoke_through_cuts() {
    half_width = wheel_width / 2;

    translate([0, 0, -half_width - 0.01])
        linear_extrude(height = wheel_width + 0.02)
            split_spoke_recess_mask_2d();
}

module split_spoke_surface_grooves() {
    half_width = wheel_width / 2;

    for (side = [-1, 1]) {
        z_start = side > 0 ? half_width - spoke_groove_depth : -half_width - 0.01;

        translate([0, 0, z_start])
            linear_extrude(height = spoke_groove_depth + 0.02)
                union() {
                    for (index = [0 : spoke_count - 1]) {
                        angle = index * 360 / spoke_count + spoke_rotation;

                        spoke_rib_segment_2d(
                            spoke_hub_radius + 0.35,
                            angle,
                            spoke_groove_width,
                            spoke_fork_radius - 0.25,
                            angle,
                            spoke_groove_width
                        );
                        for (branch = [-1, 1])
                            spoke_rib_segment_2d(
                                spoke_fork_radius + 0.3,
                                angle,
                                spoke_groove_width,
                                spoke_tip_radius - 0.05,
                                angle + branch * spoke_branch_angle,
                                spoke_groove_width
                            );
                    }
                }
    }
}

module lug_recesses() {
    half_width = wheel_width / 2;

    for (side = [-1, 1])
        for (index = [0 : spoke_count - 1]) {
            angle = index * 360 / spoke_count + spoke_rotation;
            z_start = side > 0 ? half_width - lug_recess_depth : -half_width - 0.01;

            translate([
                lug_circle_radius * cos(angle),
                lug_circle_radius * sin(angle),
                z_start
            ])
                cylinder(d = lug_recess_diameter, h = lug_recess_depth + 0.02, $fn = 32);
        }
}

module double_tire_wheel_v6() {
    difference() {
        union() {
            twin_n_profile_tire_seats();
            outer_retaining_lips();
            shaft_offset_boss();
            // Continuous waist core joins both tire seats and reinforces the web.
            cylinder(d = waist_diameter, h = wheel_width, center = true);
        }

        d_shaft_bore();
        if (!$preview)
            coin_edge_reliefs();
        split_spoke_through_cuts();
        if (!$preview) {
            split_spoke_surface_grooves();
            lug_recesses();
        }
    }
}

if (is_undef($combined_print_layout) || !$combined_print_layout)
    color("red")
        double_tire_wheel_v6();