$fn = 128;

disc_diameter = 250;
disc_thickness = 5;
pipe_slot_width = 22.5;
lap_overlap_width = 10;
lap_seam_overlap = 0.25;
assembly_gap = 12;
assembly_offset = lap_overlap_width + assembly_gap / 2;

module pipe_entry_bore_2d() {
    circle(d = pipe_slot_width);
}

module pipe_entry_slot_2d(is_left) {
    slot_radius = pipe_slot_width / 2;

    intersection() {
        pipe_entry_bore_2d();

        if (is_left)
            translate([-slot_radius, -slot_radius])
                square([slot_radius, pipe_slot_width]);
        else
            translate([0, -slot_radius])
                square([slot_radius, pipe_slot_width]);
    }
}

module disc_half_outline_2d(is_left) {
    difference() {
        intersection() {
            circle(d = disc_diameter);

            if (is_left)
                translate([-disc_diameter / 2, -disc_diameter / 2])
                    square([disc_diameter / 2, disc_diameter]);
            else
                translate([0, -disc_diameter / 2])
                    square([disc_diameter / 2, disc_diameter]);
        }

        // Each half contributes one semicircular notch, forming a round hole when joined.
        pipe_entry_slot_2d(is_left);
    }
}

module lap_shelf_2d(extends_right) {
    difference() {
        intersection() {
            circle(d = disc_diameter);

            if (extends_right)
                translate([-lap_seam_overlap, -disc_diameter / 2 - 1])
                    square([lap_overlap_width + lap_seam_overlap, disc_diameter + 2]);
            else
                translate([-lap_overlap_width, -disc_diameter / 2 - 1])
                    square([lap_overlap_width + lap_seam_overlap, disc_diameter + 2]);
        }

        // Keep the complete pipe bore clear through the overlapping lap shelf.
        pipe_entry_bore_2d();
    }
}

module rigid_end_stop_part_a() {
    half_height = disc_thickness / 2;

    difference() {
        union() {
            linear_extrude(height = disc_thickness)
                disc_half_outline_2d(true);
            linear_extrude(height = half_height)
                lap_shelf_2d(true);
        }

        translate([-lap_overlap_width, -disc_diameter / 2 - 1, half_height])
            cube([lap_overlap_width + 0.01, disc_diameter + 2, half_height + 0.01]);
    }
}

module rigid_end_stop_part_b() {
    half_height = disc_thickness / 2;

    difference() {
        union() {
            linear_extrude(height = disc_thickness)
                disc_half_outline_2d(false);
            translate([0, 0, half_height])
                linear_extrude(height = half_height)
                    lap_shelf_2d(false);
        }

        translate([-0.01, -disc_diameter / 2 - 1, 0])
            cube([lap_overlap_width + 0.01, disc_diameter + 2, half_height]);
    }
}