//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemoMBS11p.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/application/traci/Request_m.h"
//#include "veins/modules/application/traci/Reply_m.h"




#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <omnetpp.h>
#include <random>
#include <chrono>
#include <ctime>

using namespace veins;
using namespace std;
using namespace omnetpp;

Define_Module(veins::TraCIDemoMBS11p);

LAddress::L2Type TraCIDemoMBS11p::getAdress(){

    return myId;
}


TraCIDemoMBS11p::TraCIDemoMBS11p()
    : gen(std::chrono::system_clock::now().time_since_epoch().count()), // Utiliser l'horodatage en nanosecondes comme graine
      sizeDist(40, 1000),  // Taille aléatoire entre 40 et 1000 caractères
      charDist(65, 90)     // Caractères entre 'A' et 'Z'
{}

const std::vector<std::tuple<bool, int, int, bool, float>>& TraCIDemoMBS11p::getResults() const {
    return results;
}


void TraCIDemoMBS11p::addResult(bool vehRsu, int myId, int movieId, bool exists, float simTime) {
    results.emplace_back(vehRsu, myId, movieId, exists, simTime);
}

void TraCIDemoMBS11p::printResults() const {
    std::cout << "Results Content:" << std::endl;
    for (const auto& result : results) {
        bool vehRsu = std::get<0>(result);
        int myId = std::get<1>(result);
        int movieId = std::get<2>(result);
        bool exists = std::get<3>(result);
        float simTime = std::get<4>(result);

        std::cout << "Type: " << (vehRsu ? "Vehicule" : "RSU")
                  << ", MyID: " << myId
                  << ", MovieID: " << movieId
                  << ", Exists: " << (exists ? "Yes" : "No")
                  << ", SimTime: " << simTime
                  << std::endl;
    }
}

std::string TraCIDemoMBS11p::createLargeBinaryData() {
    // Générer une taille aléatoire pour la chaîne
    int size = sizeDist(gen);
    std::string largeData;

    // Générer des caractères aléatoires pour remplir la chaîne
    for (int i = 0; i < size; ++i) {
        largeData += static_cast<char>(charDist(gen));
    }

    // Afficher des informations sur les données générées
    std::cout << "Generating large binary data" << std::endl;
    std::cout << "Data: " << largeData << std::endl; // Afficher le contenu des données
    std::cout << "Size of generated data: " << largeData.size() << " bytes" << std::endl; // Afficher la taille

    return largeData;
}




void TraCIDemoMBS11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if(stage == 0)
    {
        // Ouvrir le fichier CSV
        /*ifstream file("movies_metadata.csv");

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
            }

            // Lire la note
            if (validRow) {
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
            }

            if (validRow) {

                // Ajouter une colonne exist_cache initialisée à true
                get<3>(row) = true;

                // Ajouter la ligne valide à la structure de données temporaire
                data.push_back(row);
            }
        }

        file.close();
*/


        tuple<int, float, float,  bool, string> row;
                    get<0>(row) = 1; //id
                    get<1>(row) = 81; //duration
                    get<2>(row) = 3.92; //rating
                    get<3>(row) = true; //flag
                    get<4>(row) = createLargeBinaryData() ; //data

                    data.push_back(row);

                    get<0>(row) = 2;
                    get<1>(row) = 104; //duration
                    get<2>(row) = 3.43; //rating
                    get<3>(row) = true; //flag
                    get<4>(row) = createLargeBinaryData() ; //data
                    data.push_back(row);

                    get<0>(row) = 3; //id
                    get<1>(row) = 101; //duration
                    get<2>(row) = 3.25; //rating
                    get<3>(row) = true; //flag
                    get<4>(row) = createLargeBinaryData() ; //data
                    data.push_back(row);

                    get<0>(row) = 4; //id
                    get<1>(row) = 124; //duration
                    get<2>(row) = 2.35; //rating
                    get<3>(row) = true; //flag
                    get<4>(row) = createLargeBinaryData() ; //data
                    data.push_back(row);

                    get<0>(row) = 5;
                    get<1>(row) = 106; //duration
                    get<2>(row) = 2.07; //rating
                    get<3>(row) = true; //flag
                    get<4>(row) = createLargeBinaryData() ; //data
                    data.push_back(row);
/*
                    get<0>(row) = 6; //id
                    get<1>(row) = 170; //duration
                    get<2>(row) = 2.01; //rating
                    get<3>(row) = true; //flag
                    data.push_back(row);

                    get<0>(row) = 7; //id
                    get<1>(row) = 127; //duration
                    get<2>(row) = 1.97; //rating
                    get<3>(row) = true; //flag
                    data.push_back(row);

                    get<0>(row) = 8; //id
                    get<1>(row) = 97; //duration
                    get<2>(row) = 1.8; //rating
                    get<3>(row) = true; //flag
                    data.push_back(row);

                    get<0>(row) = 9; //id
                    get<1>(row) = 111; //duration
                    get<2>(row) = 1.6; //rating
                    get<3>(row) = true; //flag
                    data.push_back(row);

                    get<0>(row) = 10; //id
                    get<1>(row) = 130; //duration
                    get<2>(row) = 1.52; //rating
                    get<3>(row) = true; //flag
                    data.push_back(row);
*/
                    EV << "Hello, i'm the véhicule:"<< getId() << endl;


        // Afficher les données pour vérification après le tri
        EV << "Données MBS :" << endl;
        for (const auto& row : data) {
            EV << "ID: " << get<0>(row) << "\t"
                    << "Duration: " << get<1>(row) << "\t"
                    << "Rating: " << get<2>(row) << "\t"
                    << "Flag: " << get<3>(row) << "\t"
                    << "data: " << get<4>(row) << endl;
        }

        /*Request* reqID = new Request("ID MBS");
        populateWSM(reqID);
        reqID->setIdSender(getId());
        sendDown(reqID);*/
    }
    stage = 1;
}


