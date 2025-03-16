#include "veins/modules/application/traci/edgeCachingApp.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/application/traci/Request_m.h"
//#include "veins/modules/application/traci/Reply_m.h"


#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <omnetpp.h>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <unordered_map>



using namespace veins;
using namespace std;
using namespace omnetpp;

Define_Module(veins::edgeCachingApp);


void edgeCachingApp::writeVectorToCSV(const std::vector<std::tuple<int, float, float>>& data, const std::string& filename) {
    // Ouvrir un fichier CSV en mode écriture
    std::ofstream outFile(filename);

    // Vérifier si le fichier est ouvert
    if (!outFile.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier!" << std::endl;
        return;
    }

    // Écrire l'en-tête du fichier CSV
    outFile << "ID,StartTime,EndTime,ProcessingTime\n";

    // Parcourir le vecteur et écrire chaque tuple dans le fichier CSV
    for (const auto& entry : data) {
        int id = std::get<0>(entry);
        float startTime = std::get<1>(entry);
        float endTime = std::get<2>(entry);
        float processingTime = endTime - startTime;

        outFile << id << ","
                << processingTime << "\n";
    }

    // Fermer le fichier
    outFile.close();
}


double edgeCachingApp::calculateProbability(int k, int N, double epsilon = 0, double rho = 1) {
    double numerator = pow(k + epsilon, -rho);
    double denominator = 0;

    for (int i = 1; i <= N; ++i) {
        denominator += pow(i + epsilon, -rho);
    }

    return numerator / denominator;
}


std::string edgeCachingApp::getRandomTarget() {
    std::uniform_int_distribution<> dis(0, 1); // Distribution pour générer 0 ou 1
    int number = dis(gen); // Utiliser le générateur initialisé
    return (number == 0) ? "v" : "r"; // Retourner "v" pour 0 et "r" pour 1
}


int edgeCachingApp::getRandomIndexByProbability() {
    // Extraire les probabilités de request_probability
    std::vector<double> probabilities;

    for (const auto& row : request_probability) {
        probabilities.push_back(std::get<4>(row)); // Récupérer la probabilité
    }

    // Utiliser std::discrete_distribution avec les probabilités extraites
    std::discrete_distribution<> dis(probabilities.begin(), probabilities.end());
    return dis(gen); // Utiliser le générateur initialisé
}


bool edgeCachingApp::isFlagTrueForMovieID(int movieID) {
    for (const auto& row : request_probability) {
        if (std::get<1>(row) == movieID) {
            return std::get<5>(row);
        }
    }
    return false; // Retourner false si movieID n'est pas trouvé
}


float edgeCachingApp::getDurationById(int id) {
    for (const auto& row : request_probability) {
        if (std::get<1>(row) == id) {
            return std::get<2>(row);
        }
    }
    return -1;
}


std::string edgeCachingApp::getDataById(int id) {
    for (const auto& row : *cache) {
        if (std::get<0>(row) == id) {
            return std::get<2>(row);
        }
    }
    return "";
}


bool edgeCachingApp::canAddMovieToCache( float durationNew) {
    float sumDurations = 0;
    for (const auto& movie : *cache) {
        sumDurations += std::get<1>(movie);
        //EV << "sumDurations "<<sumDurations<<endl;
    }
    return (cacheSize >= sumDurations + durationNew);
}


void edgeCachingApp::FIFO() {
    if (!cache->empty()) {
        // Récupérer l'ID de l'élément supprimé (le premier élément du cache)
        int removed_id = std::get<0>(*cache->begin()); // Récupérer l'ID à partir du tuple

        // Supprime le premier élément du cache (FIFO)
        cache->erase(cache->begin());

        // Parcourir request_probability et mettre à jour le flag correspondant
        for (auto& entry : request_probability) {
            int current_id = std::get<1>(entry); // Récupère l'ID

            if (current_id == removed_id) {
                std::get<5>(entry) = false; // Mettre le flag à false
                break; // On peut arrêter la boucle après avoir trouvé et modifié le bon tuple
            }
        }

        EV << "FIFO done and flag set to false for ID: " << removed_id << endl;
    }
}


