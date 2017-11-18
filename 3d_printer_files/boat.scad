// TODO:
// Widen the spot for protoboard. Switch to 2 pegs, w/ proper diameter. Tray?
// Measure servo horns, ensure enough room for them
// Make mast/boom holes smaller?
// 3 (4?) levels: keel, electronics, deck
// In Blender:
// Rework rudder fin, center of gravity
// Narrow the back of the boat

BOAT_WIDTH = 50;
BOAT_LENGTH = 150;
BOAT_DEPTH = 32;
HULL_THICKNESS = 1;

BATTERY_SIZE_PADDING = 2;
BATTERY_INNER_WIDTH = 32 + BATTERY_SIZE_PADDING;
BATTERY_INNER_LENGTH = 59 + BATTERY_SIZE_PADDING;
BATTERY_INNER_HEIGHT = 28 + BATTERY_SIZE_PADDING;
BATTERY_BOX_THICKNESS = 1;
BATTERY_FORWARD_TRANSLATE = 36;

SERVO_SIZE_PADDING = 2;
SERVO_INNER_WIDTH = 11.8 + SERVO_SIZE_PADDING;
SERVO_INNER_LENGTH = 22.5 + SERVO_SIZE_PADDING;
SERVO_INNER_HEIGHT = 15.9 + SERVO_SIZE_PADDING;
SERVO_BOX_THICKNESS = 1;
SERVO_CONNECTOR_WIDTH = 9.5;

MAST_DIAMETER = 4;
MAST_PADDING = 0.5;
MAST_MOUNT_THICKNESS = 1;
MAST_MOUNT_INNER_DIAMETER = MAST_DIAMETER + 2*(MAST_PADDING);
MAST_HEIGHT = 150;
MAST_FORWARD_TRANSLATE = BATTERY_FORWARD_TRANSLATE + BATTERY_INNER_LENGTH/2 + BATTERY_BOX_THICKNESS/2 + MAST_MOUNT_INNER_DIAMETER/2;

MSR_RAIL_WIDTH = MAST_MOUNT_INNER_DIAMETER*1;
MSR_RAIL_LENGTH = 14;
MSR_RAIL_HEIGHT = 10;

BOOM_LENGTH = BOAT_LENGTH*.75;
RING_OUTER_RADIUS = 3;
RING_INNER_RADIUS = 2;

RAIL_WIDTH = (BOAT_WIDTH-BATTERY_INNER_WIDTH-BATTERY_BOX_THICKNESS)/2-HULL_THICKNESS;
RAIL_LENGTH = 40;
RAIL_HEIGHT = BOAT_DEPTH;

RUDDER_MOUNT_WIDTH = 8;
RUDDER_MOUNT_LENGTH = 20;
RUDDER_WIDTH = RUDDER_MOUNT_WIDTH - 2;
RUDDER_PIVOT_DIAMETER = RUDDER_WIDTH/2;
RUDDER_PIVOT_HOLE_DIAMETER = RUDDER_MOUNT_WIDTH/2 - 1;

CIRCUIT_BOARD_TRAY_THICKNESS = 1;
CIRCUIT_BOARD_WIDTH = 84 + CIRCUIT_BOARD_TRAY_THICKNESS; // measured 81.21
CIRCUIT_BOARD_LENGTH = 53 + CIRCUIT_BOARD_TRAY_THICKNESS; // measured 50.73
CIRCUIT_BOARD_DEPTH = 5 + CIRCUIT_BOARD_TRAY_THICKNESS; // depth of tray, not board
CIRCUIT_BOARD_HOLE_DIAMETER = 2.75; // measured 3.2

//all();
boat();
module all() {
  boat();
  mast();
  boom();
  translate([-130, 0, 5]) {
    rotate([0, 90, 90]) {
      rudder();
    }
  }
}

module boat() {
  hull();
//  keel();
  batteryBox();
  sailServo();
  rudderServo();
  rudderMount();
  mastMount();
  mainSheetRing();
  //pegs();
  //circuitBoardTray();
}

