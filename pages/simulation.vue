<script setupt>
import Tools from '~/components/Tools.vue';


</script>

<template>
    <ClientOnly>
        <div class="h-screen w-screen relative">
            <!-- <Tools class="absolute  z-10" /> -->
            <Map2 />
        </div>
            <template #fallback> Loading map... </template>
  </ClientOnly>
</template>

