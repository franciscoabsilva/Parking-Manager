#ifndef PARKINGS_H
#define PARKINGS_H

#include "vehicles.h"
#include "registry.h"
#include "time.h"

/* Maximum amount of parkings that can be created */
#define PARKINGSMAX 20

/* Decide whether the parking is valid or not */
#define DUP_PARK -1
#define INV_CAP -2
#define INV_COST -3
#define MAX_PARK -4
#define VALID_PARK -5
#define INV_PARK -6
#define FULL_PARK -7
#define INV_EXIT -11

/* Parkings errors */
#define DUP_PARK_ERR ": parking already exists."
#define INV_CAP_ERR ": invalid capacity."
#define INV_COST_ERR "invalid cost."
#define MAX_PARK_ERR "too many parks."
#define INV_PARK_ERR ": no such parking."
#define FULL_PARK_ERR ": parking is full."
#define INV_EXIT_ERR ": invalid vehicle exit."

/* Decide whether two parkings have the same name */
#define SAME_PARKING_NAME 0

/*
Struct representing a parking:
Contains information about a parking, including it's name, capacity, empty spots,
billing rates, vehicles inside and registries
 */
typedef struct Parking {
    char *name;
    int capacity;
    int empty_spots;
    float billing_rates[3];
    Vehicle **vehicles; // Vehicles inside the parking
    Registry **registry; // Registries from the parking
    int registry_capacity;
    int registry_count;
} Parking;

/* Reads the parking name into the parking_name variable */
void read_parking_name(char *parking_name);

/*List all the parkings with their name, capacity and empty spots */
void list_parkings(Parking *parkings, int *parkings_count);

/* Checks for possible errors in the creation of a parking */
int check_parking(Parking *parkings, int *parkings_count, char *parking_name,
                  int capacity, float billing_rates[3]);

/* Creates a new parking, adding it to the parkings array */
void create_parking(Parking *parkings, int *parkings_count, char *parking_name,
                    int capacity, float billing_rates[3]);

/* Adds the registry to the parking */
void add_registry_to_parking(Parking *parking, Registry *registry);

/* Adds the vehicle to the end of the list of vehicles in the parking */
void add_vehicle_to_parking(Vehicle *vehicle, Parking *parking);

/* Removes the vehicle from the parking moving all the vehicles after it to the left in the list */
void remove_vehicle_from_parking(Vehicle *vehicle, Parking *parking);

/* Prints the billing of the parking for each date that the parking made money */
void print_parking_billing(Parking *parking);

/* Prints the billing of the parking for a specific date with all the exits and their specific payment */
void print_parking_billing_date(Parking *parking, Date date);

/* Checks if a parking name it's from an existing parking, returning it's index in the parkings array in case it is */
int check_parking_name(Parking *parkings, int *parkings_count, char *parking_name);

/* Removes all the registries from the parking, removing them from the vehicules aswell */
void remove_parking_registries(Parking *parking);

/* Removes all the vehicules from the parking removing their last registry */
void remove_vehicules_from_parking(Parking *parking);

/* Removes the parking from the parkings array */
void remove_parking(Parking *parkings, int *parkings_count, int parking_index);

/* Creates a array with all the parkings sorted by their names */
Parking **sort_parkings(Parking *parkings, int *parkings_count);

/* Prints all the parkings sorted by name remaining after removing a parking */
void print_remaining_parkings(Parking *parkings, int *parkings_count);

#endif