module hull() {
  aftLength = BOAT_LENGTH * .75;
  foreLength = BOAT_LENGTH - aftLength;
  polyhedron(
    points = [
      // Underside
      [0, aftLength/2 + foreLength, 0],
      [-BOAT_WIDTH/2, aftLength/2, 0],
      [-BOAT_WIDTH/2, -aftLength/2, 0],
      [BOAT_WIDTH/2, -aftLength/2, 0],
      [BOAT_WIDTH/2, aftLength/2, 0],

      // Top outside
      [0, aftLength/2 + foreLength, BOAT_DEPTH],
      [-BOAT_WIDTH/2, aftLength/2, BOAT_DEPTH],
      [-BOAT_WIDTH/2, -aftLength/2, BOAT_DEPTH],
      [BOAT_WIDTH/2, -aftLength/2, BOAT_DEPTH],
      [BOAT_WIDTH/2, aftLength/2, BOAT_DEPTH],

      // Top inside
      [0, aftLength/2 + foreLength - HULL_THICKNESS, BOAT_DEPTH],
      [-BOAT_WIDTH/2+HULL_THICKNESS, aftLength/2, BOAT_DEPTH],
      [-BOAT_WIDTH/2+HULL_THICKNESS, HULL_THICKNESS-aftLength/2, BOAT_DEPTH],
      [BOAT_WIDTH/2-HULL_THICKNESS, HULL_THICKNESS-aftLength/2, BOAT_DEPTH],
      [BOAT_WIDTH/2-HULL_THICKNESS, aftLength/2, BOAT_DEPTH],

      // Floor
      [0, aftLength/2 + foreLength - HULL_THICKNESS, HULL_THICKNESS],
      [-BOAT_WIDTH/2+HULL_THICKNESS, aftLength/2, HULL_THICKNESS],
      [-BOAT_WIDTH/2+HULL_THICKNESS, HULL_THICKNESS-aftLength/2, HULL_THICKNESS],
      [BOAT_WIDTH/2-HULL_THICKNESS, HULL_THICKNESS-aftLength/2, HULL_THICKNESS],
      [BOAT_WIDTH/2-HULL_THICKNESS, aftLength/2, HULL_THICKNESS],
    ],
    triangles = [
      // Underside
      [3, 1, 2],
      [3, 4, 1],
      [1, 4, 0],

      // Outer Walls
      [2, 1, 7],
      [7, 1, 6],
      [1, 0, 6],
      [6, 0, 5],
      [0, 4, 5],
      [5, 4, 9],
      [4, 3, 8],
      [8, 9, 4],
      [3, 2, 7],
      [3, 7, 8],

      // Inner Walls
      [12, 11, 17],
      [17, 11, 16],
      [11, 10, 16],
      [16, 10, 15],
      [10, 14, 15],
      [15, 14, 19],
      [14, 13, 18],
      [18, 19, 14],
      [13, 12, 17],
      [13, 17, 18],
      
      // Top of railing
      [8, 14, 9],
      [8, 13, 14],
      [9, 14, 10],
      [9, 10, 5],
      [5, 11, 6],
      [5, 10, 11],
      [7, 6, 11],
      [7, 11, 12],
      [7, 13, 8],
      [7, 12, 13],

      // Floor
      [18, 17, 16],
      [18, 16, 19],
      [16, 15, 19],
    ]
  );
}

module keel() {
  aftLength = BOAT_LENGTH * .75;
  foreLength = BOAT_LENGTH - aftLength;
  polyhedron(
    points = [
      // Underside
      [0, aftLength/2 + foreLength, 0],
      [-BOAT_WIDTH/2, aftLength/2, 0],
      [-BOAT_WIDTH/2, -aftLength/2, 0],
      [BOAT_WIDTH/2, -aftLength/2, 0],
      [BOAT_WIDTH/2, aftLength/2, 0],

      [-BOAT_WIDTH*.25, aftLength*.55, -BOAT_DEPTH*.25], // 5
      [-BOAT_WIDTH*.25, -aftLength*.3, -BOAT_DEPTH*.25],
      [BOAT_WIDTH*.25, -aftLength*.3, -BOAT_DEPTH*.25],
      [BOAT_WIDTH*.25, aftLength*.55, -BOAT_DEPTH*.25],

      [-BOAT_WIDTH*.1, aftLength*.35, -BOAT_DEPTH*.67], // 9
      [-BOAT_WIDTH*.1, -aftLength*.15, -BOAT_DEPTH*.67],
      [BOAT_WIDTH*.1, -aftLength*.15, -BOAT_DEPTH*.67],
      [BOAT_WIDTH*.1, aftLength*.35, -BOAT_DEPTH*.67],

      [0, -aftLength*.1, -BOAT_DEPTH*1.2], // 13
      [0, aftLength*.3, -BOAT_DEPTH*1.2],
    ],
    triangles = [
      // Underside
      [3, 2, 1],
      [3, 1, 4],
      [1, 0, 4],

      [0, 1, 5],
      [1, 2, 5],
      [5, 2, 6],
      [2, 3, 6],
      [6, 3, 7],
      [3, 4, 7],
      [7, 4, 8],
      [4, 0, 8],
      [8, 0, 5],

      [5, 6, 9],
      [9, 6, 10],
      [6, 7, 10],
      [10, 7, 11],
      [7, 8, 11],
      [11, 8, 12],
      [8, 5, 12],
      [12, 5, 9],

      [9, 10, 13],
      [9, 13, 14],
      [10, 11, 13],
      [11, 12, 13],
      [13, 12, 14],
      [9, 14, 12],
    ]
  );
}

