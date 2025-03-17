import xml.etree.ElementTree as ET
import json

# Load the SUMO poly file
poly_file = "C:/Users/FADEL/Desktop/sumo-win64-1.8.0/sumo-1.8.0/tools/2025-03-16-23-15-31/osm.poly.xml"


tree = ET.parse(poly_file)
root = tree.getroot()

geojson_data = {
    "type": "FeatureCollection",
    "features": []
}

# Iterate through polygons in .poly.xml
for poly in root.findall("poly"):
    points = poly.get("shape").split()
    coordinates = [[float(x), float(y)] for x, y in (point.split(",") for point in points)]
    
    geojson_data["features"].append({
        "type": "Feature",
        "geometry": {
            "type": "Polygon",
            "coordinates": [coordinates]
        },
        "properties": {
            "id": poly.get("id"),
            "type": poly.get("type", "unknown")  # Default to "unknown" if type isn't specified
        }
    })

# Save as GeoJSON
output_file = "osm_poly.geojson"
with open(output_file, "w") as f:
    json.dump(geojson_data, f, indent=4)

print(f"Exported SUMO polygons to GeoJSON: {output_file}")
