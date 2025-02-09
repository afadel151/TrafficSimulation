<script setup>
import { ref } from 'vue'
const center = ref([40, 40])
const projection = ref('EPSG:4326')
const zoom = ref(8)
const rotation = ref(0)
</script>

<template>
  <ol-map :loadTilesWhileAnimating="true" :loadTilesWhileInteracting="true" style="height:100vh">
    <ol-view :center="center" :rotation="rotation" :zoom="zoom"
    :projection="projection" />
    <ol-tile-layer>
        <ol-source-osm />
    </ol-tile-layer>
  </ol-map>
</template>

<style scoped>
@import 'vue3-openlayers/dist/vue3-openlayers.css';
</style>