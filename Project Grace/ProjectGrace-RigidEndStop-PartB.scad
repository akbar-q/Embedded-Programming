// Part B is flipped so its half-height shelf rests flat on the print bed.

include <ProjectGrace-RigidEndStop-Parts.scad>

color("red")
    translate([0, 0, disc_thickness])
        rotate([180, 0, 0])
            rigid_end_stop_part_b();