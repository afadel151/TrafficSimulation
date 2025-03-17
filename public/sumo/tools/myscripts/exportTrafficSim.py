import traci
import json

# Connect to a running SUMO simulation
sumoCmd = ["sumo", "-c", "your_sumo_config.sumocfg"]
traci.start(sumoCmd)

geojson_data = {
    "type": "FeatureCollection",
    "features": []
}

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    
    # Extract vehicles
    for veh_id in traci.vehicle.getIDList():
        lon, lat = traci.vehicle.getPosition(veh_id)  # X, Y
        geojson_data["features"].append({
            "type": "Feature",
            "geometry": {
                "type": "Point",
                "coordinates": [lon, lat]
            },
            "properties": {
                "id": veh_id,
                "type": "vehicle",
                "speed": traci.vehicle.getSpeed(veh_id)
            }
        })

    # Extract pedestrians
    for ped_id in traci.person.getIDList():
        lon, lat = traci.person.getPosition(ped_id)
        geojson_data["features"].append({
            "type": "Feature",
            "geometry": {
                "type": "Point",
                "coordinates": [lon, lat]
            },
            "properties": {
                "id": ped_id,
                "type": "pedestrian"
            }
        })

traci.close()

# Save as GeoJSON
with open("traffic_simulation.geojson", "w") as f:
    json.dump(geojson_data, f, indent=4)

print("Exported traffic simulation data to GeoJSON.")
