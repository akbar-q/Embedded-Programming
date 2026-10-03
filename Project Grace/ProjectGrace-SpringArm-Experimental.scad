// Project Grace - experimental compliant-arm pipe climber body.
// Dimensions are in millimetres. ProjectGrace-V5.scad remains preserved.

// Set to "motor_reference" or "servo_reference" to inspect component envelopes.
view_mode = "skeleton";

// IMechE External Pipe Climber brief: commercial 22 mm copper pipe.
// The wheels flare 15 mm into the gap on each side, so a 44 mm hub spacing
// leaves the required 14 mm clear gap between the tyre treads.
pipe_outer_diameter = 22;
target_tread_gap = 14;
wheel_flare_per_side = 15;
target_wheel_hub_spacing = target_tread_gap + 2 * wheel_flare_per_side;

frame_thickness = 6;

// Central body.
center_width = 30;
center_height = 83;
center_wire_opening_width = 20;
center_wire_opening_height = 10;

// Four identical motor arms. Their roots are compliant accordion flexures.
// Increase arm_extension to gain climbing stroke without changing the mount layout.
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

// Paired PETG accordion flexures are built into each arm and carry its radial load.
// The thinner 3 mm section bends in-plane while the centre frame and motor pad remain rigid.
arm_spring_length = 28;
arm_spring_beam_width = 3.5;
arm_spring_thickness = 5;
arm_spring_amplitude = 2.5;
arm_spring_cycles = 2;
arm_spring_pair_spacing = 10;
arm_spring_overlap = 5;
spring_gusset_length = 7;
spring_gusset_root_diameter = 8;
spring_gusset_tip_diameter = 4;

// Built-in zig-zag tensioners join the upper and lower pads at each outer side.
side_spring_beam_width = 3.2;
side_spring_thickness = 5;
side_spring_amplitude = 6;
side_spring_folds = 4;
side_spring_pad_overlap = 5;
side_spring_pad_embed = 3;
include_side_accordion_springs = true;

// Measured N20-style micro metal gearmotor envelope from the supplied photos.
// The total length includes the output shaft; the individual can and gearbox lengths are estimates.
motor_overall_length = 46.05;
motor_width = 15.23;
motor_thickness = 11.82;
motor_can_length = 22;
gearbox_length = 16;
output_shaft_length = motor_overall_length - motor_can_length - gearbox_length;
output_shaft_diameter = 3;

// Integrated holder dimensions. The motor sits directly on the frame surface.
motor_fit_clearance = 1.5;
holder_wall_thickness = 3.5;
holder_wall_height = 12;
holder_back_stop_thickness = 3;
holder_length = motor_can_length + gearbox_length + 2;
holder_lip_length = 14;
holder_lip_depth = 5;
holder_lip_height = 2;
holder_lip_rise = 3.5;
holder_lip_gusset_length = 4;
holder_lip_positions = [5, 22];
wire_exit_slot_width = 4;
wire_exit_height = 8;
wire_exit_corner_offset = motor_width / 2 - wire_exit_slot_width / 2 - 0.5;
cable_tie_slot_width = 2.5;
cable_tie_slot_length = 4;
cable_tie_slot_positions = [10, 30];
cable_tie_slot_y = 10;

// MG90S micro-servo dimensions. Verify the tab-hole spacing on the supplied servo.
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
servo_cradle_wall_thickness = 2;
servo_cradle_wall_height = 10;
servo_cradle_floor_thickness = 2;
servo_spline_diameter = 4.9;
servo_spline_projection = 4;
servo_spline_x_offset = 7;
servo_cradle_center_x = -31;
servo_cradle_mount_z = servo_cradle_floor_thickness;
servo_inner_bracket_hole_diameter = 3.2;
servo_inner_bracket_hole_positions = [-10, 0, 10];