void edgeCachingApp::LRU() {
    if (!cache->empty()) {
        // Identifier l'élément le moins récemment utilisé
        auto leastRecentlyUsed = cache->begin();
        simtime_t oldestTime = simTime();

        for (auto it = cache->begin(); it != cache->end(); ++it) {
            if (lastUsedTime.find(std::get<0>(*it)) != lastUsedTime.end() &&
                lastUsedTime[std::get<0>(*it)] < oldestTime) {
                oldestTime = lastUsedTime[std::get<0>(*it)];
                leastRecentlyUsed = it;
            }
        }

        // Récupérer l'ID de l'élément supprimé
        int removed_id = std::get<0>(*leastRecentlyUsed); // Assurez-vous que l'ID est accessible comme membre du cache

        // Supprimer l'élément le moins récemment utilisé du cache
        cache->erase(leastRecentlyUsed);

        // Parcourir request_probability et mettre à jour le flag correspondant
        for (auto& entry : request_probability) {
            int current_id = std::get<1>(entry); // Récupère l'ID (deuxième élément du tuple)

            if (current_id == removed_id) {
                std::get<5>(entry) = false; // Mettre le flag (sixième élément) à false
                break; // On peut arrêter la boucle après avoir trouvé et modifié le bon tuple
            }
        }

        EV << "LRU done and flag set to false for ID: " << removed_id << endl;
    }
}


void edgeCachingApp::LFU() {
    if (!cache->empty()) {
        // Identifier l'élément le moins fréquemment utilisé
        int minUsageCount = INT_MAX;
        auto leastFrequent = cache->begin();

        for (auto it = cache->begin(); it != cache->end(); ++it) {
            if (cacheUsageCount[std::get<0>(*it)] < minUsageCount) {
                minUsageCount = cacheUsageCount[std::get<0>(*it)];
                leastFrequent = it;
            }
        }

        // Récupérer l'ID de l'élément supprimé
        int removed_id = std::get<0>(*leastFrequent); // Assurez-vous que l'ID est bien le premier élément du tuple

        // Supprimer l'élément le moins fréquemment utilisé du cache
        cache->erase(leastFrequent);

        // Parcourir request_probability et mettre à jour le flag correspondant
        for (auto& entry : request_probability) {
            int current_id = std::get<1>(entry); // Récupère l'ID (deuxième élément du tuple)

            if (current_id == removed_id) {
                std::get<5>(entry) = false; // Mettre le flag (sixième élément) à false
                break; // On arrête la boucle une fois l'entrée correspondante trouvée et modifiée
            }
        }

        EV << "LFU done and flag set to false for ID: " << removed_id << endl;
    }
}


void edgeCachingApp::addMovieToCache(int idMovieNew, float durationNew, string demoData) {
    while (!canAddMovieToCache(durationNew)) {
        EV << "We can't add the movie " << idMovieNew << " because the cache is full" << endl;
        EV << "Call algo of cache replacement " << endl;
        FIFO();
        //LFU ();
        //LRU();

    }
    cache->push_back(std::make_tuple(idMovieNew, durationNew, demoData));
    EV << "Movie with id " << idMovieNew << " and duration " << durationNew << " and data " << demoData << " added to cache successfuly" << endl;


    lastUsedTime[idMovieNew] = simTime();//pour LRU
    cacheUsageCount[idMovieNew]++;//pour LFU
    //changer la valeur du flag
    for (auto& record : request_probability) {
        if (std::get<1>(record) == idMovieNew) {
            std::get<5>(record) = true;
            EV << "Flag for added movie ID " << idMovieNew << " set to true." << endl;

            break;
        }
    }
}


LAddress::L2Type edgeCachingApp::getAdress(){
    return myId;
}


/*void edgeCachingApp::printRequestTimes() {
    EV << "Current request times:" << endl;
    for (const auto& request : requestTimes) {
        EV << "Request ID: " << std::get<0>(request)<< ", Start Time: " << std::get<1>(request)<< ", End Time: " << std::get<2>(request) << endl;
    }
}*/


void edgeCachingApp::printCacheContents() {
    EV << "Current cache contents:" << endl;
    for (const auto& movie : *cache) {
        EV << "Movie ID: " << std::get<0>(movie)<< ", Duration: " << std::get<1>(movie) <<", Data: " << std::get<2>(movie) << endl;
    }
}


void edgeCachingApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        cacheSize = par("cacheSize");
        time_stamp = par("time_stamp");

        int vehicleId = getId(); // Récupérer l'ID du véhicule
        std::seed_seq seed{static_cast<unsigned int>(vehicleId)}; // Utiliser l'ID comme graine
        gen = std::mt19937(seed); // Initialiser le générateur avec la graine