float TraCIDemoMBS11p::getDurationById(int id) {
    for (const auto& row : data) {
        if (std::get<0>(row) == id) {
            return std::get<1>(row);
        }
    }
    // Si l'ID n'est pas trouvé
    return -1;
}


std::string TraCIDemoMBS11p::getDataById(int id) {
    for (const auto& row : data) {
        if (std::get<0>(row) == id) {
            return std::get<4>(row);
        }
    }
    // Si l'ID n'est pas trouvé
    return "No data";
}


void TraCIDemoMBS11p::onWSM(BaseFrame1609_4* frame)
{
Request* wsm = check_and_cast<Request*>(frame);

EV << "MBS : " << wsm->getName()<< " ReceiverType " << wsm->getReceiverType() <<" SenderType " << wsm->getSenderType();
if(strcmp(wsm->getReceiverType(), "m") == 0)
{
    //EV <<"I'm the MBS  " << getId() <<endl;
    if(strcmp(wsm->getName(),"RequestContent") == 0)
    {
            //EV <<"MBS recieves a RequestContent " <<endl;

            Request* rep = new Request("DataAvailable");

            int movieID = wsm->getIdMovieWants();
            int destNodeId = wsm->getIdSender();
            std::string binaryData =  getDataById(movieID);
            const char* BD = binaryData.c_str();



            populateWSM(rep, wsm->getSenderAddress());
            rep->setIdSender(getId());
            rep->setDemoData(BD);
            rep->setReceiverType(wsm->getSenderType());
            //rep->setReceiverType("v");
            rep->setIdMovieNew(wsm->getIdMovieWants());
            rep->setDurationNew(getDurationById(wsm->getIdMovieWants()));
            sendDown(rep);
            //sendDirect(rep,destModule,"lowerLayerOut");

            EV <<" Sending data to destNodeId : " << destNodeId <<endl;
        }



    else{
        EV <<"Another message type for mbs : " << wsm->getName() << endl;

    }
}
}


void TraCIDemoMBS11p::onWSA(DemoServiceAdvertisment* wsa)
{
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(static_cast<Channel>(wsa->getTargetChannel()));
        //EV <<"I'm the MBS " << getId()<< " and i'm going to send the data " <<  endl;
    }
}


void TraCIDemoMBS11p::writeResultsToCSV(const std::string& filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        // Write headers
        file << "Vehicle/Rsu,NodeID,MovieID,Exists,SimTime\n";

        // Write each tuple in results
        for (const auto& entry : results) {
            file << (std::get<0>(entry) ? "Vehicle" : "Rsu") << ","
                 << std::get<1>(entry) << ","
                 << std::get<2>(entry) << ","
                 << (std::get<3>(entry) ? "true" : "false") << ","
                 << std::get<4>(entry) << "\n";
        }

        file.close();
        std::cout << "Results written to " << filename << " successfully." << std::endl;
    } else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}


void TraCIDemoMBS11p::finish() {
    // Write results to CSV file
    writeResultsToCSV("simulation_results.csv");
    // Call the base class finish method if needed
    DemoBaseApplLayer::finish();
}