module arm_with_pad(root_x, root_y, angle) {
    rigid_arm_start = arm_spring_length - arm_spring_overlap;
    lightening_slot_start = max(arm_slot_start, rigid_arm_start + 4);
    arm_slot_length = arm_length - end_pad_length - lightening_slot_start - arm_slot_end_clearance;
    pad_start_x = arm_length - end_pad_length;
    holder_outer_width = motor_width + motor_fit_clearance + 2 * holder_wall_thickness;
    translate([root_x, root_y, 0])
        rotate([0, 0, angle])
            difference() {
                union() {
                    // Rigid section begins after the accordion flexure and ends at the motor pad.
                    translate([rigid_arm_start, -arm_width / 2, 0])
                        cube([
                            arm_length - rigid_arm_start,
                            arm_width,
                            frame_thickness
                        ]);

                    // Rigid bridges keep the paired flexures parallel and prevent arm twist.
                    translate([-arm_spring_overlap, -arm_width / 2, 0])
                        cube([arm_spring_overlap + 2, arm_width, frame_thickness]);
                    translate([
                        arm_spring_length - arm_spring_overlap,
                        -arm_width / 2,
                        0
                    ])
                        cube([arm_spring_overlap + 2, arm_width, frame_thickness]);

                    // Motor floor ends flush with the continuous holder walls.
                    translate([pad_start_x, -holder_outer_width / 2, 0])
                        cube([end_pad_length, holder_outer_width, frame_thickness]);

                }

                // Long lightening slot, matching the reference skeleton.
                translate([lightening_slot_start, -arm_slot_width / 2, -1])
                    cube([arm_slot_length, arm_slot_width, frame_thickness + 2]);

            }
}

module triangular_opening(y_position, points_up) {
    translate([0, y_position, -1])
        linear_extrude(height = frame_thickness + 2)
            if (points_up)
                polygon(points = [
                    [-triangle_opening_width / 2, -triangle_opening_height / 2],
                    [triangle_opening_width / 2, -triangle_opening_height / 2],
                    [0, triangle_opening_height / 2]
                ]);
            else
                polygon(points = [
                    [-triangle_opening_width / 2, triangle_opening_height / 2],
                    [triangle_opening_width / 2, triangle_opening_height / 2],
                    [0, -triangle_opening_height / 2]
                ]);
}

module center_body() {
    // Solid, uninterrupted deck for electronics, battery, and cable routing.
    difference() {
        linear_extrude(height = frame_thickness)
            polygon(points = [
                [-center_width / 2, -center_height / 2],
                [center_width / 2, -center_height / 2],
                [center_width / 2, -center_height / 2 + 14],
                [center_width / 2 + 6, -8],
                [center_width / 2 + 6, 8],
                [center_width / 2, center_height / 2 - 14],
                [center_width / 2, center_height / 2],
                [-center_width / 2, center_height / 2],
                [-center_width / 2, center_height / 2 - 14],
                [-center_width / 2 - 6, 8],
                [-center_width / 2 - 6, -8],
                [-center_width / 2, -center_height / 2 + 14]
            ]);

        // Wide central pass-through routes the motor wires under the electronics.
        hull() {
            translate([
                -(center_wire_opening_width - center_wire_opening_height) / 2,
                0,
                -1
            ])
                cylinder(d = center_wire_opening_height, h = frame_thickness + 2, $fn = 32);
            translate([
                (center_wire_opening_width - center_wire_opening_height) / 2,
                0,
                -1
            ])
                cylinder(d = center_wire_opening_height, h = frame_thickness + 2, $fn = 32);
        }
    }
}

module holder_lip_gussets(lip_x, wall_y) {
    // Angled webs brace both ends of each raised stub against side loads.
    hull() {
        translate([
            lip_x - holder_lip_gusset_length,
            wall_y,
            holder_wall_height - 1
        ])
            cube([holder_lip_gusset_length, holder_wall_thickness, 1]);
        translate([lip_x, wall_y, holder_wall_height + holder_lip_rise - 1])
            cube([holder_lip_gusset_length, holder_wall_thickness, 1]);
    }
    hull() {
        translate([
            lip_x + holder_lip_length,
            wall_y,
            holder_wall_height - 1
        ])
            cube([holder_lip_gusset_length, holder_wall_thickness, 1]);
        translate([
            lip_x + holder_lip_length - holder_lip_gusset_length,
            wall_y,
            holder_wall_height + holder_lip_rise - 1
        ])
            cube([holder_lip_gusset_length, holder_wall_thickness, 1]);
    }
}

module motor_holder() {
    holder_outer_width = motor_width + motor_fit_clearance + 2 * holder_wall_thickness;

    union() {
        // Continuous rear stop and side walls make a rigid U-shaped motor cradle.
        difference() {
            translate([0, -holder_outer_width / 2, 0])
                cube([holder_back_stop_thickness, holder_outer_width, holder_wall_height]);
            for (side = [-1, 1])
                translate([
                    -1,
                    side * wire_exit_corner_offset - wire_exit_slot_width / 2,
                    (holder_wall_height - wire_exit_height) / 2
                ])
                    cube([
                        holder_back_stop_thickness + 2,
                        wire_exit_slot_width,
                        wire_exit_height
                    ]);
        }
        translate([0, -holder_outer_width / 2, 0])
            cube([holder_length, holder_wall_thickness, holder_wall_height]);
        translate([0, holder_outer_width / 2 - holder_wall_thickness, 0])
            cube([holder_length, holder_wall_thickness, holder_wall_height]);

