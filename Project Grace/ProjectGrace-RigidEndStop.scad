// Exploded preview of the split rigid end stop and its half-height glue ledges.

include <ProjectGrace-RigidEndStop-Parts.scad>

color("red")
    translate([-assembly_offset, 0, 0])
        rigid_end_stop_part_a();

color("red")
    translate([assembly_offset, 0, 0])
        rigid_end_stop_part_b();