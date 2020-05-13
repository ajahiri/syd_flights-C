/*******************************************************************************
 * 48430 Fundamentals of C Programming - Assignment 2
 * Name: Arian Jahiri
 * Student ID: 13348469
 * Date of submission: 14/05/2020
 * A brief statement on what you could achieve (less than 50 words):
 * I was able to achieve all the required functionality set in the assignment
 * description including the checking of flight code format and handling
 * extra-long input.
 * A brief statement on what you could NOT achieve (less than 50 words):
 * Nothing in terms of what was asked of us in description of assessment.
 *
*******************************************************************************/

/*******************************************************************************
 * List header files - do NOT use any other header files. Note that stdlib.h is
 * included in case you want to use any of the functions in there. However the
 * task can be achieved with stdio.h and string.h only.
*******************************************************************************/
#include <stdio.h>   /* scanf, printf, fscanf, fprintf, fopen, fclose, getchar,
                      * fgets                                                 */
#include <string.h>  /* strncmp, strlen, strcpy, strcspn */

/*******************************************************************************
 * List preprocessing directives - you may define your own.
*******************************************************************************/
#define MAX_FLIGHTCODE_LEN 6
#define MAX_CITYCODE_LEN 3
#define MAX_NUM_FLIGHTS 5
#define DB_NAME "database"

/*******************************************************************************
 * List structs - you may define struct date_time and struct flight only. Each
 * struct definition should have only the fields mentioned in the assignment
 * description.
*******************************************************************************/
struct date_time {
   int month;
   int day;
   int hour;
   int minute;
};
typedef struct date_time date_time_t;

struct flight {
   char flightcode[MAX_FLIGHTCODE_LEN+1];
   date_time_t departure_dt;
   char arrival_city[MAX_CITYCODE_LEN+1];
   date_time_t arrival_dt;
};
typedef struct flight flight_t;

/*******************************************************************************
 * Function prototypes - do NOT change the given prototypes. However you may
 * define your own functions if required.
*******************************************************************************/
void print_menu (void);
date_time_t getDate (void); /* Defined by me to avoid code duplication */
int add_flight (flight_t flights[], size_t flights_size, int* flightCounter);
void display_flight (flight_t flights[], size_t flights_size, 
                     int flightCounter);
void print_flight (flight_t* flightp);
void save_flights (const flight_t flights[], size_t flights_size,
                   int flightCounter);
int load_flights (flight_t flights[], size_t flights_size);
int checkFlightCode (const char flightCode[], size_t flightCodeLen);

/*******************************************************************************
 * Main
*******************************************************************************/
int main(void)
{
   int menuInput; /* Keep track of our menu input */
   int flightCounter = 0; /* Keep track of what flight we're upto */
   int* flightCounter_p = &flightCounter; /* flightCounter pointer */
   flight_t flights[MAX_NUM_FLIGHTS]; /* Initialise array for flight storage  */

   /* Below is for passing flights array to functions. */
   size_t flights_size = (sizeof(flights)/sizeof(flights[0])); 

   int loadedFlights = 0; /* Used to track loaded flights. */
   int done = 0; /* Means to break the main loop. */

   /* The main loop of the program, keep displaying menu until exit input (5).*/
   while (!done) {
      print_menu();
      scanf("%d", &menuInput);
      switch(menuInput) {
         case 1:
             /* Catch the newline char so that fgets doesn't skip.  */
            getchar();
            add_flight(flights, flights_size, flightCounter_p);
            break;
         case 2:
            display_flight(flights, flights_size, flightCounter);
            break;
         case 3:
            save_flights(flights, flights_size, flightCounter);
            break;
         case 4:
            loadedFlights = load_flights(flights, flights_size);
            /* Don't set the counter if loading failed. i.e if 0 is returned  *
             * form the load_flights function.                                */
            if (loadedFlights >= 1) {
               flightCounter = loadedFlights;
            }
            break;
         case 5:
            done = 1;
            break;
         default:
            printf("Invalid choice\n");
      }
   }

   return 0;
}

/*******************************************************************************
 * This function prints the initial menu with all instructions on how to use
 * this program.
 * inputs:
 * - none
 * outputs:
 * - none
*******************************************************************************/
void print_menu (void)
{
   printf("\n"
   "1. add a flight\n"
   "2. display all flights to a destination\n"
   "3. save the flights to the database file\n"
   "4. load the flights from the database file\n"
   "5. exit the program\n"
   "Enter choice (number between 1-5)>\n");
}