/*
        gen.seed(getParentModule()->getId());
        //gen.seed(getParentModule()->getId() + std::chrono::system_clock::now().time_since_epoch().count());
        ifstream file("movies_metadata.csv");



        if (!file.is_open()) {
            EV_ERROR << "Impossible d'ouvrir le fichier CSV." << endl;
            return;
        }

        // Utilisation d'un tuple avec six éléments pour inclure le rang (k)
        vector<tuple<int, int, float, float, float, bool>> temp_data;
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string value;
            tuple<int, int, float, float, float, bool> row;

            bool validRow = true;

            // Lire l'ID du film
            getline(ss, value, ',');
            try {
                get<1>(row) = stoi(value);
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
                    get<2>(row) = stof(value);
                } catch (const invalid_argument& e) {
                    EV_ERROR << "Invalid argument for duration: " << value << endl;
                    validRow = false;
                } catch (const out_of_range& e) {
                    EV_ERROR << "Out of range value for duration: " << value << endl;
                    validRow = false;
                }
            }

            // Lire la note
            if (validRow) {
                getline(ss, value, ',');
                try {
                    get<3>(row) = stof(value);
                } catch (const invalid_argument& e) {
                    EV_ERROR << "Invalid argument for rating: " << value << endl;
                    validRow = false;
                } catch (const out_of_range& e) {
                    EV_ERROR << "Out of range value for rating: " << value << endl;
                    validRow = false;
                }
            }

            if (validRow) {
                get<4>(row) = 0.0;   // Probability initialisée à 0.0
                get<5>(row) = false; // Flag initialisé à false
                temp_data.push_back(row);
            }
        }

        file.close();

        // Trier les données par la colonne "note" en ordre décroissant
        std::sort(temp_data.begin(), temp_data.end(), [](const tuple<int, int, float, float, float, bool>& a, const tuple<int, int, float, float, float, bool>& b) {
            return get<3>(a) > get<3>(b); // get<3>(a) correspond à la note
        });

        // Ajouter le rang (k) et calculer la probabilité après le tri
        int N = temp_data.size();
        for (size_t i = 0; i < temp_data.size(); ++i) {
            get<0>(temp_data[i]) = i + 1; // Rang commence à 1
            get<4>(temp_data[i]) = calculateProbability(i + 1 , N); // Calculer la probabilité
        }

        // Copier les données triées dans la structure de données principale
        request_probability = temp_data;

        // Afficher les données pour vérification après le tri
        EV << "Données importées du fichier CSV après tri :" << endl;
        for (const auto& row : request_probability) {
            EV << "Rank: " << get<0>(row) << "\t"
                    << "ID: " << get<1>(row) << "\t"
                    << "Duration: " << get<2>(row) << "\t"
                    << "Rating: " << get<3>(row) << "\t"
                    << "Probability: " << get<4>(row) << "\t"
                    << "Flag: " << get<5>(row) << endl;
        }

        // Mettre à jour la couleur du nœud véhicule
        if (!request_probability.empty()) {
            EV << "request probability not empty" << endl;
            getParentModule()->getDisplayString().setTagArg("i", 1, "blue");
            //getParentModule()->bubble("DataSet loaded successfully");
        }

        EV << "cache veh"<< endl;
        printCacheContents();
*/
        tuple<int, int, float, float, float, bool> row;
        get<0>(row) = 1;
        get<1>(row) = 1; //id
        get<2>(row) = 81; //duration
        get<3>(row) = 3.92; //rating
        get<4>(row) = 0.9; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 2;
        get<1>(row) = 2; //id
        get<2>(row) = 104; //duration
        get<3>(row) = 3.43; //rating
        get<4>(row) = 0.87; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 3;
        get<1>(row) = 3; //id
        get<2>(row) = 101; //duration
        get<3>(row) = 3.25; //rating
        get<4>(row) = 0.71; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 4;
        get<1>(row) = 4; //id
        get<2>(row) = 124; //duration
        get<3>(row) = 2.35; //rating
        get<4>(row) = 0.701; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 5;
        get<1>(row) = 5; //id
        get<2>(row) = 106; //duration
        get<3>(row) = 2.07; //rating
        get<4>(row) = 0.66; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

