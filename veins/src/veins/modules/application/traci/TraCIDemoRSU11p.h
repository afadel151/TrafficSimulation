#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/TraCIDemoMBS11p.h"

#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <string>
#include <unordered_map>


using namespace std;


namespace veins {

/**
 * Small RSU Demo using 11p
 */
class VEINS_API TraCIDemoRSU11p : public DemoBaseApplLayer {
protected:
    // Déclarer une structure de données pour stocker les valeurs du fichier CSV
    vector<tuple<int,float,float,bool>> dataRSU;

    vector<tuple<int,float,string>> *cacheRSU = new vector<tuple<int,float,string>>(); //id et duree et data
    int cacheSize = 400;

    int cache_hit_RSU = 0;
    int cache_miss_RSU = 0;


    int idMBS = 27;
    std::unordered_map<int, int> cacheUsageCount; // Pour LFU
    std::unordered_map<int, simtime_t> lastUsedTime; // Pour LRU


public:
    void initialize(int stage) override;
    LAddress::L2Type getAdress();

protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    bool isFlagTrueForMovieID(int movieID);
    float getDurationById(int id);
    std::string getDataById(int id);

    bool canAddMovieToCache(float durationNew);
    void addMovieToCache(int idMovieNew, float durationNew, string demoData);

    void FIFO();
    void LRU();
    void LFU();

    void printCacheContents();
};

} // namespace veins
