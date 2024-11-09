#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "system.h"
#include "parkings.h"
#include "vehicles.h"
#include "registry.h"
#include "time.h"

/* Free all the memory allocated */
void command_q(Parking *parkings, Vehicle **vehicles_table, int *parkings_count) {
    Parking *parking;
    Vehicle *current_vehicle, *next_vehicle;
    int i;

    // Frees all the information related to parkings
    for (i = 0; i < *parkings_count; i++) {
        parking = &parkings[i];
        free(parking->name);
        remove_parking_registries(parking);
    }
    
    // Frees all the information related to vehicles
    for (i = 0; i < VEHICLESTABLESIZE; i++) {
        current_vehicle = vehicles_table[i];
        while (current_vehicle != NULL) {
            if (current_vehicle->parking != NULL)
                free(current_vehicle->registry);
            next_vehicle = current_vehicle->next;
            free(current_vehicle);
            current_vehicle = next_vehicle;
        }
    }
}

/* Create parking or list all the parkings */
void command_p(Parking *parkings, int *parkings_count) {
    char *parking_name = malloc(INPUTLEN * sizeof(char));;
    int capacity, check;
    float billing_rates[3];

    // List parkings
    if (getchar() == '\n')
        list_parkings(parkings, parkings_count);

    // Create parking
    else {
        read_parking_name(parking_name);
        scanf("%d %f %f %f", &capacity, &billing_rates[0], &billing_rates[1], &billing_rates[2]);
        
        check = check_parking(parkings, parkings_count, parking_name, capacity, billing_rates);
        
        switch(check) {
            case DUP_PARK:
                printf("%s%s\n", parking_name, DUP_PARK_ERR);
                break;
            case INV_CAP:
                printf("%d%s\n", capacity, INV_CAP_ERR);
                break;
            case INV_COST:
                printf("%s\n", INV_COST_ERR);
                break;
            case MAX_PARK:
                printf("%s\n", MAX_PARK_ERR);
                break;
            case VALID_PARK:
                create_parking(parkings, parkings_count, parking_name, capacity, billing_rates);
                break;
        }
    }
    free(parking_name);
}

/* Register vehicle entry */
void command_e(Parking *parkings, Vehicle **vehicles_table,
               int *parkings_count, Date *current_date, Time *current_time) {
    Parking *parking;
    Vehicle *vehicle;
    Date date;
    Time time;
    char *parking_name = malloc(INPUTLEN * sizeof(char)), license_plate[LICENSEPLATEMAX];
    int parking_index;

    getchar();
    read_parking_name(parking_name);
    scanf("%s", license_plate);
    read_date(&date);
    read_time(&time);

    vehicle = search_vehicle(vehicles_table, license_plate);
    parking_index = check_entry(parkings, current_date, current_time, parkings_count,
                                parking_name, license_plate, date, time, vehicle);

    switch (parking_index) {
        case INV_PARK:
            printf("%s%s\n", parking_name, INV_PARK_ERR);
            break;
        case FULL_PARK:
            printf("%s%s\n", parking_name, FULL_PARK_ERR);
            break;
        case INV_PLATE:
            printf("%s%s\n", license_plate, INV_PLATE_ERR);
            break;
        case VEHICLE_IN_PARK:
            printf("%s%s\n", license_plate, VEHICLE_IN_PARK_ERR);
            break;
        case INV_DATE:
            printf("%s\n", INV_DATE_ERR);
            break;
        default:
            parking = &parkings[parking_index];
            vehicle_entry(vehicles_table, vehicle, license_plate, parking, date, time);
            *current_date = date;
            *current_time = time;
            break;
    }
    free(parking_name);
}

