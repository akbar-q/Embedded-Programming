// Project Grace - experimental compliant-arm pipe climber body.
// Dimensions are in millimetres. ProjectGrace-V5.scad remains preserved.

// Set to "motor_reference" or "servo_reference" to inspect component envelopes.
view_mode = "skeleton";

// IMechE External Pipe Climber brief: commercial 22 mm copper pipe.
// A 4 mm undersize wheel gap preloads the tyres against the pipe for inward grip.
pipe_outer_diameter = 22;
wheel_gap_undersize = 4;
target_wheel_gap = pipe_outer_diameter - wheel_gap_undersize;

frame_thickness = 5;

// Central body.
center_width = 30;
center_height = 58;
center_opening_size = 14;
triangle_opening_width = 11;
triangle_opening_height = 10;

// Four identical motor arms. Their roots are compliant accordion flexures.
// Increase arm_extension to gain climbing stroke without changing the mount layout.
base_arm_length = 92;
arm_extension = 0;
arm_length = base_arm_length + arm_extension;
arm_width = 16;
arm_slot_width = 7;
arm_slot_start = 13;
arm_slot_end_clearance = 7;
end_pad_length = 32;
end_pad_width = 36;
arm_angle = 72;

// Paired PETG accordion flexures carry each arm's radial load.
// The thinner 3 mm section bends in-plane while the centre frame and motor pad remain rigid.
arm_spring_length = 36;
arm_spring_beam_width = 2.2;
arm_spring_thickness = 3;
arm_spring_amplitude = 3.5;
arm_spring_cycles = 3;
arm_spring_pair_spacing = 8;
arm_spring_overlap = 3;

// Forward ToF boom. Angle 0 points between the two upper arms along the pipe.
tof_boom_length = 110;
tof_boom_width = 14;
tof_boom_rail_width = 3;
tof_boom_brace_length = 10;
tof_boom_overlap = 3;
tof_boom_angle = 0;
tof_face_width = 18;
tof_face_height = 12;
tof_face_thickness = 2.5;

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
motor_fit_clearance = 0.6;
holder_wall_thickness = 1.5;
holder_wall_height = 9;
holder_back_stop_thickness = 2;
holder_length = motor_can_length + gearbox_length + 2;
wire_exit_width = 7;
wire_exit_height = 6;
cable_tie_slot_width = 2.5;
cable_tie_slot_length = 4;
cable_tie_slot_positions = [10, 30];
cable_tie_slot_y = 10;
motor_pad_rail_width = 4;
motor_pad_brace_length = 5;
motor_mount_bottom_slot_width = 6;
motor_mount_bottom_slot_start_clearance = 4;
motor_mount_bottom_slot_end_clearance = 4;
holder_rail_segment_length = 7;
holder_rail_segment_positions = [1, 17, 32];
holder_back_stop_tab_width = 4;

// Posts accept the separate tension-band eyelets. Locking caps are fitted after assembly.
spring_anchor_post_diameter = 3;
spring_anchor_post_height = 5;
spring_anchor_base_diameter = 8;
spring_anchor_position_x = 20;
spring_anchor_lug_length = 10;
spring_anchor_lug_extension = 6;

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
    motor_mount_slot_start_x = arm_length - holder_length + motor_mount_bottom_slot_start_clearance;
    motor_mount_slot_length = holder_length
        - motor_mount_bottom_slot_start_clearance
        - motor_mount_bottom_slot_end_clearance;
    motor_pad_rail_center = (
        motor_width + motor_fit_clearance + holder_wall_thickness
    ) / 2;
    spring_anchor_x = pad_start_x + spring_anchor_position_x;

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

                    // Two rails carry the motor holder; braces remain only where load is applied.
                    for (side = [-1, 1])
                        translate([
                            pad_start_x,
                            side * motor_pad_rail_center - motor_pad_rail_width / 2,
                            0
                        ])
                            cube([end_pad_length, motor_pad_rail_width, frame_thickness]);

                    translate([pad_start_x, -end_pad_width / 2, 0])
                        cube([motor_pad_brace_length, end_pad_width, frame_thickness]);
                    translate([
                        arm_length - motor_pad_brace_length,
                        -end_pad_width / 2,
                        0
                    ])
                        cube([motor_pad_brace_length, end_pad_width, frame_thickness]);

                    // Tie stations remain fully supported while the rest of the pad is open.
                    for (tie_x = cable_tie_slot_positions)
                        translate([
                            pad_start_x + end_pad_length - holder_length + tie_x
                                - motor_pad_brace_length / 2,
                            -end_pad_width / 2,
                            0
                        ])
                            cube([
                                motor_pad_brace_length,
                                end_pad_width,
                                frame_thickness
                            ]);

                    // Matched outboard lugs keep both spring anchors clear of the motor cradle.
                    for (side = [-1, 1]) {
                        spring_anchor_y = side * (
                            end_pad_width / 2 + spring_anchor_lug_extension / 2
                        );
                        translate([
                            spring_anchor_x - spring_anchor_lug_length / 2,
                            side < 0
                                ? -end_pad_width / 2 - spring_anchor_lug_extension
                                : end_pad_width / 2 - 2,
                            0
                        ])
                            cube([
                                spring_anchor_lug_length,
                                spring_anchor_lug_extension + 2,
                                frame_thickness
                            ]);
                        translate([spring_anchor_x, spring_anchor_y, 0])
                            cylinder(
                                d = spring_anchor_base_diameter,
                                h = frame_thickness,
                                $fn = 32
                            );
                        translate([
                            spring_anchor_x,
                            spring_anchor_y,
                            frame_thickness - 1
                        ])
                            cylinder(
                                d = spring_anchor_post_diameter,
                                h = spring_anchor_post_height + 1,
                                $fn = 32
                            );
                    }

                }

                // Long lightening slot, matching the reference skeleton.
                translate([lightening_slot_start, -arm_slot_width / 2, -1])
                    cube([arm_slot_length, arm_slot_width, frame_thickness + 2]);

                // Open the underside between the motor support rails to reduce mount mass.
                translate([
                    motor_mount_slot_start_x,
                    -motor_mount_bottom_slot_width / 2,
                    -1
                ])
                    cube([
                        motor_mount_slot_length,
                        motor_mount_bottom_slot_width,
                        frame_thickness + 2
                    ]);
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

        translate([-center_opening_size / 2, -center_opening_size / 2, -1])
            cube([center_opening_size, center_opening_size, frame_thickness + 2]);
        triangular_opening(center_opening_size / 2 + triangle_opening_height, true);
        triangular_opening(-center_opening_size / 2 - triangle_opening_height, false);
    }
}

