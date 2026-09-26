// Flat cap for a 4 mm diameter stem.
// Dimensions are in millimetres.

stem_diameter = 4;
stem_length = 5;
cap_diameter = 8;
cap_height = 2;
facet_count = 64;

$fn = facet_count;

union() {
    cylinder(d = stem_diameter, h = stem_length);
    translate([0, 0, stem_length])
        cylinder(d = cap_diameter, h = cap_height);
}