module batteryBox() {
  color("red") {
    translate([0, BATTERY_FORWARD_TRANSLATE, HULL_THICKNESS]) {
      openTopBox(BATTERY_INNER_WIDTH+BATTERY_BOX_THICKNESS, BATTERY_INNER_LENGTH+BATTERY_BOX_THICKNESS, BATTERY_INNER_HEIGHT+BATTERY_BOX_THICKNESS, BATTERY_BOX_THICKNESS);
    }
  }
}

module servoBox() {
  servoOuterHeight = SERVO_INNER_HEIGHT + SERVO_BOX_THICKNESS;
  color("blue") {
    translate([0, 0, BOAT_DEPTH-SERVO_INNER_HEIGHT-HULL_THICKNESS]) {
      difference() {
        union() {
          openTopBox(SERVO_INNER_WIDTH+SERVO_BOX_THICKNESS,
                     SERVO_INNER_LENGTH+SERVO_BOX_THICKNESS,
                     servoOuterHeight, SERVO_BOX_THICKNESS);
          // Wire-side mount
          translate([0,15,14.9]) {
            rotate([0,90,90]) {
              prism(8, SERVO_INNER_WIDTH+SERVO_BOX_THICKNESS, 4.7);
            }
          }
        } //union
        // Connector hole
        translate([SERVO_CONNECTOR_WIDTH*-.5,
                   SERVO_INNER_LENGTH/2 - 1,
                   SERVO_BOX_THICKNESS+4.75]) {
          cube([SERVO_CONNECTOR_WIDTH, 3, 4]);
        }
        // Wire channel cutout
        translate([SERVO_CONNECTOR_WIDTH*-.5, 10, 8]) {
          cube([SERVO_CONNECTOR_WIDTH,
                4.7,
                SERVO_INNER_HEIGHT-6.75], false);
        }
      } //difference
      // Non-wire-side mount
      translate([0,-15,14.9]) {
        rotate([0,90,-90]) {
          prism(8, SERVO_INNER_WIDTH+SERVO_BOX_THICKNESS, 4.7);
        }
      }
    }
    // Stand
    translate([0, 0, (HULL_THICKNESS+BOAT_DEPTH-servoOuterHeight)/2]) {
      cube([SERVO_INNER_WIDTH+SERVO_BOX_THICKNESS, SERVO_INNER_LENGTH+SERVO_BOX_THICKNESS, BOAT_DEPTH-servoOuterHeight-HULL_THICKNESS], true);
    }
  }
}

module sailServo() {
  translate([5.9, -2.4, 0]) {
    rotate([0, 0, 90]) {
      servoBox();
    }
  }
}

module rudderServo() {
  translate([0, -38.5, 0]) {
    servoBox();
  }
}

