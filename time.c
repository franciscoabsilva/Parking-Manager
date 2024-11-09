#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "system.h"
#include "parkings.h"
#include "vehicles.h"
#include "registry.h"
#include "time.h"

void read_date(Date *date) {
    scanf("%d-%d-%d", &date->day, &date->month, &date->year);
}

void read_time(Time *time) {
    scanf("%d:%d", &time->hour, &time->minute);
}

int is_valid_date_time(Date *current_date, Time *current_time, Date date, Time time) {
    if (time.hour < 0 || time.hour > 23)
        return FALSE;
    if (time.minute < 0 || time.minute > 59)
        return FALSE;
    if (date.month < 1 || date.month > 12)
        return FALSE;
    if (date.day < 1 || date.day > 31)
        return FALSE;
    if (date.month == 2 && date.day > 28)
        return FALSE;
    if ((date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11) && date.day > 30)
        return FALSE;

    if (calculate_time_difference(*current_date, *current_time, date, time) < 0) // Date is in the future
        return FALSE;

    return TRUE;
}

int is_valid_date(Date *current_date, Date date) {
    if (date.month < 1 || date.month > 12)
        return FALSE;
    if (date.day < 1 || date.day > 31)
        return FALSE;
    if (date.month == 2 && date.day > 28)
        return FALSE;
    if ((date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11) && date.day > 30)
        return FALSE;

    if (date.year > current_date->year)
        return FALSE;
    if (date.year == current_date->year && date.month > current_date->month)
        return FALSE;
    if (date.year == current_date->year && date.month == current_date->month && date.day > current_date->day)
        return FALSE;

    return TRUE;
}

int calculate_time_difference(Date date1, Time time1, Date date2, Time time2) {
    int i, time1_minutes, time2_minutes, month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    time1_minutes = time1.minute + time1.hour * HOUR_MINUTES + (date1.day-OFFSET) * DAY_MINUTES;
    time2_minutes = time2.minute + time2.hour * HOUR_MINUTES + (date2.day-OFFSET) * DAY_MINUTES;

    for (i = 0; i < (date1.month-OFFSET); i++)
        time1_minutes += month_days[i] * DAY_MINUTES;
        
    for (i = 0; i < (date2.month-OFFSET); i++) 
        time2_minutes += month_days[i] * DAY_MINUTES;

    time1_minutes += (date1.year-OFFSET) * YEAR_MINUTES;
    time2_minutes += (date2.year-OFFSET) * YEAR_MINUTES;

    return time2_minutes - time1_minutes;
}

int most_recent_date(Date date1, Date date2) {
    if (date1.year < date2.year)
        return DATE2;
    if (date1.year > date2.year)
        return DATE1;

    if (date1.month < date2.month)
        return DATE2;
    if (date1.month > date2.month)
        return DATE1;

    if (date1.day < date2.day)
        return DATE2;
    if (date1.day > date2.day)
        return DATE1;

    return SAME_DATE;
}