/*        get<0>(row) = 6;
        get<1>(row) = 6; //id
        get<2>(row) = 170; //duration
        get<3>(row) = 2.01; //rating
        get<4>(row) = 0.62; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 7;
        get<1>(row) = 7; //id
        get<2>(row) = 127; //duration
        get<3>(row) = 1.97; //rating
        get<4>(row) = 0.6; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 8;
        get<1>(row) = 8; //id
        get<2>(row) = 97; //duration
        get<3>(row) = 1.8; //rating
        get<4>(row) = 0.55; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 9;
        get<1>(row) = 9; //id
        get<2>(row) = 111; //duration
        get<3>(row) = 1.6; //rating
        get<4>(row) = 0.53; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);

        get<0>(row) = 10;
        get<1>(row) = 10; //id
        get<2>(row) = 130; //duration
        get<3>(row) = 1.52; //rating
        get<4>(row) = 0.44; //proba
        get<5>(row) = false; //flag
        request_probability.push_back(row);
*/
        EV << "Hello, i'm the véhicule: "<< getId() << endl;

        // Afficher les données pour vérification après le tri
                EV << "Données :" << endl;
                for (const auto& row : request_probability) {
                    EV << "Rank: " << get<0>(row) << "\t"
                            << "ID: " << get<1>(row) << "\t"
                            << "Duration: " << get<2>(row) << "\t"
                            << "Rating: " << get<3>(row) << "\t"
                            << "Probability: " << get<4>(row) << "\t"
                            << "Flag: " << get<5>(row) << endl;
                }

                // Mettre à jour la couleur du nœud véhicule
                if (!request_probability.empty()) {
                    EV << "request probability not empty" << endl;
                    getParentModule()->getDisplayString().setTagArg("i", 1, "blue");
                    //getParentModule()->bubble("DataSet loaded successfully");
                }

                EV << "cache veh"<< endl;
                printCacheContents();


        scheduleAt(simTime() + time_stamp, new Request("RequestContentTrigger"));

    }
    stage = 1;

}


void edgeCachingApp::onWSM(BaseFrame1609_4* frame)
{
Request* wsm = check_and_cast<Request*>(frame);
EV << "Vehicule : " << wsm->getName()<< "   " << wsm->getReceiverType();
if(strcmp(wsm->getReceiverType(), "v") == 0)
{
    cModule* mbs = getSimulation()->getModule(idMBS);
    TraCIDemoMBS11p* b = check_and_cast<TraCIDemoMBS11p*>(mbs);


    if(strcmp(wsm->getName(),"RequestContent") == 0)
    {
            EV <<"Vehicule: Request content form " << wsm->getIdSender()<<" to me: "<< getId()<< endl;
            int movieID = wsm->getIdMovieWants();
            float durationNew = getDurationById(movieID);
            bool flag = isFlagTrueForMovieID(movieID); //retourne true si flag(exist_cache) est vrai pour le id correspendant
            //EV <<"I'm the node " << getId()<< " and the flag for movieID " << movieID << " is: " << (flag ? "true" : "false") << endl;
            int destNodeId = wsm->getIdSender();
            //EV <<"destNodeId : " << destNodeId <<endl;

            //cModule* destModule = getSimulation()->getModule(destNodeId);

            if (flag){
                EV <<"vehicule: flag=ture " <<endl;
                cache_hit++;
                //Request* rep = new Request("responseToTheRequest");
                Request* rep = new Request("DataAvailable");

                std::string binaryData =  getDataById(movieID);
                const char* BD = binaryData.c_str();

                //liste.add((0;myid; movieid; 1; simtime())): // 1 rsu
                b->addResult(0, myId, movieID, flag, simTime().dbl()); //flag=ture so hit
                b->printResults();

                populateWSM(rep);
                rep->setDemoData(BD);
                rep->setIdSender(getId());
                rep->setIdMovieNew(movieID);
                rep->setDurationNew(durationNew);
                rep->setReceiverType(wsm->getSenderType());
                rep->setSenderType("v");

                sendDown(rep);

                //sendDirect(rep,destModule,"lowerLayerOut");
            }
            else{
                EV <<"vehicule: flag=flase " <<endl;
                cache_miss++;

                //liste.add((0;myid; movieid; 0; simtime())):
                b->addResult(0, myId, movieID, flag, simTime().dbl()); //flag=false so miss
                b->printResults();

                Request* rep = new Request("NoDataAvailable");
                populateWSM(rep);
                rep->setIdSender(getId());
                rep->setSenderType("v");
                rep->setReceiverType(wsm->getSenderType());
                rep->setIdMovieWants(movieID);
                EV <<"vehicule "<<getId()<< " send to vehicule " <<wsm->getSenderAddress() << " no data available for movieID " << movieID << endl;
                sendDown(rep);

                //sendDirect(rep,destModule,"lowerLayerOut");
            }
        }





    else
        if(strcmp(wsm->getName(),"NoDataAvailable") == 0)
        {
            EV <<"No data available from : " << wsm->getIdSender()<<" to me: "<< getId()<< endl;

            EV <<"Vehicule will send request to MBS" <<endl;

            Request* reqMBS = new Request("RequestContent");
            //cModule* mbs = getSimulation()->getModule(idMBS);
            //TraCIDemoMBS11p* b = check_and_cast<TraCIDemoMBS11p*>(mbs);
            //TraCIDemoMBS11p* a = FindModule<TraCIDemoMBS11p*>::findSubModule(mbs->getSubmodule("appl"));

            populateWSM(reqMBS);
            reqMBS->setIdMovieWants(wsm->getIdMovieWants());
            reqMBS->setIdSender(getId());
            //reqMBS->setReceiverType(wsm->getReceiverType());
            reqMBS->setReceiverType("m");
            reqMBS->setSenderType("v");
            reqMBS->setSenderAddress(myId);
            sendDown(reqMBS);

            //EV <<"reqMBS from veh: " << getId()<<endl;

        }

        else
            if (strcmp(wsm->getName(),"DataAvailable") == 0 && (wsm->getRecipientAddress() == myId))
            {
                EV <<"Downloading data form " << wsm->getIdSender()<<" to me: "<< getId()<< endl;
                int idMovieNew = wsm->getIdMovieNew();
                float durationNew = wsm->getDurationNew();
                string demoData = wsm->getDemoData();

                //canAddMovieToCache(durationNew);
                EV<<"idMovieNew: "<<idMovieNew<<" durationNew: "<<durationNew<<" can i add movie to cache? "<<canAddMovieToCache(durationNew)<<endl;
                addMovieToCache(idMovieNew,durationNew,demoData);

                /*for (auto& request : requestTimes) {
                        if (std::get<0>(request) == wsm->getIdMovieNew()) {
                            // Mettre le temps de fin à une valeur très grande
                            std::get<2>(request) = (simTime() + SimTime(1e3)).dbl(); // 1e6 secondes est une valeur très grande
                            break;
                        }
                    }*/

                /*// Mettre à jour le temps de fin dans requestTimes
                std::string binaryData = createLargeBinaryData();
                double endTime = binaryData.size();
               // double endTime = INT_MAX;
                for (auto& request : requestTimes) {
                    if (std::get<0>(request) == wsm->getIdMovieNew()) {
                        // Mettre le temps de fin à une valeur très grande
                        std::get<2>(request) = (simTime() + SimTime(endTime)).dbl(); // 1e6 secondes est une valeur très grande
                        break;
                    }
                }*/

                printCacheContents();

                //printRequestTimes();
            }


    /*else
    {
        EV <<"Other message type : " << wsm->getName()<< endl;
    }*/
}

}