module rudderMount() {
  color("orange") {
    translate([0, -59, 0]) {
      translate([-RUDDER_MOUNT_WIDTH/2,
                 4-RUDDER_MOUNT_LENGTH,
                 0]) {
        difference() {
          cube([RUDDER_MOUNT_WIDTH,
                RUDDER_MOUNT_LENGTH,
                BOAT_DEPTH]);
          translate([0, -20, -32]) {
            rotate([35, 0, 0]) {
              cube([50, 50, 50]);
            }
          }
          translate([RUDDER_MOUNT_WIDTH/2, 7, BOAT_DEPTH*.7]) {
            cylinder($fn=20, BOAT_DEPTH*.5,
                     RUDDER_PIVOT_HOLE_DIAMETER, RUDDER_PIVOT_HOLE_DIAMETER);
          }
        }
      }
    }
  }
}

module rudder() {
  color("green") {
    translate([0,
               -RUDDER_MOUNT_LENGTH-42,
               BOAT_DEPTH*.75]) {
      translate([0, -6, -2]) {
        cylinder($fn=20, BOAT_DEPTH*.4,
                 RUDDER_PIVOT_DIAMETER, RUDDER_PIVOT_DIAMETER);
      }
      // Servo wire mount
      translate([RUDDER_WIDTH/-2, -RUDDER_PIVOT_DIAMETER-6, 10]) {
        cube([RUDDER_WIDTH, 14.8, 10]);
      }
      // Biggest part
      translate([-RUDDER_WIDTH/2, -24.2, BOAT_DEPTH*.25]) {
        cube([RUDDER_WIDTH, RUDDER_MOUNT_LENGTH*1.5, BOAT_DEPTH*.25]);
      }
      // Part that goes down
      translate([RUDDER_WIDTH/-2, -24.2, -BOAT_DEPTH*.5]) {
        cube([RUDDER_WIDTH, 10, BOAT_DEPTH]);
      }
      translate([RUDDER_WIDTH/-2, -8, -40]) {
        rotate([35, 0, 0]) {
          cube([RUDDER_WIDTH, 18, 40]);
        }
      }
    }
  }
}

module mastMount() {
  translate([0, MAST_FORWARD_TRANSLATE, 0]) {
    color("orange") {
      openTopBox(MAST_MOUNT_INNER_DIAMETER, MAST_MOUNT_INNER_DIAMETER, BOAT_DEPTH, MAST_MOUNT_THICKNESS);
    }
  }
}

module mast() {
  color("orange") {
    translate([BOAT_WIDTH, BOAT_LENGTH*-.375, 0]) {
      cube([MAST_DIAMETER, MAST_HEIGHT, MAST_DIAMETER]);
      ring(BOAT_DEPTH+5);
      ring(BOAT_DEPTH+15);
      ring(BOAT_DEPTH+35);
      ring(BOAT_DEPTH+55);
      ring(BOAT_DEPTH+75);
      ring(BOAT_DEPTH+95);
      ring(BOAT_DEPTH+113);
    }
  }
}

module ring(pos) {
  translate([-1+RING_OUTER_RADIUS-MAST_DIAMETER/2, pos+MAST_DIAMETER/2, 0]) {
    difference() {
      cylinder($fn=20, MAST_DIAMETER, RING_OUTER_RADIUS, RING_OUTER_RADIUS);
      cylinder($fn=20, MAST_DIAMETER, RING_INNER_RADIUS, RING_INNER_RADIUS);
    }
  }
}

module boom() {
  color("orange") {
    translate([BOAT_WIDTH*.75, -BOOM_LENGTH/2, 0]) {
      cube([MAST_DIAMETER, BOOM_LENGTH, MAST_DIAMETER]);
      translate([2,0,0]) {
        ring(-3);
      }
      ring(22.3);
      ring(43.6);
      ring(64.9);
      ring(86.2);
      ring(107.5);
    }
  }
}

module mainSheetRing() {
  translate([0, -15.3, 7.5]) {
    color("orange") {
      translate([0, 0, BOAT_DEPTH-.5]) {
        difference() {
          openTopBox(MAST_MOUNT_INNER_DIAMETER, MAST_MOUNT_INNER_DIAMETER, 9.5, MAST_MOUNT_THICKNESS);
          translate([0, MAST_MOUNT_INNER_DIAMETER/2, 4.5]) {
            sphere(MAST_MOUNT_INNER_DIAMETER/3, $fn=20);
          }
        }
      }

      translate([0, 1.5, BOAT_DEPTH-11.45]) {
        rotate([0, 180, 90]) {
          prism(8.0/*MSR_RAIL_LENGTH*/,
                MSR_RAIL_WIDTH,
                22);
        }
      }
    }
  }
}

