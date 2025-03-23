// Arduino Nano 33 BLE Sense Enclosure
// Units: mm
// Designed for FDM 3D printing (PLA, PETG, ABS)

// Base module (bottom part)
module base() {
    difference() {
        union() {
            // Outer shell of the base
            cube([69, 44, 22]);
            
            // Add screw posts at corners (~6 mm from edges)
            translate([6, 6, 2]) cylinder(h=20, r=4);
            translate([6, 38, 2]) cylinder(h=20, r=4);
            translate([63, 6, 2]) cylinder(h=20, r=4);
            translate([63, 38, 2]) cylinder(h=20, r=4);
            
            // Add Arduino mounting standoffs
            // Positioned to match Arduino Nano 33 BLE Sense holes (38.1 mm apart)
            translate([15.45, 12, 2]) cylinder(h=2, r=2.5);
            translate([53.55, 12, 2]) cylinder(h=2, r=2.5);
        }
        
        // Subtract internal cavity (65 x 40 x 20 mm)
        translate([2, 2, 2]) cube([65, 40, 20]);
        
        // Subtract M3 screw holes from screw posts (Ø 3 mm)
        translate([6, 6, 2]) cylinder(h=20, r=1.5);
        translate([6, 38, 2]) cylinder(h=20, r=1.5);
        translate([63, 6, 2]) cylinder(h=20, r=1.5);
        translate([63, 38, 2]) cylinder(h=20, r=1.5);
        
        // Subtract M2 screw holes from standoffs (Ø 2.2 mm)
        translate([15.45, 12, 2]) cylinder(h=2, r=1.1);
        translate([53.55, 12, 2]) cylinder(h=2, r=1.1);
        
        // Subtract gasket groove (1.5 mm deep, along top edge)
        translate([0, 0, 20.5]) linear_extrude(1.5) difference() {
            square([69, 44]);
            translate([2, 2]) square([65, 40]);
        }
    }
}

// Lid module (top cover)
module lid() {
    difference() {
        // Flat lid (69 x 44 x 3 mm)
        cube([69, 44, 3]);
        
        // Subtract M3 screw holes (Ø 3 mm, aligned with screw posts)
        translate([6, 6, -1]) cylinder(h=5, r=1.5);
        translate([6, 38, -1]) cylinder(h=5, r=1.5);
        translate([63, 6, -1]) cylinder(h=5, r=1.5);
        translate([63, 38, -1]) cylinder(h=5, r=1.5);
    }
}

// Render the parts (uncomment one at a time for printing)
//base();
translate([80, 0, 0]) lid(); // Optional: view lid beside base
