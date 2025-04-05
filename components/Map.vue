<script setup>
import { ref, onMounted } from 'vue'
import { Feature, Map, View } from 'ol'
import TileLayer from 'ol/layer/Tile'
import OSM from 'ol/source/OSM'
import VectorLayer from 'ol/layer/Vector'
import VectorSource from 'ol/source/Vector'
import { GeoJSON } from 'ol/format'
import { Style, Fill, Stroke, Icon } from 'ol/style'
import Tools from '~/components/Tools.vue';
import axios from 'axios'
import { Point } from 'ol/geom'
import Draw from 'ol/interaction/Draw'
import { Vector } from 'ol/source'
const center = ref([4, 35])
const projection = ref('EPSG:4326')

const zoom = ref(8)
const rotation = ref(0)
const map = ref(null)
let draw;
let save;
let dibujo;
let coordinates = [];
let old_coordinates = [];


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
  });
  map.value.on("click", function (evt) {
    console.log(evt.coordinate);
    let marker = new Feature({
      geometry: new Point(evt.coordinate),
    });
    marker.setStyle(new Style({
      image: new Icon(({
        src: new URL('../assets/imgs/rsu.png', import.meta.url).href,
        width:100,
        height:100
      }))
    }));
    let vectorSource = new Vector({ features: [marker] })
    var markerVectorLayer = new VectorLayer({
      source: vectorSource,
    });
    map.value.addLayer(markerVectorLayer);
  });
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