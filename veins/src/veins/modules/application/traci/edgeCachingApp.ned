package org.car2x.veins.modules.application.traci;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple edgeCachingApp extends DemoBaseApplLayer
{
    parameters:
        int cacheSize = 200;
        double skewness = default(0.5);
        double epsilon = default(0.01);
        int time_stamp = default(15);
        
    @class(veins::edgeCachingApp);
    @display("i=block/app2");
}