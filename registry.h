#ifndef REGISTRY_H
#define REGISTRY_H

#include "time.h"

#define INITIAL_REGISTRY_CAPACITY 10000

/* Struct representing a registry:
Contains information about a registry, including the parking, vehicle,
entry date and time, exit date and time, payment and pointers to the
next and previous registries from the vehicle
*/
typedef struct Registry {
    Parking *parking;
    Vehicle *vehicle;
    Date entry_date;
    Time entry_time;
    Date exit_date;
    Time exit_time;
    float payment;
    Registry *next;
    Registry *previous;
} Registry;

/* Creates a new registry for a vehicle entering a parking */
Registry *create_registry(Parking *parking, Vehicle *vehicle, Date date, Time time);

/* Updates the registry with the exit date and time and the billing */
Registry *update_registry(Parking *parking, Vehicle *vehicle, Date date, Time time);

/* Calculates the payment for a vehicle in a parking */
float calculate_payment(Parking *parking, Registry *registry);

/* Creates a array with all the registries from a vehicle sorted by the parking name and entry date and time */
Registry **sort_registry(Registry *registry, int registry_count);

/* Prints the whole registry of the vehicle sorted by parking name and entry date and time */
void print_registry(Vehicle *vehicle);

/* Checks if the billing of a parking is valid, returning the index of the parking in case it is */
int check_billing(Parking *parkings, Date *current_date, int *parkings_count, char *parking_name, Date date);

#endif