/*******************************************************************************
 * This function prompts for the date of flight from user, does validation
 * checks, then returns the date as a date_time_t type.
 * inputs:
 * - none
 * outputs:
 * - date_time_t date: returns the date entered by user.
*******************************************************************************/
date_time_t getDate(void) 
{
   int inpMonth, inpDay, inpHour, inpMin;
   date_time_t date;
   /* Inifnite loop only break on valid input */
   /* Store date attributes separately so we can test for bad input. */
   printf("Enter month, date, hour and minute separated by spaces>\n");

   scanf("%d %d %d %d", &inpMonth, &inpDay, &inpHour, &inpMin);

   if (inpMonth >= 1 && inpMonth <= 12 && inpDay >= 1 && inpDay <= 31 &&
         inpHour >= 0 && inpHour <= 23 && inpMin >= 0 && inpMin <= 59) {
      date.month = inpMonth;
      date.day = inpDay;
      date.hour = inpHour;
      date.minute = inpMin;
      return date;
   } else {
      printf("Invalid input\n");
      return getDate();
   }
};

/*******************************************************************************
 * This function will add a new flight to the flights array by asking a set
 * of questions to the user.
 * inputs:
 * - flights array, we want to change the array within the function so we will
 * NOT pass the array as const.
 * - size_t flights_size: size of flights[] array common pattern when passing
 * arrays to functions in C.
 * - flightCounter_p: pointer to int that tracks which flight we're upto from
 *                  main().
 * outputs:
 * - none
*******************************************************************************/
int add_flight(flight_t flights[], size_t flights_size, int* flightCounter_p) 
{
   /* Initialize useful vars and inputs. */
   char flightCodeInp[MAX_FLIGHTCODE_LEN+1];
   char arrivalCode[MAX_CITYCODE_LEN+1];
   date_time_t departureDate, arrivalDate;
   int addPosition = *flightCounter_p;


   /* Make sure we are not adding more than 5 flights. */
   if (addPosition >= 5) {
      printf("Cannot add more flights - memory full\n");
      return 0;
   }

   int passedFlightCode = 0; /* True (1) when passed otherwise False (0) */

   /* Loop to ensure flightCode is proper format. */
   while(!passedFlightCode) 
   {
      /* --------------------GET FLIGHT CODE--------------------     */
      /* Get all the info needed for a single flight from the user   */
      printf("Enter flight code>\n");

      /* Moved from scanf to using fgets as scanf was not handling spaces     *
       * as I would have liked. We are using 1000 so that large inputs don't  *
       * trigger weird behaviour with 'Invalid input' printing multiple times.*/
      fgets(flightCodeInp, 1000, stdin); /* Large buffer handles long input */
      /*strncpy(flightCodeInp, flightCodeLong, MAX_FLIGHTCODE_LEN);*/
      if (strlen(flightCodeInp) <= MAX_FLIGHTCODE_LEN + 1) {
         flightCodeInp[MAX_FLIGHTCODE_LEN] = '\0'; /* Ensure null terminated */
         if (checkFlightCode(flightCodeInp, strlen(flightCodeInp)) == 1) {
            /* Remove newline char from flightCodeInp from fgets() */
            flightCodeInp[strcspn(flightCodeInp, "\n")] = 0;
            /* Flight code passes validation, start constructing  *
             * flight in array.                                   */
            strcpy(flights[addPosition].flightcode, flightCodeInp);
            passedFlightCode = 1;
         } else {
            /* Flight code failed validation, start from beginning. */
            printf("Invalid input\n");
         }
      } else {
         /* Flight code failed validation, start from beginning. */
         printf("Invalid input\n");
      }
   }

   passedFlightCode = 0;

   /* --------------------GET DEPARTURE INFO-------------------- */
   printf("Enter departure info for the flight leaving SYD.\n");
   departureDate = getDate();
   flights[addPosition].departure_dt = departureDate;

   /* --------------------GET ARRIVAL CODE-------------------- */
   printf("Enter arrival city code>\n");
   char arrivalCodeLong[1000];
   /* scanf("%s", arrivalCode); */
   getchar(); /* Eat the newline \n */
   fgets(arrivalCodeLong, 1000, stdin);
   strncpy(arrivalCode, arrivalCodeLong, 4);
   arrivalCode[3] = '\0'; /*Made sure code is in proper string form.*/
   strcpy(flights[addPosition].arrival_city, arrivalCode);

   /* --------------------ARRIVAL INFO-------------------- */
   printf("Enter arrival info.\n");
   arrivalDate = getDate();
   flights[addPosition].arrival_dt = arrivalDate;
   *flightCounter_p += 1; /* Increment amt of flights */
   return 1; /* Successfully added flight, return 1 */
}

