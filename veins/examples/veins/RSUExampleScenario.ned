//
// Copyright (C) 2017 Christoph Sommer <sommer@ccs-labs.org>
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

import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Car;
import org.car2x.veins.nodes.MBS;
import org.car2x.veins.nodes.Scenario;

network RSUExampleScenario extends Scenario
{
    @display("bgb=4000,4000");
    submodules:
        rsu[3]: RSU {
            @display("p=150,140;i=device/server;is=l;t=RSU,,black");
                //nicType = default("Nic80211p");        
                //appl[0].typename = default("TraCiDemoRSU11p");
        }

        mbs[1]: MBS {
            @display("p=230,141;i=device/antennatower;is=vl;t=MBS,,black;i2=status/ctrl");
        }
}
