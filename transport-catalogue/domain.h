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
		size_t id;
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
	
	struct IdealRouteItem
	{		
		std::string bus_stop;
		double time;
		size_t span;
	};

	/*
	<route time, vector<bus or stop name : span time : span count>>
	*/
	using IDEAL_ROUTE = std::pair<double,std::vector<IdealRouteItem>>;	

	STOPS MakeFullRoute(const Bus_* bus);
}
