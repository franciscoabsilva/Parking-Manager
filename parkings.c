#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "system.h"
#include "parkings.h"
#include "vehicles.h"
#include "registry.h"
#include "time.h"

void read_parking_name(char *parking_name) {
    char command;

    if ((command = getchar()) == '"') {
        ungetc(command, stdin);
        scanf("\"%[^\"]\"", parking_name); // Read the parking name between quotes
    }
    else {
        ungetc(command, stdin);
        scanf("%s", parking_name);
    }
}

void list_parkings(Parking *parkings, int *parkings_count) {
    int i;

    for (i = 0; i < *parkings_count; i++)
        printf("%s %d %d\n", parkings[i].name, parkings[i].capacity, parkings[i].empty_spots);
}

int check_parking(Parking *parkings, int *parkings_count, char *parking_name,
                  int capacity, float billing_rates[3]) {
    int i;
    
    for (i = 0; i < *parkings_count; i++)
        if (strcmp(parkings[i].name, parking_name) == SAME_PARKING_NAME)
            return DUP_PARK;
        
    if (capacity < 1)
        return INV_CAP;

    if (billing_rates[0] <= 0 || billing_rates[1] <= 0 || billing_rates[2] <= 0)
        return INV_COST;
    if (billing_rates[0] > billing_rates[1] || billing_rates[1] > billing_rates[2])
        return INV_COST;

    if (*parkings_count == PARKINGSMAX)
        return MAX_PARK;

    return VALID_PARK;  
}

void create_parking(Parking *parkings, int *parkings_count, char *parking_name,
                    int capacity, float billing_rates[3]) {
    Parking *parking = &parkings[*parkings_count];

    parking->name = malloc(sizeof(char) * (strlen(parking_name) + 1));
    strcpy(parking->name, parking_name);

    parking->capacity = capacity;
    parking->empty_spots = capacity;

    parking->billing_rates[0] = billing_rates[0];
    parking->billing_rates[1] = billing_rates[1];
    parking->billing_rates[2] = billing_rates[2];

    parking->vehicles = malloc(capacity * sizeof(Vehicle *));

    parking->registry = malloc(INITIAL_REGISTRY_CAPACITY * sizeof(Registry *));
    parking->registry_capacity = INITIAL_REGISTRY_CAPACITY;
    parking->registry_count = 0;
    
    *parkings_count += 1;
}

void add_registry_to_parking(Parking *parking, Registry *registry) {
    // In case the registries array is full double his capacity
    if (parking->registry_count == parking->registry_capacity) {
        parking->registry_capacity *= 2;
        parking->registry = realloc(parking->registry, parking->registry_capacity * sizeof(Registry *));
    }

    parking->registry[parking->registry_count] = registry;
    parking->registry_count++;
}

void add_vehicle_to_parking(Vehicle *vehicle, Parking *parking) {
    int cars_inside = parking->capacity - parking->empty_spots;

    parking->vehicles[cars_inside] = vehicle; // Add vehicle to the end of the list
}

void remove_vehicle_from_parking(Vehicle *vehicle, Parking *parking) {
    int vehicle_index = INVALID, i, cars_inside = parking->capacity - parking->empty_spots;
    
    // Find the vehicle in the list of vehicles inside the parking
    for (i = 0; i < cars_inside; i++)
        if (parking->vehicles[i] == vehicle) {
            vehicle_index = i;
            break;
        }
    
    // Move all the vehicles after the removed vehicle to the left
    for (int i = vehicle_index; i < cars_inside - OFFSET; i++)
        parking->vehicles[i] = parking->vehicles[i + 1];
}

void print_parking_billing(Parking *parking) {
    Registry *registry;
    Date current_date = {INVALID, INVALID, INVALID};
    Date last_date = {INVALID, INVALID, INVALID};
    int registry_count = parking->registry_count;
    float billing = 0, last_billing;

    for (int i = 0; i < registry_count; i++) {
        registry = parking->registry[i];

        if (most_recent_date(registry->exit_date, current_date) == SAME_DATE)
            billing += registry->payment;
            
        else { // When the date changes
            last_date = current_date;
            last_billing = billing;
            billing = registry->payment;
            current_date = registry->exit_date;
            if (last_date.year != INVALID)
                printf("%02d-%02d-%04d %.2f\n", last_date.day, last_date.month, last_date.year, last_billing);
        }
    }

    // Print the billing for the last date
    if (current_date.year != INVALID)
        printf("%02d-%02d-%04d %.2f\n", current_date.day, current_date.month, current_date.year, billing);
}

