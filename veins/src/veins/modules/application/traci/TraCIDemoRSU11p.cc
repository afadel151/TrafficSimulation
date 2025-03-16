#include "veins/modules/application/traci/TraCIDemoRSU11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/application/traci/Request_m.h"
//#include "veins/modules/application/traci/Reply_m.h"




#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <omnetpp.h>
#include <unordered_map>

using namespace veins;
using namespace std;
using namespace omnetpp;


Define_Module(veins::TraCIDemoRSU11p);

bool TraCIDemoRSU11p::isFlagTrueForMovieID(int movieID) {
    for (const auto& row : dataRSU) {
        if (std::get<0>(row) == movieID) {
            return std::get<3>(row);
        }
    }
    return false; // Retourner false si movieID n'est pas trouvé
}


float TraCIDemoRSU11p::getDurationById(int id) {
    for (const auto& row : dataRSU) {
        if (std::get<0>(row) == id) {
            return std::get<1>(row);
        }
    }
    // Si l'ID n'est pas trouvé
    return -1;
}


std::string TraCIDemoRSU11p::getDataById(int id) {
    for (const auto& row : *cacheRSU) {
        if (std::get<0>(row) == id) {
            return std::get<2>(row);
        }
    }
    // Si l'ID n'est pas trouvé
    return "NoData";
}


bool TraCIDemoRSU11p::canAddMovieToCache(float durationNew) {
    float sumDurations = 0;
        for (const auto& movie : *cacheRSU) {
            sumDurations += std::get<1>(movie);
            //EV << "sumDurations "<<sumDurations<<endl;
        }
        return (cacheSize >= sumDurations + durationNew);
    }


void TraCIDemoRSU11p::addMovieToCache(int idMovieNew, float durationNew, string demoData) {
    // Tant que le nouveau film ne peut pas être ajouté, on effectue un remplacement
    while (!canAddMovieToCache(durationNew)) {
        EV << "We can't add the movie " << idMovieNew << " because the cache is full" << endl;
        EV << "Call algo of cache replacement " << endl;
        // Appliquer un des algorithmes de remplacement de cache
         FIFO();
        // LFU ();
        // LRU();
    }

    // Ajouter le nouveau film au cache
    cacheRSU->push_back(std::make_tuple(idMovieNew, durationNew, demoData));
    EV << "Movie with id " << idMovieNew << " and duration " << durationNew << " and data " << demoData << " added to cache successfuly" << endl;

    // Mettre à jour le temps d'utilisation (LRU) et le compteur d'usage (LFU)
    lastUsedTime[idMovieNew] = simTime();  // Pour LRU
    cacheUsageCount[idMovieNew]++;         // Pour LFU

    // Mettre à jour le flag correspondant à idMovieNew à true
    for (auto& record : dataRSU) {
        if (std::get<0>(record) == idMovieNew) {
            std::get<3>(record) = true;
            EV << "Flag for added movie ID " << idMovieNew << " set to true." << endl;
            break;
        }
    }

    printCacheContents(); // Imprimer le contenu du cache pour vérification
}


void TraCIDemoRSU11p::FIFO() {
    if (!cacheRSU->empty()) {
        int removedMovieID = std::get<0>(cacheRSU->front());
        cacheRSU->erase(cacheRSU->begin()); // Supprime le premier élément du cache (FIFO)

        // Mettre à jour le flag correspondant à removedMovieID à false
        for (auto& record : dataRSU) {
            if (std::get<0>(record) == removedMovieID) {
                std::get<3>(record) = false;
                EV << "Flag for removed movie ID " << removedMovieID << " set to false." << endl;
                break;
            }
        }
    }
}


void TraCIDemoRSU11p::LRU() {
    if (!cacheRSU->empty()) {
        auto leastRecentlyUsed = cacheRSU->begin();
        simtime_t oldestTime = simTime();
        for (auto it = cacheRSU->begin(); it != cacheRSU->end(); ++it) {
            if (lastUsedTime.find(std::get<0>(*it)) != lastUsedTime.end() &&
                lastUsedTime[std::get<0>(*it)] < oldestTime) {
                oldestTime = lastUsedTime[std::get<0>(*it)];
                leastRecentlyUsed = it;
            }
        }

        int removedMovieID = std::get<0>(*leastRecentlyUsed);
        cacheRSU->erase(leastRecentlyUsed);

        // Mettre à jour le flag correspondant à removedMovieID à false
        for (auto& record : dataRSU) {
            if (std::get<0>(record) == removedMovieID) {
                std::get<3>(record) = false;
                EV << "Flag for removed movie ID " << removedMovieID << " set to false." << endl;
                break;
            }
        }

        EV << "LRU done" << endl;
    }
}


