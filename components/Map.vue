<script setup>
import { ref, onMounted } from 'vue'
import { Map, View } from 'ol'
import TileLayer from 'ol/layer/Tile'
import OSM from 'ol/source/OSM'
import VectorLayer from 'ol/layer/Vector'
import VectorSource from 'ol/source/Vector'
import { GeoJSON } from 'ol/format'
import { Style, Fill, Stroke } from 'ol/style'
import xml2js from 'xml2js'

const center = ref([4, 35])
const projection = ref('EPSG:4326')
const zoom = ref(8)
const rotation = ref(0)
const map = ref(null)

const parseXml = async (xmlText) => {
  const parser = new xml2js.Parser()
  const result = await parser.parseStringPromise(xmlText)
  return result
}
const convertNetToGeoJSON = (xmlData) => {
  const features = []

  // Convert edges to GeoJSON features
  xmlData['net']['edge'].forEach(edge => {
    if (edge['lane'] && edge['lane'][0]['$']['shape']) {
      features.push({
        type: 'Feature',
        geometry: {
          type: 'LineString',
          coordinates: edge['lane'][0]['$']['shape'].split(' ').map(coord => coord.split(',').map(Number))
        },
        properties: {
          id: edge['$']['id'],
          type: edge['$']['type'] || 'unknown'
        }
      })
    }
  })

  // Convert junctions to GeoJSON features
  xmlData['net']['junction'].forEach(junction => {
    if (junction['$']['x'] && junction['$']['y']) {
      features.push({
        type: 'Feature',
        geometry: {
          type: 'Point',
          coordinates: [parseFloat(junction['$']['x']), parseFloat(junction['$']['y'])]
        },
        properties: {
          id: junction['$']['id'],
          type: 'junction'
        }
      })
    }
  })

  xmlData['net']['roundabout'].forEach(roundabout => {
    if (roundabout['$']['id']) {
      features.push({
        type: 'Feature',
        geometry: {
          type: 'Point',
          coordinates: [parseFloat(roundabout['$']['x']), parseFloat(roundabout['$']['y'])]
        },
        properties: {
          id: roundabout['$']['id'],
          type: 'roundabout'
        }
      })
    }
  })


  return {
    type: 'FeatureCollection',
    features: features
  }
}
const convertTripsToGeoJSON = (xmlData) => {
  const features = xmlData['routes']['trip'].map(trip => {
    return {
      type: 'Feature',
      geometry: {
        type: 'Point',
        coordinates: [parseFloat(trip['$']['from']), parseFloat(trip['$']['to'])]
      },
      properties: {
        id: trip['$']['id'],
        type: trip['$']['type'],
        depart: trip['$']['depart'],
        departLane: trip['$']['departLane']
      }
    }
  })
  return {
    type: 'FeatureCollection',
    features: features
  }
}
const addLayersToMap = (netData, tripsData, polyData) => {
  console.log('Net Data:', netData)
  // console.log('Trips Data:', tripsData)
  // console.log('Poly Data:', polyData)
  const netGeoJSON = convertNetToGeoJSON(netData)
  const tripsGeoJSON = convertTripsToGeoJSON(tripsData)
  // const polyGeoJSON = convertToGeoJSON(polyData)
  const roadSource = new VectorSource({
    features: new GeoJSON().readFeatures(netGeoJSON)
  })
  const roadLayer = new VectorLayer({
    source: roadSource,
    style: new Style({
      stroke: new Stroke({
        color: '#000',
        width: 2
      })
    })
  })
  console.log(roadLayer);
  
  const vehicleSource = new VectorSource({
    features: new GeoJSON().readFeatures(tripsGeoJSON)
  })
  const vehicleLayer = new VectorLayer({
    source: vehicleSource,
    style: new Style({
      fill: new Fill({
        color: '#f00'
      }),
      stroke: new Stroke({
        color: '#f00',
        width: 1
      })
    })
  })

  // const polySource = new VectorSource({
  //   features: new GeoJSON().readFeatures(polyGeoJSON)
  // })
  // const polyLayer = new VectorLayer({
  //   source: polySource,
  //   style: new Style({
  //     fill: new Fill({
  //       color: '#0f0'
  //     }),
  //     stroke: new Stroke({
  //       color: '#0f0',
  //       width: 1
  //     })
  //   })
  // })

  map.value.addLayer(roadLayer)
  map.value.addLayer(vehicleLayer)
  // map.value.addLayer(polyLayer)
}
const loadSimulationData = async () => {
  const netResponse = await fetch('/veins_results/osm.net.xml')
  const netText = await netResponse.text()
  const tripsResponse = await fetch('/veins_results/osm.passenger.trips.xml')
  const tripsText = await tripsResponse.text()
  const polyResponse = await fetch('/veins_results/osm.poly.xml')
  const polyText = await polyResponse.text()

  const netData = await parseXml(netText)
  const tripsData = await parseXml(tripsText)
  const polyData = await parseXml(polyText)

  addLayersToMap(netData, tripsData, polyData)
}

onMounted(() => {
  map.value = new Map({
    target: 'map',
    layers: [
      new TileLayer({
        source: new OSM()
      })
    ],
    view: new View({
      center: center.value,
      projection: projection.value,
      zoom: zoom.value,
      rotation: rotation.value
    })
  })

  loadSimulationData()
})
</script>

<template>
  <div id="map" style="height:100vh"></div>
</template>

<style scoped>
@import 'vue3-openlayers/dist/vue3-openlayers.css';
</style>