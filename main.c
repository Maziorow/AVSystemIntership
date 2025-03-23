#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_VEHICLES 100

typedef enum {
    NORTH,
    SOUTH,
    EAST,
    WEST
} RoadDirection;

typedef enum {
    RED,
    YELLOW,
    GREEN
} LightState;

typedef struct {
    char vehicleId[20];
    RoadDirection startRoad;
    RoadDirection endRoad;
} Vehicle;

typedef struct {
    LightState lightState;
    int vehicleCount;
    Vehicle vehicles[MAX_VEHICLES];
} Road;

typedef struct {
    Road roads[4]; // Index 0: NORTH, 1: SOUTH, 2: EAST, 3: WEST
    int stepCount;
} Intersection;

void initializeIntersection(volatile Intersection *intersection){
    for(int i=0;i<4;i++){
        intersection->roads[i].lightState = RED;
        intersection->roads[i].vehicleCount = 0;
    }
}

void addVehicle(volatile Intersection *intersection, const char *vehicleId, RoadDirection startRoad, RoadDirection endRoad){
    Road road = intersection->roads[startRoad];
    if(road.vehicleCount >= MAX_VEHICLES){
        perror("Unable to add more cars to this road!\n");
        return;
    }

    strcpy(road.vehicles[road.vehicleCount].vehicleId,vehicleId);
    road.vehicles[road.vehicleCount].startRoad = startRoad;
    road.vehicles[road.vehicleCount].endRoad = endRoad;
    road.vehicleCount += 1;
}

void step(Intersection *intersection, FILE *output) {
    fprintf(output, "Step %d:\n", intersection->stepCount);

    for (int i = 0; i < 4; i++) {
        Road *road = &intersection->roads[i];

        if (road->lightState == GREEN && road->vehicleCount > 0) {
            Vehicle exitingVehicle = road->vehicles[0];
            RoadDirection start = exitingVehicle.startRoad;
            RoadDirection end = exitingVehicle.endRoad;

            // Identify conflicting roads
            Road *oppositeRoad = &intersection->roads[(i + 2) % 4]; // Opposing road
            Road *leftRoad = &intersection->roads[(i + 1) % 4]; // Road to left
            Road *rightRoad = &intersection->roads[(i + 3) % 4]; // Road to right

            bool canMove = true;

            if (end == (i + 2) % 4) { // Going straight
                if (oppositeRoad->lightState == GREEN && oppositeRoad->vehicleCount > 0) {
                    Vehicle opposingVehicle = oppositeRoad->vehicles[0];
                    if (opposingVehicle.endRoad == start) { 
                        canMove = false; // Collision risk
                        fprintf(output, "Vehicle %s cannot move: conflict with vehicle %s going straight\n",
                                exitingVehicle.vehicleId, opposingVehicle.vehicleId);
                    }
                }
            } else if (end == (i + 1) % 4) { // Turning left
                if (oppositeRoad->lightState == GREEN && oppositeRoad->vehicleCount > 0) {
                    Vehicle opposingVehicle = oppositeRoad->vehicles[0];
                    if (opposingVehicle.endRoad == (i + 2) % 4) { 
                        canMove = false; // Left turn yields
                        fprintf(output, "Vehicle %s must wait: yielding to vehicle %s going straight\n",
                                exitingVehicle.vehicleId, opposingVehicle.vehicleId);
                    }
                }
            } else if (end == (i + 3) % 4) { // Turning right
                if (rightRoad->lightState == GREEN && rightRoad->vehicleCount > 0) {
                    Vehicle rightVehicle = rightRoad->vehicles[0];
                    if (rightVehicle.endRoad == start) { 
                        canMove = false; // Right turn blocked
                        fprintf(output, "Vehicle %s cannot turn right: blocked by vehicle %s\n",
                                exitingVehicle.vehicleId, rightVehicle.vehicleId);
                    }
                }
            }

            if (canMove) {
                fprintf(output, "Vehicle %s moved from %d to %d\n", exitingVehicle.vehicleId, start, end);

                // Remove vehicle from queue
                for (int j = 1; j < road->vehicleCount; j++) {
                    road->vehicles[j - 1] = road->vehicles[j];
                }
                road->vehicleCount--;
            }
        }
    }

    // Change lights every 10 steps
    int nextLight = (intersection->stepCount / 10) % 4;
    for (int i = 0; i < 4; i++) {
        intersection->roads[i].lightState = (i == nextLight) ? GREEN : RED;
    }

    intersection->stepCount++;
}
int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char *inputFile = argv[1];
    const char *outputFile = argv[2];

    FILE *input = fopen(inputFile, "r");
    if (!input) {
        perror("Failed to open input file!\n");
        return 1;
    }

    FILE *output = fopen(outputFile, "w");
    if (!output) {
        perror("Failed to open output file!\n");
        fclose(input);
        return 1;
    }

    Intersection intersection;
    initializeIntersection(&intersection);
    printf("%d\n",intersection.roads[SOUTH].lightState);
}