void TraCIDemoRSU11p::LFU() {
    if (!cacheRSU->empty()) {
        int minUsageCount = INT_MAX;
        auto leastFrequent = cacheRSU->begin();
        for (auto it = cacheRSU->begin(); it != cacheRSU->end(); ++it) {
            if (cacheUsageCount[std::get<0>(*it)] < minUsageCount) {
                minUsageCount = cacheUsageCount[std::get<0>(*it)];
                leastFrequent = it;
            }
        }

        int removedMovieID = std::get<0>(*leastFrequent);
        cacheRSU->erase(leastFrequent);

        // Mettre à jour le flag correspondant à removedMovieID à false
        for (auto& record : dataRSU) {
            if (std::get<0>(record) == removedMovieID) {
                std::get<3>(record) = false;
                EV << "Flag for removed movie ID " << removedMovieID << " set to false." << endl;
                break;
            }
        }

        EV << "LFU done" << endl;
    }
}


void TraCIDemoRSU11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
if(stage ==0)
{
    // Ouvrir le fichier CSV
/*    ifstream file("movies_metadata.csv");

    if (!file.is_open()) {
        EV_ERROR << "Impossible d'ouvrir le fichier CSV." << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        // Utiliser un flux de chaînes pour extraire les valeurs de chaque ligne
        stringstream ss(line);
        string value;
        tuple<int, float, float, bool> row;
        // Lire et convertir chaque valeur séparée par une virgule
        bool validRow = true;  // Indicateur pour vérifier si la ligne est valide
        // Lire l'ID du film
        getline(ss, value, ',');
        try {
            get<0>(row) = stoi(value);
        } catch (const invalid_argument& e) {
            EV_ERROR << "Invalid argument for movie ID: " << value << endl;
            validRow = false;
        } catch (const out_of_range& e) {
            EV_ERROR << "Out of range value for movie ID: " << value << endl;
            validRow = false;
        }
        // Lire la durée
        if (validRow) {
            getline(ss, value, ',');
            try {
                get<1>(row) = stof(value);
            } catch (const invalid_argument& e) {
                EV_ERROR << "Invalid argument for duration: " << value << endl;
                validRow = false;
            } catch (const out_of_range& e) {
                EV_ERROR << "Out of range value for duration: " << value << endl;
                validRow = false;
            }
            getline(ss, value, ',');
            try {
                get<2>(row) = stof(value);
            } catch (const invalid_argument& e) {
                EV_ERROR << "Invalid argument for rating: " << value << endl;
                validRow = false;
            } catch (const out_of_range& e) {
                EV_ERROR << "Out of range value for rating: " << value << endl;
                validRow = false;
            }

            get<3>(row) = false;
            dataRSU.push_back(row);

        }// valid row

    }// while

    file.close();
*/

    tuple<int, float, float,  bool> row;
            get<0>(row) = 1; //id
            get<1>(row) = 81; //duration
            get<2>(row) = 3.92; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 2;
            get<1>(row) = 104; //duration
            get<2>(row) = 3.43; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 3; //id
            get<1>(row) = 101; //duration
            get<2>(row) = 3.25; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 4; //id
            get<1>(row) = 124; //duration
            get<2>(row) = 2.35; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 5;
            get<1>(row) = 106; //duration
            get<2>(row) = 2.07; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);
/*
            get<0>(row) = 6; //id
            get<1>(row) = 170; //duration
            get<2>(row) = 2.01; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 7; //id
            get<1>(row) = 127; //duration
            get<2>(row) = 1.97; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 8; //id
            get<1>(row) = 97; //duration
            get<2>(row) = 1.8; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 9; //id
            get<1>(row) = 111; //duration
            get<2>(row) = 1.6; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);

            get<0>(row) = 10; //id
            get<1>(row) = 130; //duration
            get<2>(row) = 1.52; //rating
            get<3>(row) = false; //flag
            dataRSU.push_back(row);
*/
            EV << "Hello, i'm the véhicule: "<< getId() << endl;

    // Afficher les données pour vérification après le tri
    EV << "Données RSU : " << endl;
    for (const auto& row : dataRSU) {
            EV << "ID: " << get<0>(row) << "\t"
               << "Duration: " << get<1>(row) << "\t"
               << "Rating: " << get<2>(row) << "\t"
               << "Flag: " << get<3>(row) << endl;
        }


}
stage == 1;
}

void TraCIDemoRSU11p::printCacheContents() {
    EV << "Current cacheRSU "<<getId()<<" contents: " << endl;
    for (const auto& movie : *cacheRSU) {
        EV << "Movie ID: " << std::get<0>(movie) << ", Duration: " << std::get<1>(movie) << ", Data: " << std::get<2>(movie) << endl;
    }
}


LAddress::L2Type TraCIDemoRSU11p::getAdress(){

    return myId;
}




