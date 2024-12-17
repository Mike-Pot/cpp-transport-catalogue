#include "transport_router.h"

namespace rout
{
	Builder& Builder::SetGraph(const catalogue::TransportCatalogue& db_, int wait_time, double velocity)
	{
		using namespace catalogue;		
		BUSES all_buses = db_.GetAllBuses();
		for (auto& bus : all_buses)
		{
			AddRoute({ db_, bus, 0 }, velocity, true);
			if (!bus->is_round)
			{
				AddRoute({ db_, bus, bus->route.size() - 1 }, velocity, false);			
			}			
		}
		
		STOPS all_stops = db_.GetAllStops();
		for (size_t i = 0; i < all_stops.size(); i++)
		{
			edge_inf.insert({
				route_graph.AddEdge({ all_stops[i]->id + 1,all_stops[i]->id, (double)wait_time }),
				{ 0,all_stops[i]->name} });
		}
		
		return *this;		
	}

	Data Builder::GetGraph()
	{		
		return { route_graph,edge_inf };
	}
	
	void Builder::AddRoute(Params params, double velocity, bool forth)
	{
		const catalogue::STOPS& stops = params.bus->route;
		int stop_from = (int)params.stop_from;
		int step = forth ? 1 : -1;
		int last_ind = forth ? (int)stops.size() - 1 : 0;

		for (int j = stop_from + step; step*j <= last_ind; j += step)
		{
			if (stops[stop_from] == stops[j])
			{
				continue;
			}
			edge_inf.insert({
				route_graph.AddEdge({ stops[stop_from]->id,stops[j]->id + 1,
				ComputeDist(params, j) * 3. / (velocity * 50.) }),
				{ std::abs(stop_from - j),params.bus->num} });		
		}
		if (step * stop_from < last_ind)
		{
			params.stop_from += step;
			AddRoute(params, velocity, forth);
		}
	}

	double Builder::ComputeDist(Params params, size_t stop_to) const
	{
		const catalogue::STOPS& stops = params.bus->route;		
		int step = stop_to > params.stop_from ? 1 : -1;
		double res = 0.;

		for (int k = (int)params.stop_from; step*k < step*(int)stop_to; k += step)
		{
			auto dist = params.db_.GetStopsDist({ stops[k],stops[k + step] });
			if (dist)
			{
				res += *dist;
			}
			else
			{
				res += *params.db_.GetStopsDist({ stops[k + step],stops[k] });
			}			
		}		
		return res;
	}
	
	std::optional<catalogue::IDEAL_ROUTE> TransportRouter::GetRoute(size_t stop_from, size_t stop_to) const
	{
		using namespace catalogue;
		auto route = router_.BuildRoute(stop_from+1, stop_to+1);
		IDEAL_ROUTE res;
		if (!route)
		{
			return std::nullopt;
		}
		res.first = route->weight;
		for (size_t edge : route->edges)
		{
			auto edge_info = data_.edge_inf_.at(edge);
			res.second.push_back({ 
				edge_info.second,
				data_.route_graph_.GetEdge(edge).weight,
				edge_info.first });
		}
		return res;
	}
}
