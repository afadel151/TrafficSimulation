<script setup>
import Menubar from 'primevue/menubar';
import OpenLayers from 'openlayers';
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
        label: 'Build',
        icon: 'pi pi-envelope' ,
        command: startBuild 
    }
]);

function startBuild() {
  var map = new OpenLayers.Map("map");
  var cor = map.getExtent();
  cor.transform(
    map.getProjectionObject(), // from Spherical Mercator Projection
    new OpenLayers.Projection("EPSG:4326")
  );

  var data = {
    poly: elem("#polygons").checked,
    duration: parseInt(elem("#duration").value),
    publicTransport: elem("#publicTransport").checked,
    leftHand: elem("#leftHand").checked,
    decal: elem("#decal").checked,
    carOnlyNetwork: elem("#carOnlyNetwork").checked,
    vehicles: {}
  };

  // calculates the coordinates of the rectangle if area-picking is active
  if (canvasActive) {
    var width = cor.right - cor.left;
    var height = cor.bottom - cor.top;
    data.coords = [
      cor.left + width * canvasRect[0],
      cor.top + height * canvasRect[3],
      cor.left + width * canvasRect[2],
      cor.top + height * canvasRect[1]
    ];
  } else
    data.coords = [cor.left, cor.bottom, cor.right, cor.top];

  vehicleClasses.forEach(function (vehicleClass) {
    var result = vehicleClass.toJSON();
    if (result)
      data.vehicles[vehicleClass.internal] = result;
  });
  console.log(data);
  

}
</script>


<template>
    <div class="card w-screen flex justify-center items-center mt-5">
        <Menubar :model="items" />
    </div>

</template>