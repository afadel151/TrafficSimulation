import xml.etree.ElementTree as ET
import json

# Load the trips file
trips_file = "2025-03-16-23-15-31/osm.passenger.trips.xml"
net_file = "2025-03-16-23-15-31/osm.net.xml"  # Needed to get coordinates

# Parse the trip file
tree = ET.parse(trips_file)
root = tree.getroot()

geojson_data = {
    "type": "FeatureCollection",
    "features": []
}

# Load network file to get edge coordinates
net_tree = ET.parse(net_file)
net_root = net_tree.getroot()
edge_coords = {}

# Extract edge positions from the network file
for edge in net_root.findall("edge"):
    lane = edge.find("lane")
    if lane is not None:
        shape = lane.get("shape")
        if shape:
            coords = [[float(x), float(y)] for x, y in (p.split(",") for p in shape.split())]
            edge_coords[edge.get("id")] = coords[0]  # Take first point of the edge

# Convert trips to GeoJSON
for trip in root.findall("trip"):
    from_edge = trip.get("from")
    to_edge = trip.get("to")

    if from_edge in edge_coords and to_edge in edge_coords:
        geojson_data["features"].append({
            "type": "Feature",
            "geometry": {
                "type": "LineString",
                "coordinates": [edge_coords[from_edge], edge_coords[to_edge]]
            },
            "properties": {
                "id": trip.get("id"),
                "depart": trip.get("depart")
            }
        })

# Save as GeoJSON
output_file = "osm_trips.geojson"
with open(output_file, "w") as f:
    json.dump(geojson_data, f, indent=4)

print(f"Exported passenger trips to GeoJSON: {output_file}")
