<script setup>
import { ref, onMounted } from 'vue'
import { Feature, Map, View } from 'ol'
import TileLayer from 'ol/layer/Tile'
import OSM from 'ol/source/OSM'
import 'ol/ol.css'
import VectorLayer from 'ol/layer/Vector'
import VectorSource from 'ol/source/Vector'
import { GeoJSON } from 'ol/format'
import { Style, Fill, Stroke, Icon } from 'ol/style'
import axios from 'axios'
import { Point } from 'ol/geom'
import { Vector } from 'ol/source'
import { Modify } from 'ol/interaction'
import AppSidebar from './AppSidebar.vue'
const center = ref([4, 35])
const projection = ref('EPSG:4326')
const rsuPosition = ref(null)
const mbsPosition = ref(null)
const mbsPositions = ref([])
const selectedSimulation = ref({})
const selectedUnit = ref('RSU' | 'MBS' | 'NONE')
const zoom = ref(8)
const rotation = ref(0)
const map = ref(null)
const geojsonLayer = ref(null)


const loadGeoJSONData = async (simulation, event) => {
  if (event) event.preventDefault();

  if (simulation.path == null) {
    alert("Please select a setup")
    return
  }
  selectedSimulation.value = simulation
  try {
    await axios.post('http://localhost:3333/convert-network', {
      path: simulation.path
    });
    const response = await fetch(`${simulation.path}/osm.net.geojson`)
    if (geojsonLayer.value) {
      map.value.removeLayer(geojsonLayer.value)
      geojsonLayer.value = null
    }
    const geojsonData = await response.json()

    let vectorSource = new VectorSource({
      features: new GeoJSON().readFeatures(geojsonData, {
        featureProjection: projection.value
      })
    })

    let vectorLayer = new VectorLayer({
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
    geojsonLayer.value = vectorLayer
    map.value.addLayer(vectorLayer)
    if (simulation.rsu_position == null || simulation.mbs_positions == null) {
      return
    }

    let marker = new Feature({ geometry: new Point(simulation.rsu_position.geometry.coordinates) });
    marker.setStyle(new Style({
      image: new Icon(({
        src: new URL(`../assets/svgs/RSU.svg`, import.meta.url).href,
        width: 100,
        height: 100
      }))
    }));
    vectorSource = new Vector({ features: [marker] })
    vectorLayer = new VectorLayer({
      source: vectorSource,
    });
    map.value.addLayer(vectorLayer);
    const modify = new Modify({
      hitDetection: vectorLayer,
      source: vectorSource,
      name: 'RSU',
    });
    const target = document.getElementById('map');
    modify.on(['modifystart', 'modifyend'], function (evt) {
      console.log(evt.features);
      target.style.cursor = evt.type === 'modifystart' ? 'grabbing' : 'pointer';
      if (evt.type === 'modifyend') {
        rsuPosition.value = evt.features.item(0).getGeometry().getCoordinates()
      }
    });
    map.value.addInteraction(modify);
    mbsPositions.value = simulation.mbs_positions.features.map((mbs) => {
      return mbs.geometry.coordinates
    })
    mbsPositions.value.forEach((mbs) => {
      let marker = new Feature({ geometry: new Point(mbs) });
      marker.setStyle(new Style({
        image: new Icon(({
          src: new URL(`../assets/svgs/MBS.svg`, import.meta.url).href,
          width: 100,
          height: 100
        }))
      }));
      vectorSource = new Vector({ features: [marker] })
      vectorLayer = new VectorLayer({
        source: vectorSource,
      });
      map.value.addLayer(vectorLayer);
      const modify = new Modify({
        hitDetection: vectorLayer,
        source: vectorSource,
        name: 'RSU',
      });
      map.value.addInteraction(modify);
    })
  } catch (error) {
    console.error("Failed to convert network:", error);
    alert("Failed to convert network. See console for details.");
  }
}

onMounted(() => {
  selectedUnit.value = 'NONE'
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
    if (selectedUnit.value != 'NONE') {

      if (selectedUnit.value == 'RSU' && rsuPosition.value == null) {
        rsuPosition.value = evt.coordinate
      } else if (selectedUnit.value == 'MBS') {
        mbsPositions.value.push(evt.coordinate)
      } else {
        alert("You can only add one RSU and one MBS")
        return
      }
      let marker = new Feature({
        geometry: new Point(evt.coordinate),
      });
      marker.setStyle(new Style({
        image: new Icon(({
          src: new URL(`../assets/svgs/${selectedUnit.value}.svg`, import.meta.url).href,
          width: 100,
          height: 100
        }))
      }));
      let vectorSource = new Vector({ features: [marker] })
      var markerVectorLayer = new VectorLayer({
        source: vectorSource,
      });
      const target = document.getElementById('map');
      const modify = new Modify({
        hitDetection: markerVectorLayer,
        source: vectorSource,
        name: selectedUnit.value,
      });
      modify.name = selectedUnit.value;
      map.value.addLayer(markerVectorLayer);
      modify.on(['modifystart', 'modifyend'], function (evt) {
        target.style.cursor = evt.type === 'modifystart' ? 'grabbing' : 'pointer';
        if (evt.type === 'modifyend') {
          if (modify.name == 'RSU') {
            rsuPosition.value = evt.features.item(0).getGeometry().getCoordinates()
          } else if (modify.name == 'MBS') {
            mbsPosition.value = evt.features.item(0).getGeometry().getCoordinates()
          }
        }
      });
      const overlaySource = modify.getOverlay().getSource();
      overlaySource.on(['addfeature', 'removefeature'], function (evt) {
        target.style.cursor = evt.type === 'addfeature' ? 'pointer' : '';
      });
      map.value.addInteraction(modify);
    }
  });
})


const openSumo = async (form) => {
  if (form == null) {
    alert("Please fill the form")
    return
  }
  try {
    await axios.post('http://localhost:3333/run-osm-web-wizard', {
      form: form
    });
  } catch (error) {
    console.error('Failed to run webwizard:', error);
  }
}
const addRSU = () => {
  selectedUnit.value = 'RSU'
}
const addMBS = () => {
  selectedUnit.value = 'MBS'
}

const initOmnetpp = async (event) => {
  if (event) event.preventDefault();
  try {
    if (rsuPosition.value == null || mbsPositions.value.length == 0 || selectedSimulation.value == null) {
      alert("Please add RSU and MBS to the map")
      return
    }
    const response = await axios.post('http://localhost:3333/generate-ini', {
      rsu_position: rsuPosition.value,
      mbs_positions: mbsPositions.value,
      simulation: selectedSimulation.value
    });
    if (response.code === 200) {
      console.log('OMNeT++ initialized successfully');
    } else {
      console.error('Failed to initialize OMNeT++:', response.status);
    }
  } catch (error) {
    console.error('Failed to initialize OMNeT++:', error);

  }
}
</script>

<template>
  <SidebarProvider class="h-screen w-screen ">
    <AppSidebar @addRSU="addRSU()" @initOmnet="initOmnetpp" @addMBS="addMBS()" @loadNetworkdata="loadGeoJSONData"
      @openSUMO="openSumo" class="absolute z-10" />
    <div id="map" class="h-screen relative  w-full ">
      <SidebarTrigger class="absolute top-2 left-10 z-10" />
    </div>
  </SidebarProvider>
</template>
