#include "transport_catalogue.h"
namespace catalogue
{
	void TransportCatalogue::AddStop(const std::string& stop_name, geo::Coordinates coor)
	{
		stops_.push_back({ stop_name,coor });
		stop_name_key_.insert({ stops_.back().name, &stops_.back() });
		stops_routes_.insert({ &stops_.back(),{} });
	}

	void TransportCatalogue::AddStopsDist(const std::string& stop_from, const std::string& stop_to, double dist)
	{
		stop_dist_.insert({ {stop_name_key_.at(stop_from),stop_name_key_.at(stop_to)},dist });
	}

	void TransportCatalogue::AddRoute(const std::string& bus_num, const std::vector<std::string>& route, bool is_round)
	{
		STOPS stops;
		for (auto stop : route)
		{
			stops.push_back(stop_name_key_.at(stop));
		}
		buses_.push_back({ bus_num,std::move(stops),is_round });
		bus_route_key_.insert({ buses_.back().num, &buses_.back() });
		for (auto stop : route)
		{
			stops_routes_.at(stop_name_key_.at(stop)).insert(buses_.back().num);
		}
	}

	std::optional<const Bus_*> TransportCatalogue::GetBusInfo(std::string_view bus) const
	{
		return GetInfo(bus_route_key_, bus);
	}

	std::optional<const Stop_*> TransportCatalogue::GetStopInfo(std::string_view stop) const
	{
		return GetInfo(stop_name_key_, stop);
	}

	const ROUTES* TransportCatalogue::GetStopRoutes(const Stop_* stop) const
	{
		return &stops_routes_.at(stop);
	}

	std::optional<double> TransportCatalogue::GetStopsDist(STOP_PAIR stops) const
	{
		return GetInfo(stop_dist_, stops);
	}

	BUSES TransportCatalogue::GetAllBuses(bool not_empty = false) const
	{
		BUSES buses;
		for (auto bus : bus_route_key_)
		{
			if (!not_empty || !bus.second->route.empty())
			{
				buses.push_back(bus.second);
			}
		}
		return std::move(buses);
	}

	STOPS TransportCatalogue::GetAllStops(bool not_empty = false) const
	{
		STOPS stops;
		for (auto stop : stop_name_key_)
		{
			if (!not_empty || !stops_routes_.at(stop.second).empty())
			{
				stops.push_back(stop.second);
			}
		}
		return std::move(stops);
	}

	std::optional<Stats> TransportCatalogue::GetStat(const std::string& bus_name) const
	{		
		auto bus = GetBusInfo(bus_name);

		if (bus)
		{
			STOPS stops = MakeFullRoute(*bus);
			int all_stops = stops.size();

			std::unordered_set<const Stop_*> uniques;
			uniques.insert(stops.begin(), stops.end());
			int unq_stops = uniques.size();

			double dist_fly = 0;
			double dist_ride = 0;
			for (int i = 0; i < stops.size() - 1; i++)
			{
				dist_fly += geo::ComputeDistance(stops[i]->coord, stops[i + 1]->coord);
				auto dist = GetStopsDist({ stops[i],stops[i + 1] });
				if (dist)
				{
					dist_ride += *dist;
				}
				else
				{
					dist_ride += stop_dist_.at({ stops[i + 1],stops[i] });
				}
			}
			return Stats{ all_stops,unq_stops,dist_ride,dist_ride / dist_fly };
		}
		else
		{
			return std::nullopt;
		}
	}	
}