/*
* Filename:				dijkstra_example.c
* Programmer name:		Colin McMillan
* First useful version:	2014 November
* Description:			A program for the Amazing Race designed to determine the fastest route 
*						between any two cities given a list of departure and flight times. The 
*						user is asked for a city of origin, a destination, and the current time 
*						where they are. The fastest flight plan is then generated and returned to 
*						the user, providing them with the total time taken to get to their destination,
*						and the flights they will be taking, with times printed in the time zone
*						for each location.
*/

#include "dijkstra_example.h"


#pragma warning(disable: 4996)





int main(void)
{
	// Input variables. -1 is the failure/incorrect input state for each.
	int originCity = -1;
	int destinationCity = -1;
	int startTime = -1;

	int exitProgram = 0;

	// Flight arrays
	const Flight* earliestArrivals[kCityIndex] = { 0 };
	const Flight* flightPlan[kCityIndex] = { 0 };
	

	// Application loop
	do
	{
		// Reinitialize variables on new loop.
		originCity = -1;
		destinationCity = -1;
		startTime = -1;

		for (int i = 0; i < kCityIndex; i++)
		{
			earliestArrivals[i] = 0;
			flightPlan[i] = 0;
		}

		// Ask for the first city value until a valid city is chosen or they select 0.
		do
		{
			displayCityList(0);
			originCity = getMenuChoice(kExitMenu, kLastCity, 
				"Please enter the number for your city of origin (1-7).",
				"That is not a valid city number.");

			
			if (originCity == kExitMenu)
			{
				exitProgram = 1;
			}
		} while (originCity < 0);

		printf("\n");

		/* Ask for second city choice.
		The loop continues until the user has selected a valid menu option that is 
		not their originCity, or they select 0 to exit.*/
		do
		{
			// If the user previously chose to exit the program, immediately break.
			if (exitProgram == 1)
			{
				break;
			}


			displayCityList(originCity);
			destinationCity = getMenuChoice(kExitMenu, kLastCity,
				"Please enter the number for your destination (1-7).",
				"That is not a valid city number.");


			if (destinationCity == kExitMenu)
			{
				exitProgram = 1;
			}
			else if (destinationCity == originCity)
			{
				printf("Your destination can't be the same as your starting city.\n");
				printf("Please enter another city.\n");
				destinationCity = -1;
				waitForKey();
			}
		} while (destinationCity < 0);

		printf("\n");

		// Ask the user for the current local time until a valid time is entered.
		do
		{
			// If the user previously chose to exit the program, immediately break.
			if (exitProgram == 1)
			{
				break;
			}

			// Otherwise, get the user's input for time.
			startTime = getHHMMTime();

			// If the user has entered valid time, convert it to minutes.
			if (startTime >= 0)
			{
				startTime = timeAsMinutes(startTime);
			}
		} while (startTime < 0);

		if (exitProgram != 1)
		{
			printf("\n\n");

			// Calculate and print flight plan.
			mapEarliestArrivals(startTime, originCity, earliestArrivals);
			createFastestFlightplan(originCity, destinationCity, earliestArrivals, flightPlan);
			printItinerary(originCity, destinationCity, startTime, flightPlan);

			printf("\n");
			waitForKey();
			printf("\n\n\n\n\n\n\n");
		}

		
	} while (exitProgram != 1); // loop back to beginning, unless 0 was selected at some point.

	return 0;
}



// Time conversions
/*
* Function:			timeAsMinutes()
* Description:		Takes the time in hours:minutes format (a time without colon) 
*					and returns how many minutes that is.
* Parameters:		int timeInHHMM:		The hours and minutes, eg 214 for 2 hours, 14 minutes
* Return Values:	The time in minutes equivalent to timeInHHMM
*/
int timeAsMinutes(int timeInHHMM)
{
	// Minutes is the remainder of HHMM divided by 100
	int minutes = timeInHHMM % 100;
	// Hours into minutes is HHMM integer divided by 100, multiplied by 60.
	int hoursAsMinutes = (timeInHHMM / 100) * kMinutesPerHour;

	return (minutes + hoursAsMinutes);
}



