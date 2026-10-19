// Project Grace - V7 chassis with inboard Velcro ties and servo cradle.
// Branched from V5; dimensions are in millimetres.

view_mode = "skeleton";

// Pipe and wheel contact geometry.
pipe_outer_diameter = 22;
target_tread_gap = 13.2;
wheel_flare_per_side = 15;
target_wheel_hub_spacing = target_tread_gap + 2 * wheel_flare_per_side;

frame_thickness = 8;

// Centre electronics plate.
center_width = 30;
center_height = 108;
center_extension_each_end = 15;
center_extension_half_width = 4;
center_wire_opening_width = 20;
center_wire_opening_height = 10;
servo_cradle_center_y = -15;

// Arm and motor-pad layout.
base_arm_length = 92;
arm_extension = 0;
arm_length = base_arm_length + arm_extension;
arm_width = 20;
arm_slot_width = 5;
arm_slot_start = 13;
arm_slot_end_clearance = 7;
end_pad_length = 32;
end_pad_width = 36;
arm_root_x = center_width / 2 - 3;
arm_root_y = center_height / 2 - 17;
arm_angle = acos((target_wheel_hub_spacing / 2 - arm_root_x) / arm_length);

// Paired accordion flexures. The wider pair spacing resists arm twist while
// the two parallel beams remain compliant along the intended arm travel axis.
arm_spring_length = 28;
arm_spring_beam_width = 3.5;
arm_spring_thickness = 5;
arm_spring_amplitude = 2.5;
arm_spring_cycles = 2;
arm_spring_pair_spacing = 16;
arm_spring_overlap = 8;
spring_gusset_length = 10;
spring_gusset_root_diameter = 12;
spring_gusset_tip_diameter = 6;
arm_stiffener_width = 3.5;
arm_stiffener_height = 3.5;
arm_stiffener_offset = 6.5;
arm_stiffener_ramp_length = 6;

// Outer zig-zag tensioners.
side_spring_beam_width = 4;
side_spring_thickness = 6;
side_spring_amplitude = 6;
side_spring_folds = 4;
side_spring_pad_overlap = 1.5;
side_spring_pad_embed = 3;
include_side_accordion_springs = true;

// N20 motor envelope.
motor_overall_length = 45.7;
motor_width = 15.23;
motor_thickness = 11.82;
motor_can_length = 22;
gearbox_length = 16;
gearbox_width = 12;
output_shaft_length = motor_overall_length - motor_can_length - gearbox_length;
output_shaft_diameter = 3;
motor_floor_ridge_height = 0.4;
motor_floor_ridge_width = 0.8;
motor_floor_ridge_pitch = 3;
motor_floor_ridge_end_clearance = 1.5;

// Open-topped motor holders with loose-fit side clearance.
motor_fit_clearance = 1.5;
holder_wall_thickness = 3.5;
holder_wall_height = motor_thickness
    + motor_floor_ridge_height
    + 1.8;
holder_back_stop_thickness = 3;
holder_front_stop_thickness = 3;
gearbox_support_clearance = 1.0;
gearbox_support_wall_thickness = 2.5;
gearbox_support_wall_height = holder_wall_height / 2;
holder_inner_length = 47.7;
gearbox_support_length = gearbox_length;
holder_length = holder_inner_length + holder_back_stop_thickness + holder_front_stop_thickness;

// Raised tabs above the side walls carry the Velcro tie point over the motor.
velcro_tie_slot_length = 11.5;
velcro_tie_slot_height = 3.2;
velcro_tie_tab_end_margin = 2;
velcro_tie_slot_bottom_clearance = 1.5;
velcro_tie_tab_top_margin = 2;
velcro_tie_tab_height = velcro_tie_slot_bottom_clearance
    + velcro_tie_slot_height
    + velcro_tie_tab_top_margin;
wire_exit_slot_width = 4;
wire_exit_height = 8;
wire_exit_corner_offset = motor_width / 2 - wire_exit_slot_width / 2 - 0.5;
cable_tie_slot_width = 2.5;
cable_tie_slot_length = 4;
cable_tie_slot_positions = [10, 30];
cable_tie_slot_y = 10;

