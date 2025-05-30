// Automobile.cpp - Implementation of the Automobile class
//
// 12-Mar-19  M. Watler         Created.
//

/**
 * Name : Yashasvini Bhanuraj
 * Student ID : 164581217
 * Email : ybhanuraj@myseneca.ca
 */
#include "Automobile.h"
#include "Logger.h"

// The maximum number of characters in a message
// During DEBUG testing, I found that 256 characters was sufficient.
// Before it was 64 characters. I increased it to 256 characters
// It was crashing as the message was too long for the buffer
const int MSG_CHAR_MAX = 256;

Automobile::Automobile(string _make, string _model, string _colour, int _year)
{
    make = _make;
    model = _model;
    colour = _colour;
    year = _year;
    fuelInTank = 0;
}

void Automobile::setFuelEfficiency(double _efficiency)
{
    fuelEfficiency = _efficiency;
}

void Automobile::emptyFuel(void)
{
    fuelInTank = 0;
}

void Automobile::addFuel(double _liters)
{
    fuelInTank += _liters;
    if (fuelInTank > 50)
    {
        fuelInTank = 50; // Cap at 50 liters
        // Log a warning message
        // Changed from 64 to 256 characters
        char message[MSG_CHAR_MAX];
        sprintf(message, "The %s %d %s %s is full of gas. Discarding the rest...\n", colour.c_str(), year, make.c_str(), model.c_str());
        Log(WARNING, __FILE__, __func__, __LINE__, message);
    }
}

void Automobile::drive(double _distance)
{

    double fuelConsumed = fuelEfficiency / 100 * _distance;
    fuelInTank -= fuelConsumed;
    if (fuelInTank < 0)
    {
        fuelInTank = 0;
        // Changed from 64 to 256 characters
        char message[MSG_CHAR_MAX];
        sprintf(message, "The %s %d %s %s has no gas left in the tank\n", colour.c_str(), year, make.c_str(), model.c_str());
        Log(ERROR, __FILE__, __func__, __LINE__, message);
    }
}

void Automobile::displayReport()
{
    cout << "The " << colour << " " << year << " " << make << " " << model << " has " << fuelInTank << " left in the tank" << endl;
}
