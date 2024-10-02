#include "stat_reader.h"

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
    std::ostream& output)
{    
    std::string_view com = request.substr(0, request.find_first_of(' '));
    std::string_view req = request.substr(request.find_first_of(' ') + 1);
    if (com == "Bus")
    {
        TransportCatalogue::Stats stat;        
        if (!tansport_catalogue.ShowStat(req, &stat))
        {
            output << request << ": not found" << std::endl;
        }
        else
        {
            output << request << ": " << stat.stops << " stops on route, " << stat.unique_stops << " unique stops, " << std::setprecision(6) << stat.dist << " route length" << std::endl;
        }
    }
    if (com == "Stop")
    {
        const std::set<std::string_view>* buses  = nullptr;
        if (!tansport_catalogue.ShowStop(req, &buses))
        {
            output << request << ": not found" << std::endl;            
        }
        else
        {          
            if (buses->empty())
            {                
                output << request << ": no buses" << std::endl;
            }
            else
            {
                output << request << ": buses";
                for (auto bus : *buses)
                {                    
                    output << " " << bus;
                }
                output << std::endl;
            }
        }
    }
} 
