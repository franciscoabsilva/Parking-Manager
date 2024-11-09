#ifndef TIME_H
#define TIME_H

/* Maximum amount of characters of a date*/
#define DATEMAX 11
/* Maximum amount of characters of the time*/
#define TIMEMAX 6

/* Minutes in a year, month and day*/
#define YEAR_MINUTES 525600
#define DAY_MINUTES 1440
#define HOUR_MINUTES 60

/* Decide whether the date is valid or not*/
#define INV_DATE -10

/* Date errors*/
#define INV_DATE_ERR "invalid date."

/* Decide which of two dates is the most recent */
#define SAME_DATE 0
#define DATE1 1
#define DATE2 -1

/*
Struct representing a date:
Contains information about a date, including its day, month and year
 */
typedef struct Date {
    int day;
    int month;
    int year;
} Date;

/*
Struct representing a time:
Contains information about a time, including its hour and minute
 */
typedef struct Time {
    int hour;
    int minute;
} Time;

/* Reads the date into the date variable */
void read_date(Date *date);

/* Reads the time into the time variable */
void read_time(Time *time);

/* Checks if the date and the time are valid */
int is_valid_date_time(Date *current_date, Time *current_time, Date date, Time time);

/* Checks if the date is valid */
int is_valid_date(Date *current_date, Date date);

/* Calculates the difference in minutes between two dates comparing boths dates to 01/01/0001 00:00 */
int calculate_time_difference(Date date1, Time time1, Date date2, Time time2);

/* Finds the most recent date between two dates */
int most_recent_date(Date date1, Date date2);

#endif