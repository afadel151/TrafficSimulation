<script setup>
import Menubar from 'primevue/menubar';
import { ref } from "vue";
const emits = defineEmits(['openSUMO', 'features', 'loadNetworkData', 'loadPolyData', 'loadPassengerTrips', 'contact']);

const handleOpenSUMO = () => {
    emits('openSUMO');
};

const handleFeatures = () => {
    emits('features');
};

const handleLoadNetworkData = () => {
    emits('loadNetworkData');
};

const handleLoadPolyData = () => {
    emits('loadPolyData');
};

const handleLoadPassengerTrips = () => {
    emits('loadPassengerTrips');
};

const handleContact = () => {
    emits('contact');
};
const items = ref([
    {
        label: 'Open SUMO',
        icon: 'pi pi-map',
        command: handleOpenSUMO

    },
    {
        label: 'Features',
        icon: 'pi pi-star'
    },
    {
        label: 'Load',
        icon: 'pi pi-refresh',
        items: [
            {
                label: 'Network data',
                icon: 'pi pi-sitemap',
                command: handleLoadNetworkData
            },
            {
                label: 'Poly data',
                icon: 'pi pi-server',
                command: handleLoadPolyData
            },
            {
                label: 'Passenger trips',
                icon: 'pi pi-car',
                command: handleLoadPassengerTrips
            }
        ]
    },
    {
        label: 'Contact',
        icon: 'pi pi-envelope' ,
        command: handleContact 
    }
]);

</script>


<template>
    <div class="card w-screen flex justify-center items-center mt-5">
        <Menubar :model="items" />
    </div>

</template>