void print_parking_billing_date(Parking *parking, Date date) {
    Registry *registry;
    int i;

    for (i = 0; i < parking->registry_count; i++) {
        registry = parking->registry[i];

        if (most_recent_date(registry->exit_date, date) == SAME_DATE)
            printf("%s %02d:%02d %.2f\n", registry->vehicle->plate, registry->exit_time.hour,
                   registry->exit_time.minute, registry->payment);
        else if (most_recent_date(registry->exit_date, date) == DATE1)
            break; // After the date stops searching because they're sorted by exit date
    }
}

int check_parking_name(Parking *parkings, int *parkings_count, char *parking_name) {
    int i;

    for (i = 0; i < *parkings_count; i++)
        if (strcmp(parkings[i].name, parking_name) == SAME_PARKING_NAME)
            return i;

    return INV_PARK;
}

void remove_parking_registries(Parking *parking) {
    Registry *registry, *last_vehicle_registry, *next_vehicle_registry;

    for (int i = 0; i < parking->registry_count; i++) {
        registry = parking->registry[i];

        last_vehicle_registry = registry->previous;
        next_vehicle_registry = registry->next;

        // If the registry is the first of the vehicle
        if (last_vehicle_registry == NULL)
            registry->vehicle->registry = next_vehicle_registry;
        else
            last_vehicle_registry->next = next_vehicle_registry;

        // If the registry is the last of the vehicle
        if (next_vehicle_registry == NULL) 
            registry->vehicle->last_registry = last_vehicle_registry;
        else
            next_vehicle_registry->previous = last_vehicle_registry;

        registry->vehicle->registry_count--;

        free(registry);
    }
    free(parking->registry);
}

void remove_vehicules_from_parking(Parking *parking) {
    Vehicle *vehicle;
    int i, cars_inside = parking->capacity - parking->empty_spots;

    for (i = 0; i < cars_inside; i++) {
        vehicle = parking->vehicles[i];
        vehicle->last_registry = vehicle->last_registry->previous;
        vehicle->registry_count--;
        vehicle->parking = NULL;
    }

    free(parking->vehicles);
}

void remove_parking(Parking *parkings, int *parkings_count, int parking_index) {
    Parking *parking = &parkings[parking_index];
    int i;

    remove_parking_registries(parking);
    remove_vehicules_from_parking(parking);
    free(parking->name);

    // Move all the parkings after the removed parking to the left
    for (i = parking_index; i < *parkings_count - OFFSET; i++)
        parkings[i] = parkings[i + 1];

    *parkings_count -= 1;
}

Parking **sort_parkings(Parking *parkings, int *parkings_count) {
    Parking **parking_array = malloc(*parkings_count * sizeof(Parking *));
    int i, j, k;

    // Deep copy the parkings into an array allocating memory for it
    for (i = 0; i < *parkings_count; i++) {
        parking_array[i] = malloc(sizeof(Parking));
        memcpy(parking_array[i], &parkings[i], sizeof(Parking));
    }

    // Sort the new array using bubble sort algorithm
    for (j = 0; j < *parkings_count - OFFSET; j++)
        for (k = 0; k < *parkings_count - j - OFFSET; k++)
            // Compare parking names
            if (strcmp(parking_array[k]->name, parking_array[k + OFFSET]->name) > 0) {
                // Swap the parkings in case they're in the wrong order
                Parking *temp = parking_array[k];
                parking_array[k] = parking_array[k + OFFSET];
                parking_array[k + OFFSET] = temp;
            }

    return parking_array;
}

void print_remaining_parkings(Parking *parkings, int *parkings_count) {
    Parking **sorted_parkings;
    int i, j;

    if (*parkings_count == 1)
        printf("%s\n", parkings[0].name);

    else {
        sorted_parkings = sort_parkings(parkings, parkings_count);

        for (i = 0; i < *parkings_count; i++)
            printf("%s\n", sorted_parkings[i]->name);

        // Free the memory allocated for the sorted parkings array
        for (j = 0; j < *parkings_count; j++)
            free(sorted_parkings[j]);
        free(sorted_parkings);
    }
}