// MG90S servo cradle.
servo_body_length = 22.8;
servo_body_width = 12.2;
servo_body_height = 28.5;
servo_fit_clearance = 0.5;
servo_ear_length = 32.4;
servo_ear_width = 12.2;
servo_ear_thickness = 2.5;
servo_ear_height_from_spline = 19;
servo_mount_hole_diameter = 2.2;
servo_mount_hole_spacing = 27.4;
servo_cradle_wall_thickness = 4;
servo_cradle_wall_height = 24;
servo_cradle_floor_thickness = 3;
servo_cradle_gusset_height = 8;
servo_cradle_gusset_run = 8;
servo_cradle_gusset_thickness = 3;
servo_cradle_gusset_overlap = 0.5;
servo_spline_diameter = 4.9;
servo_spline_projection = 4;
servo_spline_x_offset = 7;
servo_cradle_center_x = 0;
servo_cradle_mount_z = servo_cradle_floor_thickness + 2;
servo_inner_bracket_hole_diameter = 3.2;
servo_inner_bracket_hole_positions = [-10, 0, 10];
servo_cradle_outer_width = servo_body_width + servo_fit_clearance
    + 2 * servo_cradle_wall_thickness;
star_mark_radius = 7;
star_mark_line_width = 0.8;
star_mark_depth = 0.6;
star_mark_y = center_height / 2 - 10;

module arm_with_pad(root_x, root_y, angle) {
    rigid_arm_start = arm_spring_length - arm_spring_overlap;
    lightening_slot_start = max(arm_slot_start, rigid_arm_start + 4);
    arm_slot_length = arm_length - end_pad_length - lightening_slot_start - arm_slot_end_clearance;
    pad_start_x = arm_length - end_pad_length;
    holder_start_x = arm_length - holder_length;
    holder_outer_width = motor_width + motor_fit_clearance + 2 * holder_wall_thickness;

    translate([root_x, root_y, 0])
        rotate([0, 0, angle])
            difference() {
                union() {
                    translate([rigid_arm_start, -arm_width / 2, 0])
                        cube([arm_length - rigid_arm_start, arm_width, frame_thickness]);

                    // Rigid cross-bridges join both flexure beams at their ends.
                    translate([-arm_spring_overlap, -arm_width / 2, 0])
                        cube([arm_spring_overlap + 2, arm_width, frame_thickness]);
                    translate([
                        arm_spring_length - arm_spring_overlap,
                        -arm_width / 2,
                        0
                    ])
                        cube([arm_spring_overlap + 2, arm_width, frame_thickness]);

                    // Raised twin rails stiffen the arm and stop at the holder entrance.
                    for (side = [-1, 1]) {
                        stiffener_y = side * arm_stiffener_offset - arm_stiffener_width / 2;
                        hull() {
                            translate([
                                rigid_arm_start + 1,
                                stiffener_y,
                                frame_thickness - 0.2
                            ])
                                cube([1, arm_stiffener_width, 1]);
                            translate([
                                rigid_arm_start + arm_stiffener_ramp_length,
                                stiffener_y,
                                frame_thickness - 0.2
                            ])
                                cube([1, arm_stiffener_width, arm_stiffener_height + 0.2]);
                        }
                        translate([
                            rigid_arm_start + arm_stiffener_ramp_length,
                            stiffener_y,
                            frame_thickness - 0.2
                        ])
                            cube([
                                holder_start_x - rigid_arm_start - arm_stiffener_ramp_length,
                                arm_stiffener_width,
                                arm_stiffener_height + 0.2
                            ]);
                    }

                    // Motor floor is flush with the outside of the holder walls.
                    translate([pad_start_x, -holder_outer_width / 2, 0])
                        cube([end_pad_length, holder_outer_width, frame_thickness]);

                    // Low transverse ribs key adhesive while keeping the motor channel clear.
                    for (ridge_x = [
                        pad_start_x + motor_floor_ridge_end_clearance
                        : motor_floor_ridge_pitch
                        : arm_length - motor_floor_ridge_end_clearance - motor_floor_ridge_width
                    ])
                        translate([ridge_x, -holder_outer_width / 2, frame_thickness])
                            cube([
                                motor_floor_ridge_width,
                                holder_outer_width,
                                motor_floor_ridge_height
                            ]);
                }

                translate([lightening_slot_start, -arm_slot_width / 2, -1])
                    cube([arm_slot_length, arm_slot_width, frame_thickness + 2]);
            }
}

