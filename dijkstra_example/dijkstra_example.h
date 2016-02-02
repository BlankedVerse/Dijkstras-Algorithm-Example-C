/*
* Filename:				dijkstra_example.h
* Programmer name:		Colin McMillan
* First useful version:	2014 November
* Description:			The header file for use with dijkstra_example.c. Contains all the constants, 
*						typedefines/structs, and prototypes for use with the Amazing Race flight
*						planner program. Specifically, it has the listing of the seven airports 
*						available for use, along with all outgoing flights from those airports.
*/


#ifndef __dijkstra_example_H__
#define __dijkstra_example_H__

// Standard headers
#include <stdio.h>
#include <string.h>
#include <conio.h>



// Constants

// - Array size constants
/* Used to define the size of arrays where the index number corresponds to a particular city.
Is equal to the number of cities + 1, with the 0 index generally being blank.*/
#define kCityIndex 8
#define kLastCity 7

// The maximum number of flights a given airport can have to a given destination
#define kMaxFlightsToDestination 17	
#define kCityNameMax 12

// - Index constants. Used when defining the airport constants.
#define kToronto 1
#define kAtlanta 2
#define kAustin 3
#define kSantaFe 4
#define kDenver 5
#define kChicago 6
#define kBuffalo 7

// - Menu/City identifier constants
const int kExitMenu = 0;
const int kTorontoID = kToronto;
const int kAtlantaID = kAtlanta;
const int kAustinID = kAustin;
const int kSantaFeID = kSantaFe;
const int kDenverID = kDenver;
const int kChicagoID = kChicago;
const int kBuffaloID = kBuffalo;

// - Timezone offset constants (in reference to UTC)
const int kMSToffset = -7;		// The timezone offset for Denver and Santa Fe.
const int kCSToffset = -6;		// The timezone offset for Austin and Chicago.
const int kESToffset = -5;		// The timezone offset for Toronto, Atlanta, and Buffalo.

// - Time conversion constants
const int kMinutesPerHour = 60;
const int kHoursPerDay = 24;
const int kMinutesPerDay = 60 * 24;





// Structure/typedefs
typedef struct
{
	int originCity;			// The cityID for the city the flight starts from.
	int destinationCity;	// The cityID for the city the flight ends at.
	int departureTime;		// The time the flight leaves, in originCity local time (HHMM format).
	int flightDuration;		// The duration of the flight, in HHMM format.
} Flight;

typedef struct
{
	/* An array of ints. 0 at a given index indicates that the city has no connection
	to the city having the cityID == index. Non-zero indicates that at least one
	connecting flight exists. */
	char flightConnections[kCityIndex];
	/* A multidimensional array of flights. The first index indicates which airport
	it flies to, and the second index points to individual flights.
	*/
	Flight flightList[kCityIndex][kMaxFlightsToDestination];
} Airport;




// Prototypes
int timeAsMinutes(int timeInHHMM);
int timeAsHHMM(int timeInMinutes);
int timezoneOffset(int cityID);
int timezoneDifference(int originCity, int destinationCity);
const Airport* airportDirectory(int cityID);
void displayCityList(int skipNumber);
void printAirportName(int airportNumber);
void printTime(int timeInMinutes);
void printClockTime(int timeInMinutes, int cityID);
void printItinerary(int origin, int destination, const int startTime, 
	const Flight* flightPlan[kCityIndex]);

int getMenuChoice(int minValue, int maxValue, char prompt[], char invalidResponse[]);
int getHHMMTime(void);
void waitForKey(void);

int soonestArrival(const int startTime, int origin, int destination, const Flight** soonestArrival);
void mapEarliestArrivals(const int startTimeInMinutes, int originAirport, 
	const Flight* earliestArrivals[kCityIndex]);
void createFastestFlightplan(int originAirport, int destinationAirport,
	const Flight* earliestArrivals[kCityIndex], const Flight* fastestFlightPlan[kLastCity]);

int checkRange(int checkInt, int minValue, int maxValue);
int getNum(void);




