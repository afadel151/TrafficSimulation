import xml.etree.ElementTree as ET
import json

tree = ET.parse("osm.poly.xml")
root = tree.getroot()

geojson_data = {
    "type": "FeatureCollection",
    "features": []
}

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
            "type": poly.get("type")
        }
    })

with open("osm_poly.geojson", "w") as f:
    json.dump(geojson_data, f, indent=4)

print("Exported SUMO polygons to GeoJSON.")
