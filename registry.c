#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "system.h"
#include "parkings.h"
#include "vehicles.h"
#include "registry.h"
#include "time.h"

Registry *create_registry(Parking *parking, Vehicle *vehicle, Date date, Time time) {
    Registry *new_registry = malloc(sizeof(Registry));

    new_registry->parking = parking;
    new_registry->vehicle = vehicle;
    new_registry->entry_date = date;
    new_registry->entry_time = time;
    new_registry->payment = INVALID;
    new_registry->previous = NULL;
    new_registry->next = NULL;

    return new_registry;
}

Registry *update_registry(Parking *parking, Vehicle *vehicle, Date date, Time time) {
    Registry *registry = vehicle->last_registry;

    registry->exit_date = date;
    registry->exit_time = time;
    registry->payment = calculate_payment(parking, registry);

    return registry;
}

float calculate_payment(Parking *parking, Registry *registry) {
    int total_minutes, full_days, remaining_minutes, remaining_periods;
    float payment, daily_payment;

    total_minutes = calculate_time_difference(registry->entry_date, registry->entry_time,
                                              registry->exit_date, registry->exit_time);

    full_days = total_minutes / (24 * 60);
    remaining_minutes = total_minutes % (24 * 60);

    payment = full_days * parking->billing_rates[2]; // Payment for full days

    remaining_periods = remaining_minutes / 15;
    if (remaining_minutes % 15 != 0)
        remaining_periods++; // Rounds up

    if (remaining_periods <= 4) // Less than 1 Hour
        daily_payment = remaining_periods * parking->billing_rates[0];
    else {
        daily_payment = 4 * parking->billing_rates[0];
        remaining_periods -= 4;
        daily_payment += remaining_periods * parking->billing_rates[1];
    }

    if (daily_payment > parking->billing_rates[2])
        daily_payment = parking->billing_rates[2]; // Maximum payment per day

    payment += daily_payment;

    return payment;
}

Registry **sort_registry(Registry *registry, int registry_count) {
    Registry *current_registry, **registry_array = malloc(registry_count * sizeof(Registry *));
    int i, j, k, cmp;

    // Deep copy the parkings into an array allocating memory for it
    current_registry = registry;
    for (i = 0; i < registry_count; i++) {
        registry_array[i] = malloc(sizeof(Registry));
        memcpy(registry_array[i], current_registry, sizeof(Registry));
        current_registry = current_registry->next;
    }

    // Sort the new array using bubble sort algorithm
    for (j = 0; j < registry_count - OFFSET; j++)
        for (k = 0; k < registry_count - j - OFFSET; k++) {
            // Compare parking names
            cmp = strcmp(registry_array[k]->parking->name, registry_array[k + OFFSET]->parking->name);
            // If the parking names are the same, compare the entry date and time
            if (cmp > 0 || (cmp == 0 && calculate_time_difference(registry_array[k]->entry_date, registry_array[k]->entry_time, registry_array[k + 1]->entry_date, registry_array[k + 1]->entry_time) < 0)) {
                // Swap the parkings in case they're in the wrong order
                Registry *temp = registry_array[k];
                registry_array[k] = registry_array[k + OFFSET];
                registry_array[k + OFFSET] = temp;
            }
        }

    return registry_array;
}

void print_registry(Vehicle *vehicle) {
    Registry **sorted_registry;
    int i, j;

    if (vehicle->registry_count == 1) {
        if (vehicle->registry->payment == INVALID) // Vehicle is parked
            printf("%s %02d-%02d-%04d %02d:%02d\n", vehicle->registry->parking->name, vehicle->registry->entry_date.day, vehicle->registry->entry_date.month, vehicle->registry->entry_date.year, vehicle->registry->entry_time.hour, vehicle->registry->entry_time.minute);
        else
            printf("%s %02d-%02d-%04d %02d:%02d %02d-%02d-%04d %02d:%02d\n", vehicle->registry->parking->name, vehicle->registry->entry_date.day, vehicle->registry->entry_date.month, vehicle->registry->entry_date.year, vehicle->registry->entry_time.hour, vehicle->registry->entry_time.minute, vehicle->registry->exit_date.day, vehicle->registry->exit_date.month, vehicle->registry->exit_date.year, vehicle->registry->exit_time.hour, vehicle->registry->exit_time.minute);
    }

    else {
        sorted_registry = sort_registry(vehicle->registry, vehicle->registry_count);

        for (i = 0; i < vehicle->registry_count; i++) {
            if (sorted_registry[i]->payment == INVALID) // Vehicle is parked
                printf("%s %02d-%02d-%04d %02d:%02d\n", sorted_registry[i]->parking->name, sorted_registry[i]->entry_date.day, sorted_registry[i]->entry_date.month, sorted_registry[i]->entry_date.year, sorted_registry[i]->entry_time.hour, sorted_registry[i]->entry_time.minute);
            else
                printf("%s %02d-%02d-%04d %02d:%02d %02d-%02d-%04d %02d:%02d\n", sorted_registry[i]->parking->name, sorted_registry[i]->entry_date.day, sorted_registry[i]->entry_date.month, sorted_registry[i]->entry_date.year, sorted_registry[i]->entry_time.hour, sorted_registry[i]->entry_time.minute, sorted_registry[i]->exit_date.day, sorted_registry[i]->exit_date.month, sorted_registry[i]->exit_date.year, sorted_registry[i]->exit_time.hour, sorted_registry[i]->exit_time.minute);
        }

        // Free the memory allocated for the sorted parkings array
        for (j = 0; j < vehicle->registry_count; j++)
            free(sorted_registry[j]);
        free(sorted_registry);
    }
}

int check_billing(Parking *parkings, Date *current_date, int *parkings_count, char *parking_name, Date date) {
    int i, parking_index = INVALID;

    for (i = 0; i < *parkings_count; i++)
        if (strcmp(parkings[i].name, parking_name) == SAME_PARKING_NAME)
            parking_index = i;

    if (parking_index == INVALID)
        return INV_PARK;

    if (date.year != INVALID)
        if (is_valid_date(current_date, date) == FALSE)
            return INV_DATE;

    return parking_index;
}