module center_body() {
    core_half_height = center_height / 2 - center_extension_each_end;
    side_shoulder_y = core_half_height - 14;

    difference() {
        linear_extrude(height = frame_thickness)
            polygon(points = [
                [-center_extension_half_width, center_height / 2],
                [center_extension_half_width, center_height / 2],
                [center_width / 2, core_half_height],
                [center_width / 2, side_shoulder_y],
                [center_width / 2 + 6, 8],
                [center_width / 2 + 6, -8],
                [center_width / 2, -side_shoulder_y],
                [center_width / 2, -core_half_height],
                [center_extension_half_width, -center_height / 2],
                [-center_extension_half_width, -center_height / 2],
                [-center_width / 2, -core_half_height],
                [-center_width / 2, -side_shoulder_y],
                [-center_width / 2 - 6, -8],
                [-center_width / 2 - 6, 8],
                [-center_width / 2, side_shoulder_y],
                [-center_width / 2, core_half_height]
            ]);

        hull() {
            translate([-(center_wire_opening_width - center_wire_opening_height) / 2, 0, -1])
                cylinder(d = center_wire_opening_height, h = frame_thickness + 2, $fn = 32);
            translate([(center_wire_opening_width - center_wire_opening_height) / 2, 0, -1])
                cylinder(d = center_wire_opening_height, h = frame_thickness + 2, $fn = 32);
        }

    }
}

module center_plate_engraving() {
    for (label = [
        ["Project", center_wire_opening_height / 2 + 6],
        ["Grace", -(center_wire_opening_height / 2 + 4)]
    ])
        translate([0, label[1], frame_thickness - 0.6])
            linear_extrude(height = 0.7)
                text(
                    label[0],
                    size = 6.5,
                    font = "Bahnschrift:style=SemiBold",
                    halign = "center",
                    valign = "center"
                );

    translate([0, star_mark_y, frame_thickness - star_mark_depth])
        linear_extrude(height = star_mark_depth + 0.1)
            inverted_pentagram_engraving();

    translate([0, -center_height / 2 + 10, frame_thickness - 0.6])
        linear_extrude(height = 0.7)
            text(
                "AQ",
                size = 5,
                font = "Bahnschrift:style=SemiBold",
                halign = "center",
                valign = "center"
            );
}

function inverted_pentagram_point(index) = [
    star_mark_radius * cos(-90 + 72 * index),
    star_mark_radius * sin(-90 + 72 * index)
];

module inverted_pentagram_engraving() {
    path_order = [0, 2, 4, 1, 3, 0];

    union() {
        difference() {
            circle(r = star_mark_radius, $fn = 64);
            circle(r = star_mark_radius - star_mark_line_width, $fn = 64);
        }

        for (edge = [0 : 4])
            hull() {
                translate(inverted_pentagram_point(path_order[edge]))
                    circle(d = star_mark_line_width, $fn = 16);
                translate(inverted_pentagram_point(path_order[edge + 1]))
                    circle(d = star_mark_line_width, $fn = 16);
            }
    }
}

module motor_holder() {
    holder_outer_width = motor_width + motor_fit_clearance + 2 * holder_wall_thickness;
    velcro_tie_slot_start_x = holder_back_stop_thickness + velcro_tie_tab_end_margin;
    velcro_tie_tab_start_x = velcro_tie_slot_start_x - velcro_tie_tab_end_margin;
    velcro_tie_tab_length = velcro_tie_slot_length + 2 * velcro_tie_tab_end_margin;
    velcro_tie_slot_z = holder_wall_height + velcro_tie_slot_bottom_clearance;