/*******************************************************************************
 * This function will simply print flight info for display_flight function.
 * This was implemented as the printing needed to be done in more than one place
 * in the code. Hence avoiding code duplication.
 * inputs:
 * - flight_t* flight_p: a pointer to particular flight object. Would've used an
 * object itself but there no need to make a local copy of the flight object.
 * outputs:
 * - none
*******************************************************************************/
void print_flight(flight_t* flight_p) 
{
   /* Simply print all flight info with leading zeros (if needed) */
   printf("%-6s SYD %02d-%02d %02d:%02d %-3s %02d-%02d %02d:%02d\n",
      flight_p->flightcode, flight_p->departure_dt.month,
      flight_p->departure_dt.day, flight_p->departure_dt.hour,
      flight_p->departure_dt.minute, flight_p->arrival_city,
      flight_p->arrival_dt.month, flight_p->arrival_dt.day,
      flight_p->arrival_dt.hour, flight_p->arrival_dt.minute);
}

/*******************************************************************************
 * This function will display a flight depending on the destination input.
 * Func will ask for arrival city code
 * inputs:
 * - flights array, we want to change the array within the function so we will
 * not pass the array as const.
 * - size_t flights_size: size of flights[] array common pattern when passing
 * arrays to functions in C.
 * outputs:
 * - none
*******************************************************************************/
void display_flight (flight_t flights[], size_t flights_size, int flightCounter)
{
   char arrivalCode[MAX_CITYCODE_LEN+1];
   int printNo = 0, flightMatches = 0; /* Keep track of amt of prints */
   int i; /* Just an iterator for looping */
   printf("Enter arrival city code or enter * to show all destinations>\n");
   scanf("%3s", arrivalCode);

   /* In both cases, we don't want to bother printing anything if there  *
    * are no recorded flights.                                           */

   /* Reason we are using flightCounter oppose to flights_size is that we   *
    * don't want to iterate over the whole array if part of it may be empty.*/
   /* Need to know if there are any matches before we proceed */
   for (i = 0; i<flightCounter; i++) {
      /* Search for flights with particular code using strcmp and track amt */
      if (strcmp(flights[i].arrival_city, arrivalCode) == 0) flightMatches++;
   }

   if (!strcmp(arrivalCode, "*") && flightCounter > 0) 
   {
      /* This will display all flights */
      printf("Flight Origin          Destination\n");
      printf("------ --------------- ---------------\n");
      for (i = 0; i<flightCounter; i++) {
         print_flight(&flights[i]);
         printNo++; /* Increment to track in case no prints were made. */
      }
   } else if (flightMatches == 0) {
         /* If no matches were found, i.e no flights found, let the user know.*/
         printf("No flights\n");
      } else if (flightCounter > 0) {
         printf("Flight Origin          Destination\n");
         printf("------ --------------- ---------------\n");
         for (i = 0; i<flightCounter; i++) {
            /* Only print the flight if the arrival code matches the wanted   *
             * dest. If equal, the strncmp will be 0. Therefore, use          *
             * !condition.                                                    */
            if (strcmp(flights[i].arrival_city, arrivalCode) == 0) {
               print_flight(&flights[i]);
               printNo++; /* Increment to track in case no prints were made. */
            }
      }
   }
}

/*******************************************************************************
 * This function will save all flights in the flights array into a text file
 * called "database".
 * inputs:
 * - const flight_t flights[]: we don't want to change the array within
 * the function so we will pass the array as const.
 * - size_t flights_size: size of flights[] array common pattern when passing
 * arrays to functions in C.
 * - flightCounter: we need to know how many flights to save.
 * outputs:
 * - none
*******************************************************************************/
void save_flights(const flight_t flights[], size_t flights_size, 
                  int flightCounter) 
{
   /* Create the file pointer and open a file called "database",
    * according to "fopen" specs, if a file "database" already exists, the file
    * will be overwritten.                                                    */
   FILE* filep;
   filep = fopen(DB_NAME, "w"); /* Open in write mode */

   /* There may be some issue writing the file, if so let user know. */
   if (filep != NULL) {
      int i; /* Loop iterator */
      /* We want to save every flight that we have added, hence we loop until *
       * we reach flightCounter. We cannot reuse "print_flight()" function    *
       * as the print function is different and the format is separated by    *
       * spaces and not "-" and ":" symbols. This is important as we will     *
       * be using fscanf on this file for loading information.                */
      for(i=0; i<flightCounter; i++) {
         fprintf(filep, "%s SYD %02d %02d %02d %02d %s %02d %02d %02d %02d\n",
            flights[i].flightcode, flights[i].departure_dt.month,
            flights[i].departure_dt.day, flights[i].departure_dt.hour,
            flights[i].departure_dt.minute, flights[i].arrival_city,
            flights[i].arrival_dt.month, flights[i].arrival_dt.day,
            flights[i].arrival_dt.hour, flights[i].arrival_dt.minute);
      }
   } else {
      printf("Write error\n"); /* Some error must have occured. */
   }
   fclose(filep); /* Close the file */
};