/*
* Function:			timeAsHHMM()
* Description:		Takes a time in minutes and produces a result that is in hours & minutes
*					format
* Parameters:		int timeInMinutes	An amount of time in minutes
* Return Values:	The hours and minutes, eg 214 for 2 hours, 14 minutes. Given in 24 hour 
*					format.
*/
int timeAsHHMM(int timeInMinutes)
{
	// Total minutes integer divide by 60 gives the number of hours.
	int hours = (timeInMinutes / kMinutesPerHour);
	// MM is given by what's left over from the total minutes.
	int minutes = timeInMinutes % kMinutesPerHour;

	// When added together, HH00 + MM = HHMM
	return (hours * 100 + minutes);
}



/*
* Function:			timezoneOffset()
* Description:		Takes a city ID and returns the difference between that city's
*					timezone and UTC.
* Parameters:		int cityID		The number identifier of the city.
* Return Values:	The time offset from UTC.
*					(For this program, always a negative number)
*/
int timezoneOffset(int cityID)
{
	int offset = 0;


	if ((cityID == kAustinID) || (cityID == kChicagoID))
	{
		offset = kCSToffset;
	}
	else if ((cityID == kDenverID) || (cityID == kSantaFeID))
	{
		offset = kMSToffset;
	}
	else if ((cityID == kTorontoID) || (cityID == kAtlantaID) || (cityID == kBuffaloID))
	{
		offset = kESToffset;
	}

	return offset;
}



/*
* Function:			timezoneDifference()
* Description:		Takes two cities and returns the difference in their timezones.
* Parameters:		int originCity		The number identifier of the origin city.
*					int destinationCity	The number identifier of the destination city.
* Return Values:	The difference, in hours, between timezones.
*					Positive X means the destination is X hours behind.
*					Negative X means the destination is X hours ahead.
*/
int timezoneDifference(int originCity, int destinationCity)
{
	int originOffset = timezoneOffset(originCity);
	int destinationOffset = timezoneOffset(destinationCity);

	return (originOffset - destinationOffset);
}
// End of time conversion functions



/*
* Function:			airportDirectory()
* Description:		Takes a cityID and returns a pointer to the Airport constant
*					for that city.
* Parameters:		int cityID	The ID of the airport you wish to find the Airport struct for.
* Return Values:	A const Airport pointer to the desired Airport struct.
*/
const Airport* airportDirectory(int cityID)
{
	const Airport* airport = NULL;

	if (cityID == kTorontoID)
	{
		airport = &kTorontoAirport;
	}
	else if (cityID == kAtlantaID)
	{
		airport = &kAtlantaAirport;
	}
	else if (cityID == kAustinID)
	{
		airport = &kAustinAirport;
	}
	else if (cityID == kSantaFeID)
	{
		airport = &kSantaFeAirport;
	}
	else if (cityID == kDenverID)
	{
		airport = &kDenverAirport;
	}
	else if (cityID == kChicagoID)
	{
		airport = &kChicagoAirport;
	}
	else if (cityID == kBuffaloID)
	{
		airport = &kBuffaloAirport;
	}

	return airport;
}



// UI functions
/*
* Function:			displayCityList()
* Description:		Displays a list of the cities and their corresponding numbers
* Parameters:		int skipNumber		A cityID to be skipped from the list. 0 if all
*										options should be listed.
*/
void displayCityList(int skipNumber)
{
	char cityName[kCityNameMax] = "";

	for (int i = 1; i <= kLastCity; i++)
	{
		if (i != skipNumber)
		{
			printf("%d) ", i);
			printAirportName(i);
			printf("\n");
		}
		else
		{
			printf("-) City of origin: ");
			printAirportName(i);
			printf("\n");
		}
	}
	printf("\n0) Exit program\n");
}