// Airport listings
const Airport kTorontoAirport =
{
	// Connections checklist: Atlanta (2), Denver (5), and Chicago (6).
	{ 0, 0, kAtlanta, 0, 0, kDenver, kChicago, 0 },

	// Flight list
	{
		// Empty space for 0 index
		{ 0 },
		// To Toronto
		{ 0 },
		// To Atlanta
		{
			{ kToronto, kAtlanta, 625, 220 },
			{ kToronto, kAtlanta, 910, 450 },
			{ kToronto, kAtlanta, 1230, 415 },
			{ kToronto, kAtlanta, 1610, 610 },
			{ kToronto, kAtlanta, 2000, 215 },
		},
		// To Austin
		{ 0 },
		// To Santa Fe
		{ 0 },
		// To Denver
		{
			{ kToronto, kDenver, 730, 335 },
			{ kToronto, kDenver, 1500, 600 },
		},
		// To Chicago
		{
			{ kToronto, kChicago, 640, 120 },

			{ kToronto, kChicago, 740, 135 },
			{ kToronto, kChicago, 840, 135 },
			{ kToronto, kChicago, 940, 135 },
			{ kToronto, kChicago, 1040, 135 },
			{ kToronto, kChicago, 1140, 135 },
			{ kToronto, kChicago, 1240, 135 },
			{ kToronto, kChicago, 1340, 135 },
			{ kToronto, kChicago, 1440, 135 },

			{ kToronto, kChicago, 1530, 145 },
			{ kToronto, kChicago, 1630, 145 },
			{ kToronto, kChicago, 1730, 145 },
			{ kToronto, kChicago, 1830, 145 },
			{ kToronto, kChicago, 1930, 145 },

			{ kToronto, kChicago, 2100, 130 },
			{ kToronto, kChicago, 2200, 115 },
		},
		// To Buffalo
		{ 0 }
	}
};


const Airport kAtlantaAirport =
{
	// Connections checklist: Toronto(1), Austin(3), Denver(5), Chicago(6).
	{ 0, kToronto, 0, kAustin, 0, kDenver, kChicago, 0 },

	// Flight list
	{
		// Empty space for 0 index
		{ 0 },
		// To Toronto
		{
			{ kAtlanta, kToronto, 710, 210 },
			{ kAtlanta, kToronto, 1030, 410 },
			{ kAtlanta, kToronto, 1500, 350 },
			{ kAtlanta, kToronto, 1710, 610 },
			{ kAtlanta, kToronto, 2100, 220 },
		},
		// To Atlanta
		{ 0 },
		// To Austin
		{
			{ kAtlanta, kAustin, 900, 210 },
			{ kAtlanta, kAustin, 1530, 250 },
			{ kAtlanta, kAustin, 2000, 230 },
		},
		// To Santa Fe
		{ 0 },
		// To Denver
		{
			{ kAtlanta, kDenver, 600, 300 },
			{ kAtlanta, kDenver, 1320, 500 },
			{ kAtlanta, kDenver, 1710, 250 },
		},
		// To Chicago
		{
			{ kAtlanta, kChicago, 650, 210 },

			{ kAtlanta, kChicago, 750, 300 },
			{ kAtlanta, kChicago, 850, 300 },
			{ kAtlanta, kChicago, 950, 300 },
			{ kAtlanta, kChicago, 1050, 300 },
			{ kAtlanta, kChicago, 1150, 300 },
			{ kAtlanta, kChicago, 1250, 300 },
			{ kAtlanta, kChicago, 1350, 300 },
			{ kAtlanta, kChicago, 1450, 300 },

			{ kAtlanta, kChicago, 1550, 230 },
			{ kAtlanta, kChicago, 1650, 230 },
			{ kAtlanta, kChicago, 1750, 230 },
			{ kAtlanta, kChicago, 1850, 230 },
			{ kAtlanta, kChicago, 1950, 230 },

			{ kAtlanta, kChicago, 2030, 210 },
		},
		// To Buffalo
		{ 0 }
	}
};


const Airport kAustinAirport =
{

	// Connections checklist: Atlanta (2), SantaFe(4), and Denver(5).
	{ 0, 0, kAtlanta, 0, kSantaFe, kDenver, 0, 0 },

	// Flight list
	{
		// Empty space for 0 index
		{ 0 },
		// To Toronto
		{ 0 },
		// To Atlanta
		{
			{ kAustin, kAtlanta, 910, 220 },
			{ kAustin, kAtlanta, 1500, 220 },
			{ kAustin, kAtlanta, 2130, 230 },
		},
		// To Austin
		{ 0 },
		// To Santa Fe
		{
			{ kAustin, kSantaFe, 1700, 55 },
		},
		// To Denver
		{
			{ kAustin, kDenver, 1030, 220 },
			{ kAustin, kDenver, 1820, 220 },
		},
		// To Chicago
		{ 0 },
		// To Buffalo
		{ 0 }
	}
};


const Airport kSantaFeAirport =
{

	// Connections checklist: Austin (3).
	{ 0, 0, 0, kAustin, 0, 0, 0, 0 },

	// Flight list
	{
		// Empty space for 0 index
		{ 0 },
		// To Toronto
		{ 0 },
		// To Atlanta
		{ 0 },
		// To Austin
		{
			{ kSantaFe, kAustin, 1500, 45 },
		},
		// To Santa Fe
		{ 0 },
		// To Denver
		{ 0 },
		// To Chicago
		{ 0 },
		// To Buffalo
		{ 0 }
	}
};


