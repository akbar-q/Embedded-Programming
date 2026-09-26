// Tall gauge needle horn for an SG90-style micro servo.
// Designed as a direct-fit prototype using the servo's original centre screw.
// Test the spline_socket_diameter with a short print before printing the full part.
// Dimensions are in millimetres.

hub_diameter = 8;
hub_height = 12;
socket_diameter = 4.9;
socket_depth = 3;
centre_screw_diameter = 2.2;
screw_head_diameter = 4.2;
screw_head_depth = 2;

needle_length = 25;
needle_width = 3;
needle_thickness = 2;
needle_tip_length = 14;
facet_count = 64;

$fn = facet_count;

difference() {
    union() {
        cylinder(d = hub_diameter, h = hub_height);

        translate([0, 0, hub_height - needle_thickness])
            linear_extrude(height = needle_thickness)
                polygon(points = [
                    [0, -needle_width / 2],
                    [needle_length - needle_tip_length, -needle_width / 2],
                    [needle_length, 0],
                    [needle_length - needle_tip_length, needle_width / 2],
                    [0, needle_width / 2]
                ]);
    }

    // Smooth socket for the SG90 output spline; adjust after a test fit.
    translate([0, 0, -0.1])
        cylinder(d = socket_diameter, h = socket_depth + 0.1);

    // Original SG90 centre screw passes through this hole.
    translate([0, 0, -0.1])
        cylinder(d = centre_screw_diameter, h = hub_height + 0.2);

    // Recesses the screw head so it does not protrude above the hub.
    translate([0, 0, hub_height - screw_head_depth])
        cylinder(d = screw_head_diameter, h = screw_head_depth + 0.1);
}