/*
* Function:			printAirportName()
* Description:		Takes an airport number and prints that name to the screen.
* Parameters:		int airportNumber	The number identifier for the airport
*/
void printAirportName(int airportNumber)
{
	if (airportNumber == kTorontoID)
	{
		printf("Toronto");
	}
	else if (airportNumber == kAtlantaID)
	{
		printf("Atlanta");
	}
	else if (airportNumber == kAustinID)
	{
		printf("Austin");
	}
	else if (airportNumber == kSantaFeID)
	{
		printf("Santa Fe");
	}
	else if (airportNumber == kDenverID)
	{
		printf("Denver");
	}
	else if (airportNumber == kChicagoID)
	{
		printf("Chicago");
	}
	else if (airportNumber == kBuffaloID)
	{
		printf("Buffalo");
	}
	else
	{
		printf("Invalid");
	}
}



/*
* Function:			printTime()
* Description:		Takes the time in minutes and prints it in HH:MM format.
* Parameters:		int timeInMinutes	The time in minutes to be converted into HH:MM
*/
void printTime(int timeInMinutes)
{
	int hours = timeAsHHMM(timeInMinutes) / 100;
	int minutes = timeAsHHMM(timeInMinutes) % 100;

	printf("%d:%02d", hours, minutes);
}



/*
* Function:			printClockTime()
* Description:		Takes the time in minutes and prints it in the standard HH:MM format,
*					using a 12 hour clock. Also prints a.m./f.m., timezone, and a notification that
*					one day has passed to the next, if necessary.
* Parameters:		int timeInMinutes	The time in minutes to be converted into HH:MM
*					int cityID			The city ID number, to be used to calculate timezone.
*/
void printClockTime(int timeInMinutes, int cityID)
{
	int hours = timeAsHHMM(timeInMinutes) / 100;
	int minutes = timeAsHHMM(timeInMinutes) % 100;

	char meridian[] = "a.m.";
	char timezone[4] = "";
	char nextDay[] = " the next day";

	// Set timezone
	if (timezoneOffset(cityID) == kCSToffset)
	{
		strcpy(timezone, "CST");
	}
	else if (timezoneOffset(cityID) == kMSToffset)
	{
		strcpy(timezone, "MST");
	}
	else if (timezoneOffset(cityID) == kESToffset)
	{
		strcpy(timezone, "EST");
	}
	
	// If over 24 hours, reduce the time by 24 hours. Otherwise, remove note about " the next day"
	if (hours > kHoursPerDay)
	{
		hours -= kHoursPerDay;
	}
	else
	{
		strcpy(nextDay, "");
	}

	// If over 12 hours, reduce by 12 and switch to p.m.
	if (hours > (kHoursPerDay / 2))
	{
		hours -= kHoursPerDay / 2;
		strcpy(meridian, "p.m.");
	}


	// For 00/12 a.m.
	if (hours == 00)
	{
		hours = 12;
	}

	// For 12 p.m.
	else if (hours == 12)
	{
		strcpy(meridian, "p.m.");
	}
	


	// Print the result.
	printf("%d:%02d %s %s%s", hours, minutes, meridian, timezone, nextDay);
}