void TraCIDemoRSU11p::onWSM(BaseFrame1609_4* frame)
{
Request* wsm = check_and_cast<Request*>(frame);

EV << "RSU : " << wsm->getName()<< "   " << wsm->getReceiverType();

if(strcmp(wsm->getReceiverType(), "r") == 0)
{
    cModule* mbs = getSimulation()->getModule(idMBS);
    TraCIDemoMBS11p* b = check_and_cast<TraCIDemoMBS11p*>(mbs);

    if (strcmp(wsm->getName(),"RequestContent") == 0)
    {


            //V <<"RSU " << getId()<<" Message type: RequestContent "  <<  endl;
            int movieID = wsm->getIdMovieWants();
            //EV <<"movieID:  " << movieID<<  endl;

            float durationNew = getDurationById(movieID);
            //EV <<"durationNew:  " << durationNew<<  endl;

            bool flag = isFlagTrueForMovieID(movieID); //retourne true si flag(exist_cache) est vrai pour le id correspendant
            EV <<"I'm the RSU " << getId()<< " and the flag for movieID " << movieID << " is: " << (flag ? "true" : "false") << endl;

            int destNodeId = wsm->getIdSender();
            //EV <<"destNodeId:  " << destNodeId<<  endl;

            cModule* destModule = getSimulation()->getModule(destNodeId);
            LAddress::L2Type vecMac = wsm->getSenderAddress();
            //EV<< "in rsu, sender adress : "<<vecMac;


            if(flag == true){
                EV <<"RSU: flag=true so send data to node"<< vecMac  <<  endl;
                cache_hit_RSU++;
                b->addResult(1, myId, movieID, flag, simTime().dbl()); //flag=ture so hit
                b->printResults();

                std::string binaryData =  getDataById(movieID);
                const char* BD = binaryData.c_str();

                Request* rep = new Request("DataAvailable");
                //populateWSM(rep, vecMac);
                populateWSM(rep);
                rep->setIdSender(getId());
                rep->setIdMovieNew(movieID);
                rep->setDurationNew(durationNew);
                rep->setDemoData(BD);
                rep->setReceiverType(wsm->getSenderType());
                rep->setSenderType("r");
                //sendDirect(rep,destModule,"lowerLayerOut");
                sendDown(rep);
            }
            else{
                EV <<"RSU: flag=false so send NoDataAvailable to node" <<  vecMac <<  endl;
                cache_miss_RSU++;
                b->addResult(1, myId, movieID, flag, simTime().dbl()); //flag=false so miss
                b->printResults();

                //EV <<"RSU send a message to the sender vehicule to tell him that he don't have the data" <<  endl;

                Request* rep = new Request("NoDataAvailable");
                populateWSM(rep, vecMac);

                //populateWSM(rep);
                rep->setIdSender(getId());
                rep->setIdMovieWants(movieID);
                rep->setSenderAddress(myId);
                rep->setSenderType("r");
                rep->setReceiverType(wsm->getSenderType());
                //sendDirect(rep,destModule,"lowerLayerIn");
                sendDown(rep);

                EV <<"RSU send a content request to the MBS " <<idMBS<<  endl;


                Request* reqMBS = new Request("RequestContent");
                //cModule* mbs = getSimulation()->getModule(idMBS);
                //TraCIDemoMBS11p* b = check_and_cast<TraCIDemoMBS11p*>(mbs);
                //EV <<"mbs " <<b->getAdress()<< endl;

                populateWSM(reqMBS, b->getAdress());
                //populateWSM(reqMBS, idMBS);
                reqMBS->setIdMovieWants(movieID);
                reqMBS->setIdSender(getId());
                reqMBS->setSenderAddress(myId);
                reqMBS->setReceiverType("m");
                reqMBS->setSenderType("r");
                sendDown(reqMBS);
                //sendDirect(reqMBS, mbs->gate("veinsradioIn"));
            }
        }



    else if(strcmp(wsm->getName(),"DataAvailable") == 0 && (wsm->getRecipientAddress() == myId))
    {
        EV <<"RSU " << getId()<<" Message type: DataAvailable "  <<  endl;

        EV <<"Downloading data form " << wsm->getIdSender()<<" to me: "<< getId()<< endl;
        int idMovieNew = wsm->getIdMovieNew();
        float durationNew = wsm->getDurationNew();
        string demoData = wsm->getDemoData();

        EV<<"durationNew: "<<durationNew<< "can i add movie to cache ? "<< canAddMovieToCache(durationNew)<<endl;
        addMovieToCache(idMovieNew,durationNew,demoData);

    }

    else{

        EV <<"Other message type in rsu " << wsm->getName()<< endl;
    }


    //delete frame;
}
}




void TraCIDemoRSU11p::onWSA(DemoServiceAdvertisment* wsa)
{
    //if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
    }
}
