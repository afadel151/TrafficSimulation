//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/TraCIDemoMBS11p.h"
#include <unordered_map>

#include <omnetpp.h>
#include <vector>
#include <tuple>
#include <string>
#include <random>
#include <unordered_map>


using namespace omnetpp;
using namespace std;

namespace veins {

/**
 * @brief
 * A tutorial demo for TraCI. When the car is stopped for longer than 10 seconds
 * it will send a message out to other cars containing the blocked road id.
 * Receiving cars will then trigger a reroute via TraCI.
 * When channel switching between SCH and CCH is enabled on the MAC, the message is
 * instead send out on a service channel following a Service Advertisement
 * on the CCH.
 *
 * @author Christoph Sommer : initial DemoApp
 * @author David Eckhoff : rewriting, moving functionality to DemoBaseApplLayer, adding WSA
 *
 */


class VEINS_API edgeCachingApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    LAddress::L2Type getAdress();

protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;

    int requestPeriod;

    vector<tuple<int,float,string>> *cache = new vector<tuple<int,float,string>>(); //id et duree et data

    int cacheSize = 250;


    bool flag = false;
    int time_stamp;

    int cache_hit = 0;
    int cache_miss = 0;

    vector<tuple<int,int,float,float,float,bool>> request_probability;

    //std::random_device rd;
    std::mt19937 gen; // Générateur de nombres aléatoires


    int idMBS = 27;

    vector<tuple<int, float, float>> requestTimes;  //idreq, temps debut,temps fin
    int requestIdCounter = 0;

    std::unordered_map<int, int> cacheUsageCount; // Pour LFU
    std::unordered_map<int, simtime_t> lastUsedTime; // Pour LRU

protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;


    void handlePositionUpdate(cObject* obj) override;

    double calculateProbability(int k, int N, double epsilon, double rho);
    std::string getRandomTarget();

    int getRandomIndexByProbability();
    bool isFlagTrueForMovieID(int movieID);
    float getDurationById(int id);
    std::string getDataById(int id);

    void writeVectorToCSV(const std::vector<std::tuple<int, float, float>>& data, const std::string& filename);
    bool canAddMovieToCache(float durationNew);
    void addMovieToCache(int idMovieNew, float durationNew, string demoData);
    void FIFO();
    void LRU();
    void LFU();

    void printRequestTimes();
    void printCacheContents();



};

} // namespace veins
