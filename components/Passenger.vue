<template>
  <div class="passenger" :style="style" ref="passengerEl"></div>
</template>

<script setup>
import { onMounted, ref } from "vue";
import gsap from "gsap";

const props = defineProps({
  speed: Number,
  style: Object,
});
const passengerEl = ref(null);

onMounted(() => {
  gsap.to(passengerEl.value, {
    x: "+=50", // Move 50px to the right
    yoyo: true, // Reverse animation
    repeat: -1, // Loop indefinitely
    duration: props.speed,
    ease: "power1.inOut", // Smooth easing
  });
});
</script>

<style scoped>
.passenger {
  width: 20px;
  height: 20px;
  background-color: #e74c3c; /* Red for passengers */
  border-radius: 50%;
  position: absolute;
}
</style>
