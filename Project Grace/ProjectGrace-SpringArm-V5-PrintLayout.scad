// Project Grace V5 print layout: chassis and eight V5 wheels.
// Servo cradle is disabled; wheels lie face-down on the bed.

$combined_print_layout = true;
include <ProjectGrace-SpringArm-V5.scad>
include <ProjectGrace-DoubleTireWheel-V5.scad>

wheel_column_offset = 62;
wheel_row_pitch = 36;
wheel_outer_face_projection = outer_retaining_lip_width - outer_retaining_lip_overlap;
wheel_bed_z = wheel_width / 2 + wheel_outer_face_projection;

color("orange")
    printed_skeleton(include_servo_cradle = false);

for (side = [-1, 1])
    for (index = [0 : 3]) {
        wheel_y = (index - 1.5) * wheel_row_pitch;

        translate([side * wheel_column_offset, wheel_y, wheel_bed_z])
            color("red")
                double_tire_wheel_v5();
    }
