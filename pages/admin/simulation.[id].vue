<script setup>
const id = useRoute().params.id
import { createClient } from '@supabase/supabase-js'
import { Feature, Map, View } from 'ol'
import TileLayer from 'ol/layer/Tile'
import { OSM, Vector } from 'ol/source'
import 'ol/ol.css'
import VectorLayer from 'ol/layer/Vector'
import VectorSource from 'ol/source/Vector'
import { GeoJSON } from 'ol/format'
import { Style, Fill, Stroke, Icon, IconImage } from 'ol/style'
import axios from 'axios'
import { Button } from '@/components/ui/button'

import {
  Dialog,
  DialogContent,
  DialogHeader,
  DialogTitle,
} from '@/components/ui/dialog'
import { ChevronLeft } from 'lucide-vue-next'
import { Point } from 'ol/geom'

const center = ref([4, 35])
const projection = ref('EPSG:4326')
const zoom = ref(8)
const rotation = ref(0)
const supabase = createClient('https://rpwnfrlgbvebtitmlfqa.supabase.co', 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InJwd25mcmxnYnZlYnRpdG1sZnFhIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDI5MDU5NjIsImV4cCI6MjA1ODQ4MTk2Mn0.uuuxAfJ4wr8f31YU3F_kozvn92CMlk4lJG13a2L7-rY')
const map = ref(null)
let simulationData = {}
const showDialog = ref(false)

const fetchSimulationSetup = async () => {
    const { data, error } = await supabase
        .from('simulations')
        .select('*')
        .eq('id', id)
    if (error) {
        console.error('Error fetching simulation setup:', error)
    } else {
        simulationData = data[0]
        console.log(simulationData.rsu_position.geometry.coordinates);
        
        loadGeoJSONData()
    }
}

const loadGeoJSONData = async () => {
  try {
    // await axios.post('http://localhost:3333/convert-network', {
    //     path: simulationData?.path
    // });
    const response = await fetch(`/${simulationData?.path}/osm.net.geojson`)
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
  
    map.value.addLayer(vectorLayer)
    let marker = new Feature({ geometry: new Point(simulationData.rsu_position.geometry.coordinates) });
    marker.setStyle(new Style({
      image: new Icon(({
        src: new URL(`../../assets/svgs/RSU.svg`, import.meta.url).href,
        width: 100,
        height: 100
      }))
    }));
    vectorSource = new Vector({ features: [marker] })
    vectorLayer = new VectorLayer({
      source: vectorSource,
    });
    map.value.addLayer(vectorLayer);
    simulationData.mbs_positions.features.forEach((mbs) => {
      let marker = new Feature({ geometry: new Point(mbs.geometry.coordinates) });
      marker.setStyle(new Style({
        image: new Icon(({
          src: new URL(`../../assets/svgs/MBS.svg`, import.meta.url).href,
          width: 100,
          height: 100
        }))
      }));
      vectorSource = new Vector({ features: [marker] })
      vectorLayer = new VectorLayer({
        source: vectorSource,
      });
      map.value.addLayer(vectorLayer);
    })
  } catch (error) {
    console.error("Failed to convert network:", error);
    alert("Failed to convert network. See console for details.");
  }
}

const formatDate = (dateString) => {
  return new Date(dateString).toLocaleString()
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
    fetchSimulationSetup()
})
</script>

<template>
    <div id="map" class="h-screen relative w-full">
        <div class="absolute top-4 space-x-2 right-4 z-10">
            <NuxtLink href="/admin">
                <Button>
                Return<ChevronLeft  />
            </Button>
            </NuxtLink>
            <Button 
                @click="showDialog = true"
                class="bg-white hover:bg-gray-100 text-gray-800 font-semibold py-2 px-4 border border-gray-300  shadow"
            >
                Simulation Details
            </Button>
        </div>
    </div>

    <Dialog v-model:open="showDialog">
        <DialogContent class="sm:max-w-[625px] max-h-[80vh] overflow-y-auto">
            <DialogHeader>
                <DialogTitle>Simulation Configuration</DialogTitle>
            </DialogHeader>
            
            <div v-if="simulationData" class="grid gap-4 py-4">
                <div class="space-y-4">
                    <h3 class="font-semibold text-lg border-b pb-2">Basic Information</h3>
                    <div class="grid grid-cols-2 gap-4">
                        <div>
                            <p class="text-sm text-gray-500">Simulation Name</p>
                            <p class="font-medium">{{ simulationData.name }}</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">Created By</p>
                            <p class="font-medium">{{ simulationData.user_name }}</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">User Email</p>
                            <p class="font-medium">{{ simulationData.user_email }}</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">Created At</p>
                            <p class="font-medium">{{ formatDate(simulationData.created_at) }}</p>
                        </div>
                    </div>
                </div>

                <!-- Simulation Parameters Section -->
                <div class="space-y-4">
                    <h3 class="font-semibold text-lg border-b pb-2">Simulation Parameters</h3>
                    <div class="grid grid-cols-2 gap-4">
                        <div>
                            <p class="text-sm text-gray-500">Duration</p>
                            <p class="font-medium">{{ simulationData.duration }} seconds</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">Number of Vehicles</p>
                            <p class="font-medium">{{ simulationData.vehicles }}</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">Vehicle Speed</p>
                            <p class="font-medium">{{ simulationData.speed }} km/h</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">Network Path</p>
                            <p class="font-medium">{{ simulationData.path }}</p>
                        </div>
                    </div>
                </div>

                <!-- Network Configuration Section -->
                <div class="space-y-4">
                    <h3 class="font-semibold text-lg border-b pb-2">Network Configuration</h3>
                    <div class="grid grid-cols-2 gap-4">
                        <div>
                            <p class="text-sm text-gray-500">Vehicle Cache</p>
                            <p class="font-medium">{{ simulationData.cache_v }} MB</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">RSU Cache</p>
                            <p class="font-medium">{{ simulationData.cache_rsu }} MB</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">Vehicle TX Power</p>
                            <p class="font-medium">{{ simulationData.tx_v }} mW</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">RSU TX Power</p>
                            <p class="font-medium">{{ simulationData.tx_rsu }} mW</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">MBS TX Power</p>
                            <p class="font-medium">{{ simulationData.tx_mbs }} mW</p>
                        </div>
                    </div>
                </div>

                <!-- Infrastructure Positions -->
                <div class="space-y-4">
                    <h3 class="font-semibold text-lg border-b pb-2">Infrastructure Positions</h3>
                    <div class="grid grid-cols-2 gap-4">
                        <div>
                            <p class="text-sm text-gray-500">RSU Positions</p>
                            <p class="font-medium">{{ simulationData.rsu_position.geometry.coordinates || 'Not specified' }}</p>
                        </div>
                        <div>
                            <p class="text-sm text-gray-500">MBS Positions</p>
                            <p class="font-medium">{{ simulationData.mbs_positions.features.map(feature => feature.geometry.coordinates) || 'Not specified' }}</p>
                        </div>
                    </div>
                </div>
            </div>

            <div v-else class="flex justify-center items-center h-32">
                <p>Loading simulation data...</p>
            </div>
        </DialogContent>
    </Dialog>
</template>

<style>
/* Custom scrollbar for dialog */
.DialogContent {
  scrollbar-width: thin;
  scrollbar-color: #cbd5e1 #f1f5f9;
}

.DialogContent::-webkit-scrollbar {
  width: 8px;
}

.DialogContent::-webkit-scrollbar-track {
  background: #f1f5f9;
}

.DialogContent::-webkit-scrollbar-thumb {
  background-color: #cbd5e1;
  border-radius: 4px;
}
</style>