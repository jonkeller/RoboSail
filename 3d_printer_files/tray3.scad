BOAT_WIDTH = 50;
BOAT_LENGTH = 150;
BOAT_DEPTH = 32;
HULL_THICKNESS = 1;

BATTERY_SIZE_PADDING = 2;
BATTERY_INNER_WIDTH = 30 + BATTERY_SIZE_PADDING;
BATTERY_INNER_LENGTH = 56 + BATTERY_SIZE_PADDING;
BATTERY_INNER_HEIGHT = 28 + BATTERY_SIZE_PADDING;
BATTERY_BOX_THICKNESS = 1;

SERVO_SIZE_PADDING = 2;
SERVO_INNER_WIDTH = 11.8 + SERVO_SIZE_PADDING;
SERVO_INNER_LENGTH = 22.5 + SERVO_SIZE_PADDING;
SERVO_INNER_HEIGHT = 15.9 + SERVO_SIZE_PADDING;
SERVO_BOX_THICKNESS = 1;

MAST_HOLE_SIZE = 6;

hull();
batteryBox();
sailServo();
rudderServo();
rudderMount();
mastMount();
mainSheetRing();
pegs();

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

module oldHull() {
  aftLength = BOAT_LENGTH * .75;
  foreLength = BOAT_LENGTH - aftLength;
  openTopBox(BOAT_WIDTH, aftLength, BOAT_DEPTH, HULL_THICKNESS, 0);
  polyhedron(
    points = [
      // Bottom of triangle (floor)
      [BOAT_WIDTH/2, aftLength/2, 0],
      [-BOAT_WIDTH/2, aftLength/2, 0],
      [0, aftLength/2 + foreLength, 0],

      // Top of triangle (floor)
      [BOAT_WIDTH/2, aftLength/2, HULL_THICKNESS],
      [-BOAT_WIDTH/2, aftLength/2, HULL_THICKNESS],
      [0, aftLength/2 + foreLength, HULL_THICKNESS],

      // Top outside of walls
      [BOAT_WIDTH/2, aftLength/2, BOAT_DEPTH],
      [-BOAT_WIDTH/2, aftLength/2, BOAT_DEPTH],
      [0, aftLength/2 + foreLength, BOAT_DEPTH],

      // Top inside of walls
      [BOAT_WIDTH/2-HULL_THICKNESS, aftLength/2, BOAT_DEPTH],
      [-BOAT_WIDTH/2+HULL_THICKNESS, aftLength/2, BOAT_DEPTH],
      [0, aftLength/2 + foreLength -HULL_THICKNESS, BOAT_DEPTH],

      // Bottom inside of walls
      [BOAT_WIDTH/2-HULL_THICKNESS, aftLength/2, HULL_THICKNESS],
      [-BOAT_WIDTH/2+HULL_THICKNESS, aftLength/2, HULL_THICKNESS],
      [0, aftLength/2 + foreLength -HULL_THICKNESS, HULL_THICKNESS],
    ],
    triangles = [
      // Triangles
      [0, 1, 2],
      [3, 4, 5],
      // Outer walls
      [1, 2, 7],
      [2, 8, 7],
      [2, 0, 8],
      [0, 6, 8],
      // Inner walls
      [10, 11, 13],
      [11, 14, 13],
      [11, 9, 14],
      [9, 12, 14],
      // Top of walls
      [6, 9, 11],
      [11, 8, 6],
      [10, 7, 11],
      [11, 7, 8],
      // Wall edges
      [3, 12, 9],
      [3, 9, 6],
      [4, 10, 13],
      [4, 7, 10],
      // Floor edge
      [0, 1, 3],
      [3, 1, 4],
    ]
  );
}

module batteryBox() {
  color("red") {
    translate([0, 30.5, HULL_THICKNESS]) {
      openTopBox(BATTERY_INNER_WIDTH+BATTERY_BOX_THICKNESS, BATTERY_INNER_LENGTH+BATTERY_BOX_THICKNESS, BATTERY_INNER_HEIGHT+BATTERY_BOX_THICKNESS, BATTERY_BOX_THICKNESS);
    }
  }
}

module servoBox() {
  servoOuterHeight = SERVO_INNER_HEIGHT + SERVO_BOX_THICKNESS;
  color("blue") {
    translate([0, 0, BOAT_DEPTH-SERVO_INNER_HEIGHT-HULL_THICKNESS]) {
      openTopBox(SERVO_INNER_WIDTH+SERVO_BOX_THICKNESS, SERVO_INNER_LENGTH+SERVO_BOX_THICKNESS, servoOuterHeight, SERVO_BOX_THICKNESS);
    }
    translate([0, 0, (HULL_THICKNESS+BOAT_DEPTH-servoOuterHeight)/2]) {
      cube([SERVO_INNER_WIDTH+SERVO_BOX_THICKNESS, SERVO_INNER_LENGTH+SERVO_BOX_THICKNESS, BOAT_DEPTH-servoOuterHeight-HULL_THICKNESS], true);
    }
  }
}

module sailServo() {
  translate([5.9, -6.4, 0]) {
    rotate([0, 0, 90]) {
      servoBox();
    }
  }
}

module rudderServo() {
  translate([0, -35, 0]) {
    servoBox();
  }
}

module rudderMount() {
  translate([0, -51, 0]) {
    peg();
  }
}

module mastMount() {
  translate([0, BOAT_LENGTH*7/16 - 2.7, 0]) {
    color("orange") {
      openTopBox(MAST_HOLE_SIZE, MAST_HOLE_SIZE, BOAT_DEPTH);
    }
  }
}

module mainSheetRing() {
  translate([0, -19, 0]) {
    color("orange") {
      difference() {
        openTopBox(MAST_HOLE_SIZE, MAST_HOLE_SIZE, BOAT_DEPTH+16);
        translate([0, MAST_HOLE_SIZE/2, BOAT_DEPTH+12]) {
          sphere(MAST_HOLE_SIZE/3, $fn=20);
        }
      }
    }
  }
}

module peg() {
  color("orange") {
    cylinder($fn=20, BOAT_DEPTH+4, 2, 2);
  }
}

module pegs() {
  translate([-20, 40, 0]) {
    peg();
  }
  translate([20, 40, 0]) {
    peg();
  }
  translate([-20, 20, 0]) {
    peg();
  }
  translate([20, 20, 0]) {
    peg();
  }
}

// Makes a box with the given outer dimensions but no top
module openTopBox(w, d, h, thickness=1.0, includeFrontWall=true) {
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
  // Floor
  translate([0, d/2, thickness/2]) {
    rotate([90, 0, 0]) {
      wall(w, d, 0, thickness);
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