    union() {
        difference() {
            translate([0, -holder_outer_width / 2, 0])
                cube([holder_back_stop_thickness, holder_outer_width, holder_wall_height]);
            for (side = [-1, 1])
                translate([
                    -1,
                    side * wire_exit_corner_offset - wire_exit_slot_width / 2,
                    (holder_wall_height - wire_exit_height) / 2
                ])
                    cube([holder_back_stop_thickness + 2, wire_exit_slot_width, wire_exit_height]);
        }

        // Full-length floor ties the can and gearbox cradle together.
        translate([0, -holder_outer_width / 2, -frame_thickness])
            cube([holder_length, holder_outer_width, frame_thickness]);

        for (side = [-1, 1])
            difference() {
                union() {
                    side_wall_y = side < 0
                        ? -holder_outer_width / 2
                        : holder_outer_width / 2 - holder_wall_thickness;

                    translate([0, side_wall_y, 0])
                        cube([holder_length, holder_wall_thickness, holder_wall_height]);
                    translate([
                        velcro_tie_tab_start_x,
                        side_wall_y,
                        holder_wall_height
                    ])
                        cube([
                            velcro_tie_tab_length,
                            holder_wall_thickness,
                            velcro_tie_tab_height
                        ]);
                }

                translate([
                    velcro_tie_slot_start_x,
                    -holder_outer_width / 2 - 1,
                    velcro_tie_slot_z
                ])
                    cube([
                        velcro_tie_slot_length,
                        holder_outer_width + 2,
                        velcro_tie_slot_height
                    ]);
            }

        front_stop_x = holder_length - holder_front_stop_thickness;
        translate([front_stop_x, -holder_outer_width / 2, 0])
            cube([
                holder_front_stop_thickness,
                holder_outer_width,
                holder_wall_height / 2
            ]);

        gearbox_support_center_y = (
            motor_width
            + gearbox_support_clearance
            + gearbox_support_wall_thickness
        ) / 2;
        for (side = [-1, 1])
            translate([
                holder_back_stop_thickness + motor_can_length,
                side * gearbox_support_center_y - gearbox_support_wall_thickness / 2,
                0
            ])
                cube([
                    gearbox_support_length,
                    gearbox_support_wall_thickness,
                    gearbox_support_wall_height
                ]);

    }
}

module holder_on_arm(root_x, root_y, angle) {
    translate([root_x, root_y, frame_thickness])
        rotate([0, 0, angle])
            translate([arm_length - holder_length, 0, 0])
                motor_holder();
}

module arm_spring_beam(point_a, point_b) {
    hull() {
        translate([point_a[0], point_a[1], 0])
            cylinder(d = arm_spring_beam_width, h = arm_spring_thickness, $fn = 48);
        translate([point_b[0], point_b[1], 0])
            cylinder(d = arm_spring_beam_width, h = arm_spring_thickness, $fn = 48);
    }
}

module arm_accordion_beam(y_offset) {
    spring_pitch = arm_spring_length / (2 * arm_spring_cycles);
    points = concat(
        [[-arm_spring_overlap, y_offset], [0, y_offset], [0, y_offset + arm_spring_amplitude]],
        [for (index = [1 : 2 * arm_spring_cycles])
            [
                index * spring_pitch,
                y_offset + (index % 2 == 1 ? arm_spring_amplitude : -arm_spring_amplitude)
            ]
        ],
        [[arm_spring_length, y_offset], [arm_spring_length + arm_spring_overlap, y_offset]]
    );

    for (index = [0 : len(points) - 2])
        arm_spring_beam(points[index], points[index + 1]);
}

module spring_mount_gusset(x_position, y_offset, direction) {
    hull() {
        translate([x_position, y_offset, 0])
            cylinder(d = spring_gusset_root_diameter, h = frame_thickness, $fn = 32);
        translate([x_position + direction * spring_gusset_length, y_offset, 0])
            cylinder(d = spring_gusset_tip_diameter, h = frame_thickness, $fn = 32);
    }
}