        // Raised L-shaped lips retain the motor above its top surface without a full roof.
        for (lip_x = holder_lip_positions) {
            translate([
                lip_x,
                -holder_outer_width / 2,
                holder_wall_height
            ])
                cube([holder_lip_length, holder_wall_thickness, holder_lip_rise]);
            translate([
                lip_x,
                holder_outer_width / 2 - holder_wall_thickness,
                holder_wall_height
            ])
                cube([holder_lip_length, holder_wall_thickness, holder_lip_rise]);
            translate([
                lip_x,
                -holder_outer_width / 2 + holder_wall_thickness,
                holder_wall_height + holder_lip_rise - holder_lip_height
            ])
                cube([holder_lip_length, holder_lip_depth, holder_lip_height]);
            translate([
                lip_x,
                holder_outer_width / 2 - holder_wall_thickness - holder_lip_depth,
                holder_wall_height + holder_lip_rise - holder_lip_height
            ])
                cube([holder_lip_length, holder_lip_depth, holder_lip_height]);
            holder_lip_gussets(lip_x, -holder_outer_width / 2);
            holder_lip_gussets(
                lip_x,
                holder_outer_width / 2 - holder_wall_thickness
            );
        }
    }
}

module motor_holder_tie_slots() {
    // Two tie stations prevent the motor rotating in its holder under drive torque.
    for (tie_x = cable_tie_slot_positions)
        for (side = [-1, 1])
            translate([
                tie_x - cable_tie_slot_length / 2,
                side * cable_tie_slot_y - cable_tie_slot_width / 2,
                -1
            ])
                cube([cable_tie_slot_length, cable_tie_slot_width, frame_thickness + 2]);
}

module holder_on_arm(root_x, root_y, angle) {
    translate([root_x, root_y, frame_thickness])
        rotate([0, 0, angle])
            translate([arm_length - holder_length, 0, 0])
                motor_holder();
}

module holder_slots_on_arm(root_x, root_y, angle) {
    translate([root_x, root_y, 0])
        rotate([0, 0, angle])
            translate([arm_length - holder_length, 0, 0])
                motor_holder_tie_slots();
}

module arm_spring_beam(point_a, point_b) {
    hull() {
        translate([point_a[0], point_a[1], 0])
            cylinder(d = arm_spring_beam_width, h = arm_spring_thickness, $fn = 24);
        translate([point_b[0], point_b[1], 0])
            cylinder(d = arm_spring_beam_width, h = arm_spring_thickness, $fn = 24);
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
    // Flush tapered flare blends each flexure into its rigid attachment.
    hull() {
        translate([x_position, y_offset, 0])
            cylinder(d = spring_gusset_root_diameter, h = frame_thickness, $fn = 32);
        translate([
            x_position + direction * spring_gusset_length,
            y_offset,
            0
        ])
            cylinder(d = spring_gusset_tip_diameter, h = frame_thickness, $fn = 32);
    }
}

module accordion_arm_root(root_x, root_y, angle) {
    translate([root_x, root_y, 0])
        rotate([0, 0, angle]) {
            for (side = [-1, 1]) {
                y_offset = side * arm_spring_pair_spacing / 2;
                spring_mount_gusset(-arm_spring_overlap, y_offset, 1);
                spring_mount_gusset(
                    arm_spring_length + arm_spring_overlap,
                    y_offset,
                    -1
                );
                arm_accordion_beam(side * arm_spring_pair_spacing / 2);
            }
        }
}

module side_spring_beam(point_a, point_b) {
    hull() {
        translate([point_a[0], point_a[1], 0])
            cylinder(d = side_spring_beam_width, h = side_spring_thickness, $fn = 24);
        translate([point_b[0], point_b[1], 0])
            cylinder(d = side_spring_beam_width, h = side_spring_thickness, $fn = 24);
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

    // Continuous diagonal turns give the tensioner the coil-like flex shown in the sketch.
    for (index = [0 : len(points) - 2])
        side_spring_beam(points[index], points[index + 1]);
}

module servo_cradle() {
    cradle_inner_length = servo_body_length + servo_fit_clearance;
    cradle_inner_width = servo_body_width + servo_fit_clearance;
    cradle_outer_length = cradle_inner_length + 2 * servo_cradle_wall_thickness;
    cradle_outer_width = cradle_inner_width + 2 * servo_cradle_wall_thickness;
    ear_hole_x = servo_mount_hole_spacing / 2;