/* Register vehicle exit */
void command_s(Parking *parkings, Vehicle **vehicles_table,
               int *parkings_count, Date *current_date, Time *current_time) {
    Parking *parking;
    Vehicle *vehicle;
    Date date;
    Time time;
    char *parking_name = malloc(INPUTLEN * sizeof(char)), license_plate[LICENSEPLATEMAX];
    int parking_index;

    getchar();
    read_parking_name(parking_name);
    scanf("%s", license_plate);
    read_date(&date);
    read_time(&time);

    vehicle = search_vehicle(vehicles_table, license_plate);
    parking_index = check_exit(parkings, current_date, current_time, parkings_count,
                               parking_name, license_plate, date, time, vehicle);
    
    switch (parking_index) {
        case INV_PARK:
            printf("%s%s\n", parking_name, INV_PARK_ERR);
            break;
        case INV_PLATE:
            printf("%s%s\n", license_plate, INV_PLATE_ERR);
            break;
        case INV_EXIT:
            printf("%s%s\n", license_plate, INV_EXIT_ERR);
            break;
        case INV_DATE:
            printf("%s\n", INV_DATE_ERR);
            break;
        default:
            parking = &parkings[parking_index];
            vehicle_exit(vehicle, license_plate, parking, date, time);
            *current_date = date;
            *current_time = time;
            break;
    }
    free(parking_name);
}

/* List all entries and exits of a vehicle */
void command_v(Vehicle **vehicles_table) {
    Vehicle *vehicle;
    char license_plate[LICENSEPLATEMAX];
    int check;

    getchar();
    scanf("%s", license_plate);

    vehicle = search_vehicle(vehicles_table, license_plate);
    check = check_vehicle(vehicle, license_plate);

    switch(check) {
        case INV_PLATE:
            printf("%s%s\n", license_plate, INV_PLATE_ERR);
            break;
        case VEHICLE_NOT_FOUND:
            printf("%s%s\n", license_plate, VEHICLE_NOT_FOUND_ERR);
            break;
        case VALID_VEHICLE:
            print_registry(vehicle);
        break;
    }
}

/* Print parking billing or daily billing */
void command_f(Parking *parkings, int *parkings_count, Date *current_date) {
    Parking *parking;
    Date date;
    char *parking_name = malloc(INPUTLEN * sizeof(char));
    int parking_index;

    getchar();
    read_parking_name(parking_name);
    if (getchar() == '\n')
        date.year = INVALID;
    else
        read_date(&date);

    parking_index = check_billing(parkings, current_date, parkings_count, parking_name, date);
    
    switch(parking_index) {
        case INV_PARK:
            printf("%s%s\n", parking_name, INV_PARK_ERR);
            break;
        case INV_DATE:
            printf("%s\n", INV_DATE_ERR);
            break;
        default:
            parking = &parkings[parking_index];
            // Print parking billing with 1 argument
            if (date.year == INVALID)
                if (parking->registry_count == 0)
                    printf("\n");
                else
                    print_parking_billing(parking);
            // Print parking billing with 2 arguments
            else
                print_parking_billing_date(parking, date);
            break;
    }
    free(parking_name);
}

/* Remove parking */
void command_r(Parking *parkings, int *parkings_count) {
    char *parking_name = malloc(INPUTLEN * sizeof(char));
    int parking_index;

    getchar();
    read_parking_name(parking_name);

    parking_index = check_parking_name(parkings, parkings_count, parking_name);
    switch (parking_index) {
        case INV_PARK:
            printf("%s%s\n", parking_name, INV_PARK_ERR);
            break;
        default:
            remove_parking(parkings, parkings_count, parking_index); 
            print_remaining_parkings(parkings, parkings_count);
            break;
    }
    free(parking_name);
}

/* Main function that initializes the parkings array,
the vehicles table and the date and reads the commands*/
int main() {
    Parking parkings[PARKINGSMAX]; // Array with all the parkings
    Vehicle *vehicles_table[VEHICLESTABLESIZE]; // Hash table with all the vehicles
    init_vehicles_table(vehicles_table);
    char command;
    int parkings_count = 0;
    Date current_date = {0, 0, 0};
    Time current_time = {0, 0};

    while (1) {
        scanf("%c", &command);

        switch(command){
            case 'q':
                command_q(parkings, vehicles_table, &parkings_count);
                exit(0);
            case 'p':
                command_p(parkings, &parkings_count);
                break;
            case 'e':
                command_e(parkings, vehicles_table, &parkings_count, &current_date, &current_time);
                break;
            case 's':
                command_s(parkings, vehicles_table, &parkings_count, &current_date, &current_time);
                break;
            case 'v':
                command_v(vehicles_table);
                break;
            case 'f':
                command_f(parkings, &parkings_count, &current_date);
                break;
            case 'r':
                command_r(parkings, &parkings_count);
                break;
        }
    }
    return 0;
}
