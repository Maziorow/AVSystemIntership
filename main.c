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

void initializeIntersection(Intersection *intersection){
    intersection->stepCount = 0;
    for(int i=0;i<4;i++){
        intersection->roads[i].lightState = RED;
        intersection->roads[i].vehicleCount = 0;
    }
}

void addVehicle(Intersection *intersection, const char *vehicleId, RoadDirection startRoad, RoadDirection endRoad){
    Road *road = &intersection->roads[startRoad];
    if(road->vehicleCount >= MAX_VEHICLES){
        perror("Unable to add more cars to this road!\n");
        return;
    }

    strcpy(road->vehicles[road->vehicleCount].vehicleId,vehicleId);
    road->vehicles[road->vehicleCount].startRoad = startRoad;
    road->vehicles[road->vehicleCount].endRoad = endRoad;
    road->vehicleCount += 1;
}

void step(Intersection *intersection, FILE *output) {
    fprintf(output, "Step %d:\n", intersection->stepCount);

    for (int i = 0; i < 4; i++) {
        Road *road = &intersection->roads[i];

        if (road->lightState == GREEN && road->vehicleCount > 0) {
            Vehicle exitingVehicle = road->vehicles[0];
            fprintf(output, "%s\n", exitingVehicle.vehicleId);
            for(int j=1;j<road->vehicleCount;j++){
                road->vehicles[j-1] = road->vehicles[j];
            }
            road->vehicleCount -= 1;
        }
    }

    // Change lights every 10 steps
    int nextLight = (intersection->stepCount / 2) % 4;
    for (int i = 0; i < 4; i++) {
        intersection->roads[i].lightState = (i == nextLight) ? GREEN : RED;
    }

    intersection->stepCount++;
}
int main(int argc, char *argv[]){
    /*if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }*/

    //const char *inputFile = argv[1];
    //const char *outputFile = argv[2];
    const char *inputFile = "c_input.txt";
    const char *outputFile = "c_output.txt";

    char command[20];
    char vehicleId[20];
    RoadDirection startRoad;
    RoadDirection endRoad;

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
    

    while (fscanf(input, "%s", command) != EOF){
        if(strcmp(command, "addVehicle") == 0){
            fscanf(input, "%s %d %d", vehicleId, (int*)&startRoad, (int*)&endRoad);
            addVehicle(&intersection, vehicleId, startRoad, endRoad);
        } else if (strcmp(command, "step") == 0) {
            step(&intersection, output);
        }
    }

    free(input);
    free(output);
}