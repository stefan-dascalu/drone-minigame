# Drone Mini Games – 3D Aerial Adventure

## Author
**Dascalu Ștefan-Nicolae**

## Design Overview

Drone Mini Games is a 3D aerial adventure where the player controls a drone in a dynamic environment. 

### Problem Breakdown
- **Drone Control and Dynamics:**  
  The drone is constructed from simple geometric primitives arranged in an X-formation, with distinct colors for the body and propellers. The propellers rotate continuously, and the drone supports translation along the X, Y, and Z axes as well as rotation around the Y-axis. User inputs allow for forward/backward movement, lateral shifts, vertical motion, and rotation, ensuring responsive and fluid control.

- **Environment and Terrain Generation:**  
  The terrain is created using a regular grid mesh whose vertex heights are modified in the vertex shader via a basic noise function. This produces natural height variations and gives the environment a realistic feel. Additionally, basic obstacles such as trees and rocks are procedurally generated to enrich the scene, contributing to a more engaging delivery scenario.

- **Delivery Mode Specifics (partially implemented):**  
  Although the project is intended to support various mini game modes (racing, shooter, etc.), the current implementation is centered on a delivery mode. In this mode, the focus is on managing drone flight in a challenging terrain. Collision detection prevents the drone from intersecting with the ground or obstacles, and a dynamic third-person camera follows the drone to maintain an optimal view of the surroundings.

## Implementation Details

The project utilizes object-oriented programming (OOP) alongside mathematical techniques (such as transformation matrices and procedural noise) to simulate realistic drone dynamics and environmental interactions. Key implementation aspects include:

- **Drone Module:**  
  Manages the drone’s position, orientation, movement, and propeller animation. The drone is built from simple shapes, and its control system supports smooth navigation in three dimensions.

- **Terrain Generation:**  
  A grid-based mesh is used to model the terrain, with vertex shader modifications applying basic noise to generate natural-looking height variations.

- **Collision Detection:**  
  Ensures that the drone does not intersect with the terrain or obstacles, maintaining realistic interactions in the delivery mode.

- **Camera Management:**  
  A dynamic third-person camera continuously follows the drone, providing a clear view of the environment during flight.

**Implementation:** The full implementation is located in `src/lab_m1/Tema2`. The rest of the repository contains my lab work and other assignments.