module circuitBoardTray() {
  translate([0, 44, BOAT_DEPTH]) {
    //rotate([0,0,90]) {
    openTopBox(CIRCUIT_BOARD_WIDTH, CIRCUIT_BOARD_LENGTH, CIRCUIT_BOARD_DEPTH, 1);
    translate([37.15, 0, 0]) {
      cylinder($fn=20, h=CIRCUIT_BOARD_DEPTH, d=CIRCUIT_BOARD_HOLE_DIAMETER);
    }
    translate([-37.15, 0, 0]) {
      cylinder($fn=20, h=CIRCUIT_BOARD_DEPTH, d=CIRCUIT_BOARD_HOLE_DIAMETER);
    }
    //}
  }
}

module peg() {
  cylinder($fn=20, 4, 2, 2);
}

module pegs() {
  color("orange") {
    // Left rail
    translate([-((BOAT_WIDTH)/2-HULL_THICKNESS), 10, BOAT_DEPTH-RAIL_HEIGHT]) {
      cube([RAIL_WIDTH, RAIL_LENGTH, RAIL_HEIGHT]);
    }
    // Right rail
    translate([((BOAT_WIDTH)/2-HULL_THICKNESS-RAIL_WIDTH), 10, BOAT_DEPTH-RAIL_HEIGHT]) {
      cube([RAIL_WIDTH, RAIL_LENGTH, RAIL_HEIGHT]);
    }
    translate([-20, 40, BOAT_DEPTH]) {
      peg();
    }
    translate([20, 40, BOAT_DEPTH]) {
      peg();
    }
    translate([-20, 20, BOAT_DEPTH]) {
      peg();
    }
    translate([20, 20, BOAT_DEPTH]) {
      peg();
    }
  }
}

// Makes a box with the given outer dimensions but no top
module openTopBox(w, d, h, thickness=1.0, includeFrontWall=true, includeFloor=true) {
  translate([(w-thickness)/2, 0, 0]) {
    wall(d, h, 90, thickness);
  }
  translate([(w-thickness)/-2, 0, 0]) {
    wall(d, h, 90, thickness);
  }
  translate([0, (d-thickness)/-2, 0]) {
    wall(w, h, 0, thickness);
  }
  if (includeFrontWall) {
    translate([0, (d-thickness)/2, 0]) {
      wall(w, h, 0, thickness);
    }
  }
  if (includeFloor) {
    translate([0, d/2, thickness/2]) {
      rotate([90, 0, 0]) {
        wall(w, d, 0, thickness);
      }
    }
  }
}

// Makes a wall (a closed box) with specified width, height
// Origin will be on the floor, in the middle of the box
module wall(w, h, r=0.0, thickness=1.0) {
  rotate([0, 0, r]) {
  polyhedron(
    points = [
      [-w/2, -thickness/2, 0], // 0
      [w/2, -thickness/2, 0],  // 1
      [w/2, thickness/2, 0],   // 2
      [-w/2, thickness/2, 0],  // 3
      [-w/2, -thickness/2, h], // 4
      [w/2, -thickness/2, h],  // 5
      [w/2, thickness/2, h],   // 6
      [-w/2, thickness/2, h],  // 7
    ],
    triangles = [
      [0, 1, 2],
      [2, 3, 0],
      [4, 6, 5],
      [6, 4, 7],
      [0, 4, 1],
      [4, 5, 1],
      [2, 6, 3],
      [6, 7, 3],
      [0, 3, 4],
      [4, 3, 7],
      [1, 5, 2],
      [5, 6, 2],
    ]
  );
  }
}

module prism(w, l, h) {
  polyhedron(
    points = [
      [w/2, l/2, -h/2],
      [w/2, -l/2, -h/2],
      [-w/2, l/2, h/2],
      [-w/2, l/2, -h/2],
      [-w/2, -l/2, h/2],
      [-w/2, -l/2, -h/2],
    ],
    triangles = [
      [1, 5, 4],
      [0, 2, 3],
      [2, 4, 5],
      [2, 5, 3],
      [0, 3, 5],
      [0, 5, 1],
      [1, 4, 2],
      [1, 2, 0],
    ]
  );
}