/*
* Function:			printItinerary()
* Description:		Given a flightplan, an itinerary is printed out, including all
*					departure and arrival times (in local timezones) and total travel time.
* Parameters:		int origin					The ID of the starting airport.
*					int destination				The ID of the final destination.
*					int startTime				The start time (in minutes since midnight 
*												local time) of the journey.
*					const Flight* flightPlan[]	The chain of flights to be printed.
*/
void printItinerary(int origin, int destination, const int startTime, 
	const Flight* flightPlan[kCityIndex])
{
	int localStartTime = startTime;
	int totalTravelTime = 0;

	printf("Flying from ");
	printAirportName(origin);
	printf(" to ");
	printAirportName(destination);
	printf(".\n\n");

	printf("Starting from ");
	printAirportName(origin);
	printf(" at ");
	printClockTime(localStartTime, origin);
	printf(".\n");


	// For each flight in the flightPlan
	for (int i = 0; i < kCityIndex; i++)
	{
		// If there are no flights left in the flightPlan.
		if (flightPlan[i] == NULL)
		{
			break;
		}

		int flightOrigin = flightPlan[i]->originCity;
		int flightDestination = flightPlan[i]->destinationCity;
		int flightDuration = timeAsMinutes(flightPlan[i]->flightDuration);

		int localDepartureTime = timeAsMinutes(flightPlan[i]->departureTime);
		int destinationArrivalTime = localDepartureTime + flightDuration;

		// Account for the difference in timezones for the destination arrival time.
		destinationArrivalTime -= timezoneDifference(flightOrigin, flightDestination) 
			* kMinutesPerHour;

		/* If departure time since midnight is less than current time since midnight,
		you're leaving the next day. Increase localDepartureTime by one day.*/
		if (localDepartureTime < localStartTime)
		{
			localDepartureTime += kMinutesPerDay;
		}



		printf("Leaving ");
		printAirportName(flightOrigin);
		printf(" at ");
		printClockTime(localDepartureTime, flightOrigin);
		printf(" for ");
		printAirportName(flightDestination);
		printf(".\n");



		printf("Arriving in ");
		printAirportName(flightDestination);
		printf(" at ");
		printClockTime(destinationArrivalTime, flightDestination);
		printf(".\n");


		/* Increase travel time by the difference between localStartTime and localDepartureTime
		(layover time waiting from when one flight touches down to the next one taking off) and
		flight time. */
		totalTravelTime += localDepartureTime - localStartTime;
		totalTravelTime += flightDuration;

		// Prepare for the next loop.
		localStartTime = destinationArrivalTime;

		/* Prevents some reduntant " the next day" messages. localStartTime shouldn't be greater 
		than a day's worth of minutes for the beginning of the for, so reduce it by a day if it is. */
		if (localStartTime > (kMinutesPerDay))
		{
			localStartTime -= kMinutesPerDay;
		}
	}

	printf("\nTotal travel time: ");
	printTime(totalTravelTime);
	printf(".\n");
}



/*
* Function:			getMenuChoice()
* Description:		Display a prompt and wait for user numerical input within a range.
* Parameters:		int minValue			A minimum valid response.
*					int maxValue			A maximum valid response.
*					char prompt[]			A prompt string, asking for input.
*					char invalidResponse[]	A string giving a response for invalid/out of range input.
* Return Values:	The user's chosen number, if valid, or -1 if invalid input was received.
*/
int getMenuChoice(int minValue, int maxValue, char prompt[], char invalidResponse[])
{
	int userChoice = 0;

	// Prompt the user.
	printf("%s\n", prompt);

	// Get user input.
	userChoice = getNum();

	// If the user's entered number isn't within range, set choice to be -1.
	if (!checkRange(userChoice, kExitMenu, kLastCity))
	{
		userChoice = -1;

		// Error response
		printf("%s \n", invalidResponse);
		printf("Press any key to continue.\n");
		getch();
		printf("\n");
	}
	
	return userChoice;
}



/*
* Function:			getHHMMTime()
* Description:		Display a prompt and wait for user numerical input within a range.
* Return Values:	The user's chosen number, if valid, or -1 if invalid input was received.
*/
int getHHMMTime(void)
{
	int usersTime = 0;

	int hours = 0;
	int minutes = 0;

	printf("Please enter the current time in your city of origin, in 24-hour format.\n");
	printf("Enter times in the form HHMM, with no colon between hours and minutes.\n");
	printf("e.g. 140, 1820, 0020, 2359.\n");

	// Get user input.
	usersTime = getNum();

	// Separate HH00 and MM into hours and minutes.
	hours = usersTime / 100;
	minutes = usersTime % 100;

	if ((hours >= 24) || (hours < 0))
	{
		printf("Invalid input for hours.\n");
		hours = -1;
	}
	if ((minutes >= 60) || (minutes < 0))
	{
		printf("Invalid input for minutes.\n");
		minutes = -1;
	}

	// If the minutes or hours are invalid, set usersTime to -1.
	if ((minutes < 0) || (hours < 0))
	{
		usersTime = -1;
		printf("That is not a valid time. Please try again.\n");
		waitForKey();
		printf("\n");
	}
	// Otherwise, just return their input time.

	return usersTime;
}



