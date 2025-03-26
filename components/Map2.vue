<script setup>
import { ref, onMounted } from 'vue'
import { Map, View } from 'ol'
import TileLayer from 'ol/layer/Tile'
import OSM from 'ol/source/OSM'
import VectorLayer from 'ol/layer/Vector'
import VectorSource from 'ol/source/Vector'
import { GeoJSON } from 'ol/format'
import { Style, Fill, Stroke } from 'ol/style'
import Tools from '~/components/Tools.vue';
import axios from 'axios'
const center = ref([4, 35])
const projection = ref('EPSG:4326')
const zoom = ref(8)
const rotation = ref(0)
const map = ref(null)


const loadGeoJSONData = async (path) => {
  console.log(path);
  
  const response = await fetch(`${path}.osm.net.geojson`)
  console.log(response);
  
  const geojsonData = await response.json()

  const vectorSource = new VectorSource({
    features: new GeoJSON().readFeatures(geojsonData, {
      featureProjection: projection.value
    })
  })

  const vectorLayer = new VectorLayer({
    source: vectorSource,
    style: new Style({
      stroke: new Stroke({
        color: '#000',
        width: 2
      }),
      fill: new Fill({
        color: 'rgba(0, 0, 0, 0.1)'
      })
    })
  })
  console.log(vectorLayer);
  
  map.value.addLayer(vectorLayer)
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
})

const openSumo = async () => {
  try {
    await axios.post('http://localhost:3333/run-osm-web-wizard');
    emits('openSUMO');
  } catch (error) {
    console.error('Failed to run webwizard:', error);
  }
}


</script>

<template>
  <div id="map" style="height:100vh" class="relative">
    <Tools class="absolute  z-10" @openSUMO="openSumo" @loadNetworkdata="loadGeoJSONData" />
  </div>
</template>

<style scoped>
@import 'vue3-openlayers/dist/vue3-openlayers.css';
</style>