/*******************************************************************************
 * This function will load all flights from the "database" file and put them
 * into the flights[] array accordingly.
 * inputs:
 * - flight_t flights[]: we want to change the array within the function so we
 * won't pass the array as const.
 * - size_t flights_size: size of flights[] array common pattern when passing
 * arrays to functions in C.
 * outputs:
 * - int flightNo: the number of flights loaded by the function.
*******************************************************************************/
int load_flights(flight_t flights[], size_t flights_size) 
{
   /* Create the file pointer and open a file called "database", according to *
    * "fopen" specs, if a file "database" doesn't exist the pointer will be   *
    * NULL.                                                                   */
   FILE* filep;
   filep = fopen(DB_NAME, "r"); /* Open in read mode */
   int flightNo=0; /* Track number of flights loaded. */
   /* There may be some issue reading the file, if so let user know. */
   if (filep) {
      /* This while loop will keep doing fscanf until it hits an empty line in
       * in which it will give EOF making it stop the loop. We do this to make
       * sure we aren't scanning empty lines into flights array.              */
      while (fscanf(filep,"%s SYD %02d %02d %02d %02d %s %02d %02d %02d %02d\n",
         flights[flightNo].flightcode, &flights[flightNo].departure_dt.month,
         &flights[flightNo].departure_dt.day,
         &flights[flightNo].departure_dt.hour,
         &flights[flightNo].departure_dt.minute, flights[flightNo].arrival_city,
         &flights[flightNo].arrival_dt.month, &flights[flightNo].arrival_dt.day,
         &flights[flightNo].arrival_dt.hour,
         &flights[flightNo].arrival_dt.minute) != EOF)
      {
         flightNo++; /* Iterate amt of loaded flights so we can track it *
                           * in main.                                         */
      }
      fclose(filep); /* Close the file */
   } else {
      printf("Read error\n"); /* Some error must have occured. */
   }
   return flightNo;
};

/*******************************************************************************
 * This function takes in a flightCode string and runs various tests to validate
 * it and returns the result of this validation, 0 is fail and 1 if pass.
 * inputs:
 * - const char flightCode[]: flightCode "string" passed as const as no need to
 * change the array.
 * - size_t flightCodeLen: length of flightCode array.
 * outputs:
 * - int result: will return 1 if flightCode passes tests and 0 if it fails.
*******************************************************************************/
int checkFlightCode (const char flightCode[], size_t flightCodeLen) 
{
   /* Using ASCII chart to equate chars to A-Z and 0-9 */
   /* A-Z is range 65-90 inclusive and 0-9 is 48-57 inclusive in ASCII */

   /* printf("%d", flightCodeLen); FOR DEBUGGING */

   /* First check extra long input, code len must <= 6 characters  */
   if (flightCodeLen > (MAX_FLIGHTCODE_LEN + 1) /*6 chars + '\0'*/) {
      return 0; /* Too long: invalid */
   }

   /* We can now assume the array is no longer than max and can do checks
    * on each of the chars in the array. */
   if((flightCode[0] >= 65 && flightCode[0] <= 90)) {
      /* First char is A-Z */
      if ((flightCode[1] >= 65 && flightCode[1] <= 90)) {
         /* Both 1st and 2nd char are A-Z */

         /* From here, we are testing if the chars after the A-Z letters are
          * 0-9 depending on the length of the array as we can have 1-4 digits
          * after the first 3 chars. */
         if (flightCodeLen == 4 &&
             (flightCode[2] >= 48 && flightCode[2] <= 57)) {
            return 1;
         } else if (flightCodeLen == 5 &&
                    (flightCode[2] >= 48 && flightCode[2] <= 57) &&
                    (flightCode[3] >= 48 && flightCode[3] <= 57)) {
            return 1;
         } else if (flightCodeLen == 6 &&
                    (flightCode[2] >= 48 && flightCode[2] <= 57) &&
                    (flightCode[3] >= 48 && flightCode[3] <= 57) &&
                    (flightCode[4] >= 48 && flightCode[4] <= 57)) {
            return 1;
         } else if (flightCodeLen == 7 &&
                    (flightCode[2] >= 48 && flightCode[2] <= 57) &&
                    (flightCode[3] >= 48 && flightCode[3] <= 57) &&
                    (flightCode[4] >= 48 && flightCode[4] <= 57) &&
                    (flightCode[5] >= 48 && flightCode[5] <= 57)) {
            return 1;
         }

         /* There is most likely a better and more concise way to do this     *
          * but this is what I could up with my knowledge and limited time.   *
          * Although, I think this is definitely not a bad approach           *
          * considering there are only at most 4 digits after the first       *
          * 2 chars.                                                          */
      }
   }

   /* Failed validation conditions, return 0. */
   return 0;
};
