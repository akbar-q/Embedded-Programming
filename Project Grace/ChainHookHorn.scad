// SG90-compatible chain hook horn for Project Grace.
// Test the spline fit with a short print before printing the full part.

servo_hub_diameter = 8;
servo_hub_height = 12;
servo_socket_diameter = 4.9;
servo_socket_depth = 3;
servo_centre_screw_diameter = 2.2;
servo_screw_head_diameter = 4.2;
servo_screw_head_depth = 2;

hook_outer_radius = 8;
hook_inner_radius = 4.5;
hook_centre_x = 20;
hook_opening_width = 6;
hook_arm_width = 5;
hook_plate_thickness = 3;

module chain_hook_horn() {
    difference() {
        union() {
            cylinder(d = servo_hub_diameter, h = servo_hub_height, $fn = 64);

            translate([
                0,
                -hook_arm_width / 2,
                servo_hub_height - hook_plate_thickness
            ])
                cube([
                    hook_centre_x - hook_inner_radius + 1,
                    hook_arm_width,
                    hook_plate_thickness
                ]);

            translate([hook_centre_x, 0, servo_hub_height - hook_plate_thickness])
                cylinder(r = hook_outer_radius, h = hook_plate_thickness, $fn = 64);
        }

        translate([hook_centre_x, 0, servo_hub_height - hook_plate_thickness - 0.1])
            cylinder(r = hook_inner_radius, h = hook_plate_thickness + 0.2, $fn = 64);
        translate([
            hook_centre_x,
            -hook_opening_width / 2,
            servo_hub_height - hook_plate_thickness - 0.1
        ])
            cube([
                hook_outer_radius + 1,
                hook_opening_width,
                hook_plate_thickness + 0.2
            ]);

        translate([0, 0, -0.1])
            cylinder(d = servo_socket_diameter, h = servo_socket_depth + 0.1, $fn = 64);
        translate([0, 0, -0.1])
            cylinder(d = servo_centre_screw_diameter, h = servo_hub_height + 0.2, $fn = 64);
        translate([0, 0, servo_hub_height - servo_screw_head_depth])
            cylinder(d = servo_screw_head_diameter, h = servo_screw_head_depth + 0.1, $fn = 64);
    }
}

color("orange")
    chain_hook_horn();