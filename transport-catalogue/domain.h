#pragma once

#include "geo.h"
#include <string>
#include <vector>

namespace catalogue
{
	struct Stop_
	{
		std::string name;
		geo::Coordinates coord;
	};
	using STOPS = std::vector<const Stop_*>;

	struct Bus_
	{
		std::string num;		
		STOPS route;
		bool is_round;
	};
	using BUSES = std::vector<const Bus_*>;

	struct Stats
	{
		int stops;
		int unique_stops;
		double dist;
		double curv;
	};

	STOPS MakeFullRoute(const Bus_* bus);
}
