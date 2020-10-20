//#define DEBUG

#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include <unistd.h>
#include <chrono>
#include <ctime>    

#define PIN 8
#define TIMEOUTTIME 5.0

auto start = std::chrono::system_clock::now();
bool changing = false;
bool countTime = false;

void ScreenTurnOff()
{
	system("vcgencmd display_power 0");
}

void ScreenTurnOn()
{
	system("vcgencmd display_power 1");
}

void PinValueChanged()
{
	if (digitalRead(PIN))
	{
#ifdef DEBUG
		std::cout << "PIN value: " << digitalRead(PIN) << std::endl;
		std::cout << "Detected" << std::endl;
#endif // DEBUG

		ScreenTurnOn();

#ifdef DEBUG
		std::cout << "Screen turned on" << std::endl;
#endif // DEBUG

		countTime = false;
	}
	else
	{
#ifdef DEBUG
		std::cout << "PIN value: " << digitalRead(PIN) << std::endl;
		std::cout << "Undetected" << std::endl;
#endif // DEBUG

		while (changing) {};
		changing = true;
		start = std::chrono::system_clock::now();
		changing = false;

		countTime = true;
	}
}

int main()
{
	wiringPiSetup();
	pinMode(PIN, INPUT);

	// Bind to interrupt
	wiringPiISR(PIN, INT_EDGE_BOTH, &PinValueChanged);

	while(1)
	{
		if (countTime && !changing)
		{
			changing = true;
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsedSeconds = end - start;
			changing = false;

			if (elapsedSeconds.count() > TIMEOUTTIME)
			{
				ScreenTurnOff();

#ifdef DEBUG
				std::cout << "Screen turned off" << std::endl;
#endif // DEBUG

				countTime = false;
			}
		}
		sleep(1);
	}

    return 0;
}