    // Side-mounted under-frame tray: the servo spline and ChainHookHorn face downward.
    difference() {
        union() {
            // The wide plate carries the servo's two mounting ears.
            translate([-servo_ear_length / 2, -servo_ear_width / 2, -servo_cradle_floor_thickness])
                cube([servo_ear_length, servo_ear_width, servo_cradle_floor_thickness]);

            // Low side rails locate the body in the cradle without covering its output end.
            translate([
                -cradle_outer_length / 2,
                -cradle_outer_width / 2,
                -servo_cradle_wall_height
            ])
                cube([
                    cradle_outer_length,
                    servo_cradle_wall_thickness,
                    servo_cradle_wall_height
                ]);
            translate([
                -cradle_outer_length / 2,
                cradle_outer_width / 2 - servo_cradle_wall_thickness,
                -servo_cradle_wall_height
            ])
                cube([
                    cradle_outer_length,
                    servo_cradle_wall_thickness,
                    servo_cradle_wall_height
                ]);

            // Outer end stop keeps the servo located against drive vibration.
            translate([
                -cradle_outer_length / 2,
                -cradle_outer_width / 2,
                -servo_cradle_wall_height
            ])
                cube([
                    servo_cradle_wall_thickness,
                    cradle_outer_width,
                    servo_cradle_wall_height
                ]);
        }

        // Opening under the spline lets the servo horn and chain hook run freely.
        translate([servo_spline_x_offset, 0, -servo_cradle_floor_thickness - 1])
            cylinder(d = servo_spline_diameter + 2, h = servo_cradle_floor_thickness + 2, $fn = 48);

        // M2 through-holes align with the MG90S mounting ears.
        for (x_position = [-ear_hole_x, ear_hole_x])
            translate([x_position, 0, -servo_cradle_floor_thickness - 1])
                cylinder(d = servo_mount_hole_diameter, h = servo_cradle_floor_thickness + 2, $fn = 32);

        // Three clearance holes sit in the inner bracket where shown in the concept sketch.
        for (x_position = servo_inner_bracket_hole_positions)
            translate([x_position, 0, -servo_cradle_floor_thickness - 1])
                cylinder(
                    d = servo_inner_bracket_hole_diameter,
                    h = servo_cradle_floor_thickness + 2,
                    $fn = 32
                );
    }
}

module printed_skeleton(include_servo_cradle = true) {
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
                // The floor overlaps the chassis by 2 mm, making the side cradle one printed part.
                translate([servo_cradle_center_x, 0, servo_cradle_mount_z])
                    servo_cradle();
        }

    }
}

module compound_motor_reference() {
    // Motor can, gearbox, and output shaft are separate volumes for mount design.
    color("silver")
        translate([0, -motor_width / 2, -motor_thickness / 2])
            cube([motor_can_length, motor_width, motor_thickness]);

    color("goldenrod")
        translate([motor_can_length, -motor_width / 2, -motor_thickness / 2])
            cube([gearbox_length, motor_width, motor_thickness]);

    color("dimgray")
        translate([motor_can_length + gearbox_length, 0, 0])
            rotate([0, 90, 0])
                cylinder(d = output_shaft_diameter, h = output_shaft_length);
}

module mg90s_reference() {
    // Approximate orientation inside the side-mounted underside cradle.
    color("royalblue")
        translate([
            servo_cradle_center_x - servo_body_length / 2,
            -servo_body_width / 2,
            servo_cradle_mount_z - servo_body_height
        ])
            cube([servo_body_length, servo_body_width, servo_body_height]);

    color("dimgray")
        translate([
            servo_cradle_center_x + servo_spline_x_offset,
            0,
            servo_cradle_mount_z - servo_body_height - servo_spline_projection
        ])
            cylinder(d = servo_spline_diameter, h = servo_spline_projection, $fn = 48);
}

if (view_mode == "motor_reference")
    compound_motor_reference();
else if (view_mode == "servo_reference") {
    color("orange")
        printed_skeleton();
    mg90s_reference();
} else
    color("orange")
        printed_skeleton();
