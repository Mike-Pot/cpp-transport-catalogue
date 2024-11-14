#include "domain.h"
namespace catalogue
{
	STOPS MakeFullRoute(const Bus_* bus)
	{
		STOPS res = bus->route;
		if (!bus->is_round)
		{
			for (auto it = bus->route.rbegin() + 1; it != bus->route.rend(); it++)
			{
				res.push_back(*it);
			}
		}
		return std::move(res);
	}
}