/*
* Function:			waitForKey()
* Description:		Display a prompt for the user to press any key, then wait for any input.
*/
void waitForKey(void)
{
	printf("Press any key to continue.");
	getch();
	printf("\n\n");
}
// End of UI functions



// Flight calculations
/*
* Function:			soonestArrival()
* Description:		Finds the flight that arrives soonest at a given destination from a given
*					origin. 
* Parameters:		int startTime			The time, in minutes since midnight, that the flyer is
*											at the origin airport. Given in UTC.
*					int origin				The ID of the origin airport.
*					int destination			The ID of the destination airport.
*					Flight* soonestArrival	The flight that gets to the destination airport
*											the fastest. NULL if no connection available.
* Return Values:	Returns the time of arrival at the destination, given as the difference
*					from startTime to time of arrival in minutes.
*					Also returns a pointer to the actual flight, via soonestArrival.
*/
int soonestArrival(const int startTime, int origin, int destination, const Flight** soonestArrival)
{
	// The local clock time, given in minutes since local midnight.
	int localStartTime = startTime % (kMinutesPerDay) + timezoneOffset(origin) * kMinutesPerHour;

	int arrivalTime = 0;
	int bestArrivalTime = 0;

	char loopedAround = 0;		// 1 if the flight scan has had to go past midnight to the next day.

	const Airport* originAirport = airportDirectory(origin);
	const Flight* flightList = originAirport->flightList[destination];

	// If the originAirport has flights to the destination...
	if (originAirport->flightConnections[destination] != 0)
	{

		// <Flight scan>
		// Check each flight going out from this airport to the destination.
		for (int i = 0; i < kMaxFlightsToDestination; i++)
		{
			// How long the layover is. Used to create a point of comparison, departureTimeUTC
			int waitSinceArrival = timeAsMinutes(flightList[i].departureTime) - localStartTime;

			/* The departure time according to UTC, given as minutes since midnight UTC on the 
			original departure. */
			int departureTimeUTC = startTime + waitSinceArrival;


			/* Since the flights are arranged chronologically, if they are departing after the 
			current best arrival time, there is no way that any later flights could get to the 
			destination any sooner.
			Either way, break from the Flight Scan.*/
			if ((departureTimeUTC > bestArrivalTime) && (bestArrivalTime != 0))
			{
				break;
			}
			/* If the end of the list has been reached (empty flight entries would have originCity 
			of 0) loop around to the next day until it's 100% certain the best flight has been
			found. */
			else if ((flightList[i].originCity == 0) && (loopedAround !=1))
			{
				// Sets loopedAround flag to 1, so that time can be adjusted accordingly later.
				loopedAround = 1;
				/* localStartTime is set back 24 hours - to the same time the day before - and 
				the cycle begins again, until the best time is found.*/
				localStartTime -= kMinutesPerDay;

				// Restarts flight scan.
				i = -1;
				continue;
			}
			/* If the end of the list has been reached and the day has already cycled once, there's
			definitely no better flights to be found (since flights are the same every day, if you 
			COULD catch it and it wasn't the best flight to take today, it's not better to take it 
			tomorrow.) Break.*/
			else if (flightList[i].originCity == 0)
			{
				/* Without this break, there gets to be some funky behaviour with flights that are
				around midnight UST and departure times of 0 (i.e. no flight available.) */
				break;
			}
			else
			{
				// If the flight leaves after the current time (i.e. haven't missed it)
				if (timeAsMinutes(flightList[i].departureTime) > localStartTime)
				{
					// The arrival time is calculated.
					arrivalTime = departureTimeUTC + timeAsMinutes(flightList[i].flightDuration);

					/* If no bestArrivalTime has been found, or if the arrivalTime for this flight 
					is better than the current best. */
					if ((bestArrivalTime == 0) || (arrivalTime < bestArrivalTime))
					{
						// Set the new best time, and set soonestArrival to point at this flight.
						bestArrivalTime = arrivalTime;
						*soonestArrival = &flightList[i];
					}
				} // End of "if flight hasn't left yet"

			} // End of checking flight i

		} // End of checking each flight from origin to destination

	} // End of "if this airport has flights to given destination."

	// If the originAirport has no flights to the destination, set soonestArrival to NULL.
	else
	{
		soonestArrival = NULL;
	}

	return bestArrivalTime;
}



