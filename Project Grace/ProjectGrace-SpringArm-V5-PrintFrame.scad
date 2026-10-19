// Project Grace V5 frame-only print file with the servo cradle disabled.

$combined_print_layout = true;
include <ProjectGrace-SpringArm-V5.scad>

color("orange")
    printed_skeleton(include_servo_cradle = false);
