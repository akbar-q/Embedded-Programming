// Project Grace - initial printed skeleton.
// Dimensions are in millimetres. Motor holes and clamp features will be added later.

// Set to "motor_reference" to inspect the measured compound motor envelope.
view_mode = "skeleton";

frame_thickness = 5;

// Central body.
center_width = 30;
center_height = 58;
center_opening_size = 14;
triangle_opening_width = 11;
triangle_opening_height = 10;

// Four identical arms and their solid motor-mount pads.
arm_length = 92;
arm_width = 19;
arm_slot_width = 9;
arm_slot_start = 13;
arm_slot_end_clearance = 7;
end_pad_length = 32;
end_pad_width = 36;
arm_angle = 72;

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

module arm_with_pad(root_x, root_y, angle) {
    arm_slot_length = arm_length - end_pad_length - arm_slot_start - arm_slot_end_clearance;

    translate([root_x, root_y, 0])
        rotate([0, 0, angle])
            difference() {
                union() {
                    translate([0, -arm_width / 2, 0])
                        cube([arm_length, arm_width, frame_thickness]);
                    translate([arm_length - end_pad_length, -end_pad_width / 2, 0])
                        cube([end_pad_length, end_pad_width, frame_thickness]);
                }

                // Long lightening slot, matching the reference skeleton.
                translate([arm_slot_start, -arm_slot_width / 2, -1])
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

module motor_holder() {
    holder_outer_width = motor_width + motor_fit_clearance + 2 * holder_wall_thickness;

    union() {
        // Rear stop prevents the motor moving towards the centre of the frame.
        difference() {
            translate([0, -holder_outer_width / 2, 0])
                cube([holder_back_stop_thickness, holder_outer_width, holder_wall_height]);
            translate([
                -1,
                -wire_exit_width / 2,
                (holder_wall_height - wire_exit_height) / 2
            ])
                cube([holder_back_stop_thickness + 2, wire_exit_width, wire_exit_height]);
        }

        // Low rails locate the compound motor without interfering with its gearbox or shaft.
        translate([0, -holder_outer_width / 2, 0])
            cube([holder_length, holder_wall_thickness, holder_wall_height]);
        translate([0, holder_outer_width / 2 - holder_wall_thickness, 0])
            cube([holder_length, holder_wall_thickness, holder_wall_height]);
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

if (view_mode == "motor_reference")
    compound_motor_reference();
else
    color("orange")
        printed_skeleton();