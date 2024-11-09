#ifndef VEHICLES_H
#define VEHICLES_H

#include "time.h"

typedef struct Parking Parking;
typedef struct Registry Registry;

/* Size of Hash Table with all the vehicles*/
#define VEHICLESTABLESIZE 1109

/* Maximum amount of characters of a license plate*/
#define LICENSEPLATEMAX 9

/* Handle license plate errors*/
#define WRONG_LENGTH 1
#define WRONG_DASHES 2
#define WRONG_PAIR 3
#define WRONG_CHARACTER 4
#define NO_NUMBERS_LETTERS 5
#define VALID_LICENSE_PLATE 6

/* Decide whether the vehicle entry is valid or not */
#define INV_PLATE -8
#define VEHICLE_IN_PARK -9
#define VEHICLE_NOT_FOUND -12
#define VALID_VEHICLE -13

/* Vehicle errors */
#define INV_PLATE_ERR ": invalid licence plate."
#define VEHICLE_IN_PARK_ERR ": invalid vehicle entry."
#define VEHICLE_NOT_FOUND_ERR ": no entries found in any parking."

/* Decide whether two license plates are the same */
#define SAME_LICENSE_PLATE 0

/*
Struct representing a vehicle:
Contains information about a vehicle, including its license plate, current
parking location, number of registries, pointers to its first and last registry
and a pointer to the next vehicle in the vehicles table.    
 */
typedef struct Vehicle {
    char plate[LICENSEPLATEMAX];
    Parking *parking;
    int registry_count;
    Registry *registry; // Registries from the vehicle
    Registry *last_registry;
    struct Vehicle *next;
} Vehicle;

/* Hash function that assigns a value to a license plate */
long vehicle_hash(char *str);

/* Initializes the vehicles table with NULL values and the size of VEHICLESTABLESIZE */
void init_vehicles_table(Vehicle **vehicles_table);

/* Auxiliar function that prints the vehicles table */
void print_vehicles_table(Vehicle **vehicles_table);

/* Insert a vehicle in the vehicles table */
void insert_vehicle(Vehicle **vehicles_table, Vehicle *v);

/* Returns a pointer to the vehicle by searching in the vehicles table or NULL if it is not in the system*/
Vehicle *search_vehicle(Vehicle **vehicles_table, char *license_plate);

/* Checks if a license plate is valid */
int is_valid_license_plate(char *license_plate);

/* Checks if the vehicle entry is valid and returns the parking index */
int check_entry(Parking *parkings, Date *current_date, Time *current_time, int *parkings_count,
                char *parking_name, char *license_plate, Date date, Time time, Vehicle *vehicle);

/* Handles the vehicle entry in the parking creating a registry and adding it to the array of vehicles in the parking */
void vehicle_entry(Vehicle **vehicles_table, Vehicle *vehicle, char *license_plate,
                   Parking *parking, Date date, Time time);

/* Checks if the vehicle exit is valid and returns the parking index */
int check_exit(Parking *parkings, Date *current_date, Time *current_time, int *parkings_count,
               char *parking_name, char *license_plate, Date date, Time time, Vehicle *vehicle);

/* Handles the vehicle exit from the parking updating the registry and removing it from the array of vehicles in the parking */
void vehicle_exit(Vehicle *vehicle, char *license_plate, Parking *parking, Date date, Time time);

/* Checks if the vehicle is valid and exists in the system and has registries */
int check_vehicle(Vehicle *vehicle, char *license_plate);

#endif