const Airport kDenverAirport =
{

	// Connections checklist: Toronto (1), Atlanta (2), Austin (3), and Chicago(6).
	{ 0, kToronto, kAtlanta, kAustin, 0, 0, kChicago, 0 },

	// Flight list
	{
		// Empty space for 0 index
		{ 0 },
		// To Toronto
		{
			{ kDenver, kToronto, 630, 410 },
			{ kDenver, kToronto, 1030, 520 },
			{ kDenver, kToronto, 1400, 500 },
		},
		// To Atlanta
		{
			{ kDenver, kAtlanta, 600, 310 },
			{ kDenver, kAtlanta, 1300, 320 },
			{ kDenver, kAtlanta, 1500, 350 },
		},
		// To Austin
		{
			{ kDenver, kAustin, 1200, 200 },
			{ kDenver, kAustin, 1500, 220 },
		},
		// To Santa Fe
		{ 0 },
		// To Denver
		{ 0 },
		// To Chicago
		{
			{ kDenver, kChicago, 700, 220 },

			{ kDenver, kChicago, 800, 250 },
			{ kDenver, kChicago, 1000, 250 },
			{ kDenver, kChicago, 1200, 250 },
			{ kDenver, kChicago, 1400, 250 },
			{ kDenver, kChicago, 1600, 250 },

			{ kDenver, kChicago, 1830, 240 },
		},
		// To Buffalo
		{ 0 }
	}
};


const Airport kChicagoAirport =
{

	// Connections checklist: Toronto (1), Atlanta (2), Denver(5), and Buffalo(7).
	{ 0, kToronto, kAtlanta, 0, 0, kDenver, 0, kBuffalo },

	// Flight list
	{
		// Empty space for 0 index
		{ 0 },
		// To Toronto
		{
			{ kChicago, kToronto, 740, 110 },

			{ kChicago, kToronto, 910, 230 },
			{ kChicago, kToronto, 1010, 230 },
			{ kChicago, kToronto, 1110, 230 },
			{ kChicago, kToronto, 1210, 230 },
			{ kChicago, kToronto, 1310, 230 },
			{ kChicago, kToronto, 1410, 230 },
			{ kChicago, kToronto, 1510, 230 },
			{ kChicago, kToronto, 1610, 230 },
			{ kChicago, kToronto, 1710, 230 },

			{ kChicago, kToronto, 1910, 200 },
			{ kChicago, kToronto, 2110, 210 },
		},
		// To Atlanta
		{
			{ kChicago, kAtlanta, 650, 210 },

			{ kChicago, kAtlanta, 800, 240 },
			{ kChicago, kAtlanta, 900, 240 },
			{ kChicago, kAtlanta, 1000, 240 },
			{ kChicago, kAtlanta, 1100, 240 },
			{ kChicago, kAtlanta, 1200, 240 },
			{ kChicago, kAtlanta, 1300, 240 },
			{ kChicago, kAtlanta, 1400, 240 },
			{ kChicago, kAtlanta, 1500, 240 },
			{ kChicago, kAtlanta, 1600, 240 },
			{ kChicago, kAtlanta, 1700, 240 },
			{ kChicago, kAtlanta, 1800, 240 },
			{ kChicago, kAtlanta, 1900, 240 },
			{ kChicago, kAtlanta, 2000, 240 },

			{ kChicago, kAtlanta, 2150, 300 },
		},
		// To Austin
		{ 0 },
		// To Santa Fe
		{ 0 },
		// To Denver
		{
			{ kChicago, kDenver, 900, 210 },

			{ kChicago, kDenver, 1130, 220 },
			{ kChicago, kDenver, 1330, 220 },
			{ kChicago, kDenver, 1530, 220 },
			{ kChicago, kDenver, 1730, 220 },

			{ kChicago, kDenver, 2100, 250 },
		},
		// To Chicago
		{ 0 },
		// To Buffalo
		{
			{ kChicago, kBuffalo, 1100, 200 },
			{ kChicago, kBuffalo, 1310, 150 },
			{ kChicago, kBuffalo, 1500, 230 },
			{ kChicago, kBuffalo, 1800, 210 },
		}
	}
};


const Airport kBuffaloAirport =
{

	// Connections checklist: Chicago(6).
	{ 0, 0, 0, 0, 0, 0, kChicago, 0 },

	// Flight list
	{
		// Empty space for 0 index
		{ 0 },
		// To Toronto
		{ 0 },
		// To Atlanta
		{ 0 },
		// To Austin
		{ 0 },
		// To Santa Fe
		{ 0 },
		// To Denver
		{ 0 },
		// To Chicago
		{
			{ kBuffalo, kChicago, 940, 140 },
			{ kBuffalo, kChicago, 1110, 150 },
			{ kBuffalo, kChicago, 1740, 240 },
			{ kBuffalo, kChicago, 2010, 220 },
		},
		// To Buffalo
		{ 0 }
	}
};

#endif