/*
* Function:			mapEarliestArrivals()
* Description:		Maps out the earliest possible arrival time at each airport from your original 
*					airport, at the current time, and returns a list of the flights needed to get 
*					to each one.
* Parameters:		int startTimeMinutes		The user's starting time, in the local timezone.
*					int originAirport			The user's starting airport.
*					Flight earliestArrivals[]	An array to pass a list of flights to, representing
*												the earliest flights available to each destination.
*/
void mapEarliestArrivals(const int startTimeInMinutes, int originAirport, 
	const Flight* earliestArrivals[kCityIndex])
{
	/* The earliest possible time that each airport can be reached.
	The earliestGroundTime is given in minutes since midnight of the
	first day. All times (including original start time) are recorded
	in UTC.
	[0] is always 0, so numbering for airports remains consistent.*/
	int earliestGroundTime[kCityIndex] = { 0 };

	/* 0 if the airport does not have an updated earliestGroundTime.
	1 if the airport has an updated earliestGroundTime and needs to be rechecked.
	[0] is always 0, so numbering for airports remains consistent. */
	int earliestTimeUpdated[kCityIndex] = { 0 };

	/* The sum of earliestTimeUpdated. If 0, the fastest path has been found.
	If greater than 0, more passes are required.*/
	int updatesNeeded = 0;

	// Loop variables.
	int departureAirport = 0;
	int arrivalAirport = 0;

	// The earliestGroundTime for the origin airport is startTimeMinutes, in UTC.
	earliestGroundTime[originAirport] = startTimeInMinutes 
		- timezoneOffset(originAirport) * kMinutesPerHour;

	earliestTimeUpdated[originAirport] = 1;

	do
	{
		// <Airport check loop>
		// Check each airport...
		for  (departureAirport = 1; departureAirport < kCityIndex; departureAirport++)
		{

			// For each airport that has an updated earliestGroundTime...
			if (earliestTimeUpdated[departureAirport] != 0)
			{

				// <Destination from airport check loop>
				for (arrivalAirport = 1; arrivalAirport < kCityIndex; arrivalAirport ++)
				{

					/* Never check if an airport has a connection to itself, and never check for
					flights towards the original airport, to save time.
					Otherwise, check for flights to the arrivalAirport.*/
					if ((departureAirport != arrivalAirport)
						&& (arrivalAirport != originAirport))
					{
						/* arrivalTime is determined by the soonestArrival function. It is given
						in minutes since midnight on the day of departure from the originAirport.*/
						int arrivalTime = 0;

						const Flight* quickestFlightToGround = NULL;

						/* Set the best flight from the departureAirport to the arrivalAirport given
						the earliest possible time you could arrive there. If there is no flight to
						that destination, quickestFlightToGround will be set to NULL by soonestArrival,
						and the following if statement will be false.*/
						arrivalTime = soonestArrival(earliestGroundTime[departureAirport],
							departureAirport, arrivalAirport,
							&quickestFlightToGround);

						/* If the soonest arrival at the arrivalAirport is sooner than the
						existing earliestGroundTime, or there is no existing flight to the
						arrivalAirport, update the earliestGroundTime and set the update flag
						to true.
						If there is no quickestFlightToGround from the arrivalTime search, then
						no update occurs, even if the other conditions are true.*/
						if (
							((arrivalTime < earliestGroundTime[arrivalAirport])
							|| (earliestArrivals[arrivalAirport] == NULL))

							&& (quickestFlightToGround != NULL)
							)
						{
							// Update the earliestGroundTime for this airport, and set update flag on.
							earliestGroundTime[arrivalAirport] = arrivalTime;
							earliestTimeUpdated[arrivalAirport] = 1;

							/* The earliestArrivals for the given destination is now pointing at
							the quickestFlightToGround from this loop. */
							earliestArrivals[arrivalAirport] = quickestFlightToGround;
						}
					}

				} // End of destination check loop.
			
			/* Once all destinations from an airport are checked, that airport has been updated.
			Set the earliestTimeUpdated flag for this airport to 0. */
			earliestTimeUpdated[departureAirport] = 0;

			} // End of "if airport has been updated"

		} // End of airport check loop.


		// Check if another round of updates is needed.
		updatesNeeded = 0;
		for (int i = 0; i < kCityIndex; i++)
		{
			updatesNeeded += earliestTimeUpdated[i];
		}

	} while (updatesNeeded != 0);
	// Once all airports have a soonest flight to arrival, return.
}