module accordion_arm_root(root_x, root_y, angle) {
    translate([root_x, root_y, 0])
        rotate([0, 0, angle])
            for (side = [-1, 1]) {
                y_offset = side * arm_spring_pair_spacing / 2;
                spring_mount_gusset(-arm_spring_overlap, y_offset, 1);
                spring_mount_gusset(arm_spring_length + arm_spring_overlap, y_offset, -1);
                arm_accordion_beam(y_offset);
            }
}

module side_spring_beam(point_a, point_b) {
    hull() {
        translate([point_a[0], point_a[1], 0])
            cylinder(d = side_spring_beam_width, h = side_spring_thickness, $fn = 48);
        translate([point_b[0], point_b[1], 0])
            cylinder(d = side_spring_beam_width, h = side_spring_thickness, $fn = 48);
    }
}

module side_accordion_spring(side) {
    wheel_hub_y = arm_root_y + arm_length * sin(arm_angle);
    attach_y = wheel_hub_y - side_spring_pad_overlap;
    outer_pad_x = target_wheel_hub_spacing / 2
        + end_pad_width / 2 * sin(arm_angle)
        - side_spring_pad_embed;
    spring_outer_x = side * outer_pad_x;
    spring_inner_x = side * (outer_pad_x - side_spring_amplitude);
    point_count = 2 * side_spring_folds + 1;
    spring_pitch = 2 * attach_y / (point_count + 1);
    points = concat(
        [[spring_inner_x, -attach_y]],
        [for (index = [1 : point_count])
            [
                index % 2 == 1 ? spring_outer_x : spring_inner_x,
                -attach_y + index * spring_pitch
            ]
        ],
        [[spring_inner_x, attach_y]]
    );

    for (index = [0 : len(points) - 2])
        side_spring_beam(points[index], points[index + 1]);
}

module servo_cradle_support_gusset(x_position, side, cradle_outer_width) {
    wall_y = side * (cradle_outer_width / 2 - servo_cradle_gusset_overlap);
    top_z = -servo_cradle_floor_thickness + servo_cradle_gusset_overlap;

    translate([x_position, 0, 0])
        rotate([90, 0, 90])
            linear_extrude(height = servo_cradle_gusset_thickness, center = true)
                polygon(points = [
                    [wall_y, top_z],
                    [wall_y + side * servo_cradle_gusset_run, top_z],
                    [wall_y, top_z - servo_cradle_gusset_height]
                ]);
}

module servo_cradle() {
    cradle_inner_length = servo_body_length + servo_fit_clearance;
    cradle_inner_width = servo_body_width + servo_fit_clearance;
    cradle_outer_length = cradle_inner_length + 2 * servo_cradle_wall_thickness;
    cradle_outer_width = cradle_inner_width + 2 * servo_cradle_wall_thickness;
    ear_hole_x = servo_mount_hole_spacing / 2;

    difference() {
        union() {
            translate([-servo_ear_length / 2, -servo_ear_width / 2, -servo_cradle_floor_thickness])
                cube([servo_ear_length, servo_ear_width, servo_cradle_floor_thickness]);
            translate([
                -cradle_outer_length / 2,
                -cradle_outer_width / 2,
                -servo_cradle_wall_height
            ])
                cube([cradle_outer_length, servo_cradle_wall_thickness, servo_cradle_wall_height]);
            translate([
                -cradle_outer_length / 2,
                cradle_outer_width / 2 - servo_cradle_wall_thickness,
                -servo_cradle_wall_height
            ])
                cube([cradle_outer_length, servo_cradle_wall_thickness, servo_cradle_wall_height]);
            translate([
                -cradle_outer_length / 2,
                -cradle_outer_width / 2,
                -servo_cradle_wall_height
            ])
                cube([servo_cradle_wall_thickness, cradle_outer_width, servo_cradle_wall_height]);

            for (side = [-1, 1])
                for (x_position = [
                    -cradle_outer_length / 2 + servo_cradle_gusset_thickness,
                    cradle_outer_length / 2 - servo_cradle_gusset_thickness
                ])
                    servo_cradle_support_gusset(x_position, side, cradle_outer_width);
        }

        translate([servo_spline_x_offset, 0, -servo_cradle_floor_thickness - 1])
            cylinder(d = servo_spline_diameter + 2, h = servo_cradle_floor_thickness + 2, $fn = 48);
        for (x_position = [-ear_hole_x, ear_hole_x])
            translate([x_position, 0, -servo_cradle_floor_thickness - 1])
                cylinder(d = servo_mount_hole_diameter, h = servo_cradle_floor_thickness + 2, $fn = 32);
        for (x_position = servo_inner_bracket_hole_positions)
            translate([x_position, 0, -servo_cradle_floor_thickness - 1])
                cylinder(d = servo_inner_bracket_hole_diameter, h = servo_cradle_floor_thickness + 2, $fn = 32);
    }
}

