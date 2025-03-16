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

#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <string>
#include <vector>
#include <tuple>
#include <string>
#include <random>

using namespace std;

namespace veins {

/**
 * Small MBS Demo using 11p
 */
class VEINS_API TraCIDemoMBS11p : public DemoBaseApplLayer {

private:
    // Déclarer une structure de données pour stocker les valeurs du fichier CSV
    vector<tuple<int,float,float,bool,string>> data;
    // Déclarer le générateur et les distributions comme attributs de classe
        std::mt19937 gen; // Générateur de nombres aléatoires
        std::uniform_int_distribution<> sizeDist; // Distribution pour la taille
        std::uniform_int_distribution<> charDist; // Distribution pour les caractères

public:
    void initialize(int stage) override;
    LAddress::L2Type getAdress();
    TraCIDemoMBS11p();
    vector<tuple<bool,int,int, bool, float>> results; //0 veh/1 rsu, myid, movieID, exist or not, simtime()

    const std::vector<std::tuple<bool, int, int, bool, float>>& getResults() const;
    void addResult(bool vehRsu, int myId, int movieId, bool exists, float simTime);
    void printResults() const;


protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    float getDurationById(int id);
    std::string getDataById(int id);


    std::string createLargeBinaryData();
    void writeResultsToCSV(const std::string& filename);
    void finish();
};



} // namespace veins