module tof_sensor_boom() {
    boom_start_y = center_height / 2 - tof_boom_overlap;
    boom_end_y = boom_start_y + tof_boom_length;
    face_z = frame_thickness - 2;

    rotate([0, 0, tof_boom_angle])
        union() {
            // Two rails provide the reach while removing most of the boom weight.
            for (side = [-1, 1])
                translate([
                    side * (tof_boom_width / 2 - tof_boom_rail_width),
                    boom_start_y,
                    0
                ])
                    cube([tof_boom_rail_width, tof_boom_length, frame_thickness]);

            // Root and tip braces prevent the rails twisting under acceleration.
            translate([-tof_boom_width / 2, boom_start_y, 0])
                cube([tof_boom_width, tof_boom_brace_length, frame_thickness]);
            translate([-tof_boom_width / 2, boom_end_y - tof_boom_brace_length, 0])
                cube([tof_boom_width, tof_boom_brace_length, frame_thickness]);

            // Compact flat pad for gluing the small ToF sensor board in place.
            translate([-tof_face_width / 2, boom_end_y - tof_face_thickness, face_z])
                cube([tof_face_width, tof_face_thickness, tof_face_height + 2]);
        }
}

module motor_holder() {
    holder_outer_width = motor_width + motor_fit_clearance + 2 * holder_wall_thickness;

    union() {
        // Two rear tabs prevent inward movement while leaving a clear wire exit.
        for (tab_y = [
            -holder_outer_width / 2,
            holder_outer_width / 2 - holder_back_stop_tab_width
        ])
            translate([0, tab_y, 0])
                cube([
                    holder_back_stop_thickness,
                    holder_back_stop_tab_width,
                    holder_wall_height
                ]);

        // Short guide segments locate the motor without the mass of continuous walls.
        for (rail_x = holder_rail_segment_positions)
            for (rail_y = [
                -holder_outer_width / 2,
                holder_outer_width / 2 - holder_wall_thickness
            ])
                translate([rail_x, rail_y, 0])
                    cube([
                        holder_rail_segment_length,
                        holder_wall_thickness,
                        holder_wall_height
                    ]);
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

module accordion_arm_root(root_x, root_y, angle) {
    translate([root_x, root_y, 0])
        rotate([0, 0, angle])
            for (side = [-1, 1])
                arm_accordion_beam(side * arm_spring_pair_spacing / 2);
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

module printed_skeleton() {
    difference() {
        union() {
            center_body();
            arm_with_pad(center_width / 2 - 3, center_height / 2 - 17, arm_angle);
            arm_with_pad(-center_width / 2 + 3, center_height / 2 - 17, 180 - arm_angle);
            arm_with_pad(center_width / 2 - 3, -center_height / 2 + 17, -arm_angle);
            arm_with_pad(-center_width / 2 + 3, -center_height / 2 + 17, arm_angle - 180);
            holder_on_arm(center_width / 2 - 3, center_height / 2 - 17, arm_angle);
            holder_on_arm(-center_width / 2 + 3, center_height / 2 - 17, 180 - arm_angle);
            holder_on_arm(center_width / 2 - 3, -center_height / 2 + 17, -arm_angle);
            holder_on_arm(-center_width / 2 + 3, -center_height / 2 + 17, arm_angle - 180);
            accordion_arm_root(center_width / 2 - 3, center_height / 2 - 17, arm_angle);
            accordion_arm_root(-center_width / 2 + 3, center_height / 2 - 17, 180 - arm_angle);
            accordion_arm_root(center_width / 2 - 3, -center_height / 2 + 17, -arm_angle);
            accordion_arm_root(-center_width / 2 + 3, -center_height / 2 + 17, arm_angle - 180);
            tof_sensor_boom();
            // The floor overlaps the chassis by 2 mm, making the side cradle one printed part.
            translate([servo_cradle_center_x, 0, servo_cradle_mount_z])
                servo_cradle();
        }

        holder_slots_on_arm(center_width / 2 - 3, center_height / 2 - 17, arm_angle);
        holder_slots_on_arm(-center_width / 2 + 3, center_height / 2 - 17, 180 - arm_angle);
        holder_slots_on_arm(center_width / 2 - 3, -center_height / 2 + 17, -arm_angle);
        holder_slots_on_arm(-center_width / 2 + 3, -center_height / 2 + 17, arm_angle - 180);
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