module printed_skeleton(include_servo_cradle = false) {
    difference() {
        union() {
            center_body();
            arm_with_pad(arm_root_x, arm_root_y, arm_angle);
            arm_with_pad(-arm_root_x, arm_root_y, 180 - arm_angle);
            arm_with_pad(arm_root_x, -arm_root_y, -arm_angle);
            arm_with_pad(-arm_root_x, -arm_root_y, arm_angle - 180);
            holder_on_arm(arm_root_x, arm_root_y, arm_angle);
            holder_on_arm(-arm_root_x, arm_root_y, 180 - arm_angle);
            holder_on_arm(arm_root_x, -arm_root_y, -arm_angle);
            holder_on_arm(-arm_root_x, -arm_root_y, arm_angle - 180);
            accordion_arm_root(arm_root_x, arm_root_y, arm_angle);
            accordion_arm_root(-arm_root_x, arm_root_y, 180 - arm_angle);
            accordion_arm_root(arm_root_x, -arm_root_y, -arm_angle);
            accordion_arm_root(-arm_root_x, -arm_root_y, arm_angle - 180);
            if (include_side_accordion_springs) {
                side_accordion_spring(-1);
                side_accordion_spring(1);
            }
            if (include_servo_cradle)
                translate([servo_cradle_center_x, servo_cradle_center_y, servo_cradle_mount_z])
                    servo_cradle();
        }

        // Cut markings last so structural unions cannot cover or refill them.
        center_plate_engraving();
    }
}

module compound_motor_reference() {
    color("silver")
        translate([0, -motor_width / 2, -motor_thickness / 2])
            cube([motor_can_length, motor_width, motor_thickness]);
    color("goldenrod")
        translate([motor_can_length, -gearbox_width / 2, -motor_thickness / 2])
            cube([gearbox_length, gearbox_width, motor_thickness]);
    color("dimgray")
        translate([motor_can_length + gearbox_length, 0, 0])
            rotate([0, 90, 0])
                cylinder(d = output_shaft_diameter, h = output_shaft_length);
}

module mg90s_reference() {
    color("royalblue")
        translate([
            servo_cradle_center_x - servo_body_length / 2,
            servo_cradle_center_y - servo_body_width / 2,
            servo_cradle_mount_z - servo_body_height
        ])
            cube([servo_body_length, servo_body_width, servo_body_height]);
    color("dimgray")
        translate([
            servo_cradle_center_x + servo_spline_x_offset,
            servo_cradle_center_y,
            servo_cradle_mount_z - servo_body_height - servo_spline_projection
        ])
            cylinder(d = servo_spline_diameter, h = servo_spline_projection, $fn = 48);
}

if (is_undef($combined_print_layout) || !$combined_print_layout) {
    if (view_mode == "motor_reference")
        compound_motor_reference();
    else if (view_mode == "servo_reference") {
        color("orange")
            printed_skeleton(include_servo_cradle = false);
        mg90s_reference();
    } else
        color("orange")
            printed_skeleton(include_servo_cradle = true);
}
