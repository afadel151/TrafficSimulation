<template>
  <div class="vehicle" :style="style" ref="vehicleEl"></div>
</template>

<script setup>
import { onMounted, ref } from "vue";
import gsap from "gsap";

const props = defineProps({
  speed: Number,
  style: Object,
});
const vehicleEl = ref(null);

onMounted(() => {
  gsap.fromTo(
    vehicleEl.value,
    { x: -100 }, // Start off-screen left
    {
      x: "100vw", // Move to off-screen right
      duration: props.speed,
      repeat: -1, // Loop indefinitely
      ease: "linear",
    }
  );
});
</script>

<style scoped>
.vehicle {
  width: 60px;
  height: 30px;
  background-color: #3498db; /* Blue for vehicles */
  position: absolute;
  border-radius: 5px;
}
</style>
