#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "system.h"
#include "parkings.h"
#include "vehicles.h"
#include "registry.h"
#include "time.h"

long vehicle_hash(char *str) {
    long hash_value = 0;
    int i;

    for (i = 0; i < LICENSEPLATEMAX-OFFSET; i++)
        hash_value = (hash_value * 31) + str[i];

    return hash_value % VEHICLESTABLESIZE; // Makes it fit within the table
}

void init_vehicles_table(Vehicle **vehicles_table) {
    int i;

    for (i = 0; i < VEHICLESTABLESIZE; i++)
        vehicles_table[i] = NULL;
}

void print_vehicles_table(Vehicle **vehicles_table) {
    for (int i = 0; i < VEHICLESTABLESIZE; i++) {
        if (vehicles_table[i] == NULL)
            printf("%d NULL", i);
        else{
            Vehicle *current = vehicles_table[i];
            printf("%d", i);
            while (current != NULL) {
                printf(" %s", current->plate);
                current = current->next;
            }
        }
        printf("\n");
    }
}

void insert_vehicle(Vehicle **vehicles_table, Vehicle *v) {
    int index = vehicle_hash(v->plate);
    Vehicle *current_vehicle = vehicles_table[index];

    if (current_vehicle == NULL) // If there's no vehicle at this index, set the new vehicle as the head
        vehicles_table[index] = v;
    else {
        while (current_vehicle->next != NULL)
            current_vehicle = current_vehicle->next;
        current_vehicle->next = v; // append to the end of the linked list
    }
}

Vehicle *search_vehicle(Vehicle **vehicles_table, char *license_plate) {
    int index = vehicle_hash(license_plate);
    Vehicle *current_vehicle = vehicles_table[index];

    while (current_vehicle != NULL) {
        if (strcmp(current_vehicle->plate, license_plate) == SAME_LICENSE_PLATE)
            return current_vehicle;
        current_vehicle = current_vehicle->next;
    }

    return NULL;
}

int is_valid_license_plate(char *license_plate) {
    int i, numbers = 0, letters = 0;
    
    if (strlen(license_plate) != LICENSEPLATEMAX-OFFSET)
        return WRONG_LENGTH;

    if (license_plate[2] != '-' || license_plate[5] != '-')
        return WRONG_DASHES;

    for (i = 0; i < LICENSEPLATEMAX; i+=3) {
        if isdigit(license_plate[i]) {
            numbers++;
            if (!isdigit(license_plate[i+1]))
                return WRONG_PAIR; // Case digit and not digit
        }
        else if isupper(license_plate[i]) {
            letters++;
            if (!isupper(license_plate[i+1]))
                return WRONG_PAIR; // Case uppercase letter and not uppercase letter
        }
        else
            return WRONG_CHARACTER;
    }

    if (numbers == 0 || letters == 0) // Needs to have numbers and letters
        return NO_NUMBERS_LETTERS;

    return VALID_LICENSE_PLATE;
}

int check_entry(Parking *parkings, Date *current_date, Time *current_time, int *parkings_count, 
                char *parking_name, char *license_plate, Date date, Time time, Vehicle *vehicle) {
    int i, parking_index = INVALID;

    for (i = 0; i < *parkings_count; i++)
        if (strcmp(parkings[i].name, parking_name) == SAME_PARKING_NAME)
            parking_index = i;

    if (parking_index == INVALID)
        return INV_PARK;

    if (parkings[parking_index].empty_spots == 0)
        return FULL_PARK;

    if (is_valid_license_plate(license_plate) != VALID_LICENSE_PLATE)
       return INV_PLATE;

    if (vehicle != NULL)
        if (vehicle->parking != NULL)
            return VEHICLE_IN_PARK;

    if (is_valid_date_time(current_date, current_time, date, time) == FALSE)
        return INV_DATE;

    return parking_index;
}

void vehicle_entry(Vehicle **vehicles_table, Vehicle *vehicle, char *license_plate,
                   Parking *parking, Date date, Time time) {
    Registry *new_registry;
    Vehicle *new_vehicle;

    if (vehicle == NULL) { // In case the car doesn't exist in the system
        new_vehicle = malloc(sizeof(Vehicle));

        strcpy(new_vehicle->plate, license_plate);
        new_vehicle->parking = parking;

        new_registry = create_registry(parking, new_vehicle, date, time);
        new_vehicle->last_registry = new_registry;
        new_vehicle->registry = new_registry;
        new_vehicle->registry_count = 1;

        new_vehicle->next = NULL;

        insert_vehicle(vehicles_table, new_vehicle);
        add_vehicle_to_parking(new_vehicle, parking);
    }
    else {
        vehicle->parking = parking;

        if (vehicle->registry_count == 0) { // In case the car exists but has no registries
            new_registry = create_registry(parking, vehicle, date, time);
            vehicle->registry = new_registry;
            vehicle->last_registry = new_registry;
            vehicle->registry_count = 1;
        }
        else {
            new_registry = create_registry(parking, vehicle, date, time);
            vehicle->last_registry->next = new_registry;
            new_registry->previous = vehicle->last_registry;
            vehicle->last_registry = new_registry;
            vehicle->registry_count++;
        }

        add_vehicle_to_parking(vehicle, parking);
    }

    parking->empty_spots--;

    printf("%s %d\n", parking->name, parking->empty_spots);
}

int check_exit(Parking *parkings, Date *current_date, Time *current_time, int *parkings_count,
               char *parking_name, char *license_plate, Date date, Time time, Vehicle *vehicle) {
    int i, parking_index = INVALID;

    for (i = 0; i < *parkings_count; i++)
        if (strcmp(parkings[i].name, parking_name) == SAME_PARKING_NAME)
            parking_index = i;

    if (parking_index == INVALID)
        return INV_PARK;

    if (is_valid_license_plate(license_plate) != VALID_LICENSE_PLATE)
       return INV_PLATE;

    if (vehicle == NULL)
        return INV_EXIT;
    if (vehicle->parking != &parkings[parking_index])
        return INV_EXIT;

    if (is_valid_date_time(current_date, current_time, date, time) == FALSE)
        return INV_DATE;

    return parking_index;
}

void vehicle_exit(Vehicle *vehicle, char *license_plate, Parking *parking, Date date, Time time) {
    Registry *registry;

    vehicle->parking = NULL;

    registry = update_registry(parking, vehicle, date, time);
    add_registry_to_parking(parking, registry);

    remove_vehicle_from_parking(vehicle, parking);

    parking->empty_spots++;

    printf("%s %02d-%02d-%02d %02d:%02d %02d-%02d-%02d %02d:%02d %.2f\n", license_plate,
            registry->entry_date.day, registry->entry_date.month, registry->entry_date.year,
            registry->entry_time.hour, registry->entry_time.minute, date.day, date.month,
            date.year, time.hour, time.minute, registry->payment);
}

int check_vehicle(Vehicle *vehicle, char *license_plate) {
    if (is_valid_license_plate(license_plate) != VALID_LICENSE_PLATE)
       return INV_PLATE;

    if (vehicle == NULL || vehicle->registry_count == 0)
        return VEHICLE_NOT_FOUND;

    return VALID_VEHICLE;
}