/*
* Function:			createFastestFlightplan()
* Description:		Takes a list of optimized arrival times and forms a flight plan from
*					one location to another.
* Parameters:		int originAirport			The starting airport.
*					int destinationAirport		The final destination.
*					Flight earliestArrivals[]	An array of the best possible arrival times.
*					Flight fastestFlightPlan[]	The optimized flightplan to output.
*/
void createFastestFlightplan(int originAirport, int destinationAirport,
	const Flight* earliestArrivals[kCityIndex], const Flight* fastestFlightPlan[kLastCity])
{
	const Flight* backwardsFlightPlan[kLastCity] = { NULL };
	int currentAirport = destinationAirport;
	int stepsTaken = -1;
	int stepsReversed = 0;

	// Starting from the final destination, work backwards to the origin.
	do
	{
		stepsTaken++;
		backwardsFlightPlan[stepsTaken] = earliestArrivals[currentAirport];
		currentAirport = earliestArrivals[currentAirport]->originCity;
	} while (currentAirport != originAirport);

	/* Since the previous do loop creates the flight plan in reverse, we return it to
	cronological order for output. As stepsReversed increments, it decrements the stepsTaken
	in the backwardsFlightPlan.*/
	for (stepsReversed = 0; stepsReversed <= stepsTaken; stepsReversed++)
	{
		fastestFlightPlan[stepsReversed] = backwardsFlightPlan[stepsTaken - stepsReversed];
	}
}



/*
* Function:			checkRange()
* Description:		Takes one integer and checks if it's within a particular (inclusive) range.
* Parameters:		int checkInt		The integer to be checked
*					int minValue		The lowest valid value
*					int maxValue		The highest valid value
* Return Values:	0 if the integer is out of range, 1 if it's within range.
*/
int checkRange(int checkInt, int minValue, int maxValue)
{
	int isValid = 0;

	// If the checked value is within range... return valid.
	if ((checkInt >= minValue) && (checkInt <= maxValue))
	{
		isValid = 1;
	}

	// Otherwise, 0 is returned.

	return isValid;
}



/*
* Function:			getNum()		(Provided courtesy of the assignment notes)
* Description:		Gets a string of characters from the keyboard and extracts an integer.
* Return Values:	The number extracted from the string.
*/
int getNum(void)
{
	#define maxCharacters 81

	char record[maxCharacters] = { 0 };		// record stores the string
	int number = 0;

	fflush(stdin);

	// NOTE to  student: indent and brace this function consistent with  your others
	// use  fgets() to get a string from the keyboard
	fgets(record, maxCharacters, stdin);

	// extract  the number from the string; sscanf() returns a number
	// corresponding with the number of items it found in the string
	if (sscanf(record, "%d", &number) != 1)
	{
		// if the user did not enter a number recognizable by
		// the system, set number to -1
		number = -1;
	}

	return  number;
}