void edgeCachingApp::handleSelfMsg(cMessage* msg)
{
    Request* msgReq = check_and_cast<Request*>(msg);
    if (strcmp(msgReq->getName(), "RequestContentTrigger") == 0) {
        EV << "I'm the node " <<getId() << " and i'm starting to send broadcast request"<< endl;

        std::string target = getRandomTarget();
        const char* cible = target.c_str();
        EV << "The random target is : " << cible << endl;

        int randomIndex = getRandomIndexByProbability();
        while (randomIndex < 1 || randomIndex > 5) {
            randomIndex = getRandomIndexByProbability();
        }


        int movieID = std::get<1>(request_probability[randomIndex]);
        EV << "The movieID wants is: " << movieID << endl;

        Request* req = new Request("RequestContent");

        req->setSenderAddress(myId);

        populateWSM(req);
        req->setIdMovieWants(movieID);
        req->setReceiverType(cible);
        req->setSenderType("v");
        //req->setReceiverType("m");

        req->setIdSender(getId());
        sendDown(req);

        //requestIdCounter++;
        //requestTimes.push_back(std::make_tuple(movieID, simTime().dbl(), 0.0));

        /*if (requestTimes.size() >= 10)
        {
            writeVectorToCSV(requestTimes, "request_times.csv");
            std::cout << "Le fichier CSV a été créé avec succès." << std::endl;
        }
        else
        {
            std::cout << "La taille du vecteur est inférieure à 10." << std::endl;
        }*/

        scheduleAt(simTime() + time_stamp, msg->dup());

    }
}


void edgeCachingApp::onWSA(DemoServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService(static_cast<Channel>(wsa->getTargetChannel()), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}


void edgeCachingApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    /*if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && !sentMessage) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
            sentMessage = true;

            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm);
            //wsm->setDemoData("hello");
            //wsm->setSenderAddress(myId);

            if (dataOnSch) {
                startService(Channel::sch2, 42, "Traffic Information Service");
                scheduleAt(computeAsynchronousSendingTime(1, ChannelType::service), wsm);
            } else {
                sendDown(wsm);
            }
        }
    } else {
        lastDroveAt = simTime();
    }*/
}
