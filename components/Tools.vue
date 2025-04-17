<script setup>
import OpenLayers from 'openlayers';
import { createClient } from '@supabase/supabase-js'
const supabase = createClient('https://rpwnfrlgbvebtitmlfqa.supabase.co', 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InJwd25mcmxnYnZlYnRpdG1sZnFhIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDI5MDU5NjIsImV4cCI6MjA1ODQ4MTk2Mn0.uuuxAfJ4wr8f31YU3F_kozvn92CMlk4lJG13a2L7-rY')
import { ref } from "vue";
const emits = defineEmits(['openSUMO', 'features', 'loadNetworkdata', 'loadPolyData', 'loadPassengerTrips', 'contact']);
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
const simulations = ref([]);
const fetchSimulations = async () => 
{
    const { data } = await supabase.from('simulations').select()
    simulations.value = data;
}
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
        label: 'Simulations',
        icon: 'pi pi-star',
        items: []
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
        icon: 'pi pi-envelope',
        command: startBuild
    }
]);
const convertNetwork = async (path) => {
    const response = await fetch('http://localhost:3333/convert-network', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            path: path
        })
    });
    const data = await response.json();
    console.log(data.message);
    emits('loadNetworkdata', path);
}
watch(simulations, (newSimulations) => {
    const simulationItems = newSimulations.map(simulation => ({
        label: simulation.path,
        icon: 'pi pi-star',
        command: () => {
            convertNetwork(simulation.path);
        }
    }));
    items.value.find(item => item.label === 'Simulations').items = simulationItems;
});
function startBuild() {
  var map = new OpenLayers.Map("map");
  var cor = map.getExtent();
  cor.transform(
    map.getProjectionObject(),
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
onMounted(()=>{
    fetchSimulations();
})
</script>


<template>
    
    <div class="card w-screen flex justify-center items-center mt-5">
        <Menubar>
    <MenubarMenu>
      <MenubarTrigger>File</MenubarTrigger>
      <MenubarContent>
        <MenubarItem>
          New Tab <MenubarShortcut>⌘T</MenubarShortcut>
        </MenubarItem>
        <MenubarItem>
          New Window <MenubarShortcut>⌘N</MenubarShortcut>
        </MenubarItem>
        <MenubarItem disabled>
          New Incognito Window
        </MenubarItem>
        <MenubarSeparator />
        <MenubarSub>
          <MenubarSubTrigger>Share</MenubarSubTrigger>
          <MenubarSubContent>
            <MenubarItem>Email link</MenubarItem>
            <MenubarItem>Messages</MenubarItem>
            <MenubarItem>Notes</MenubarItem>
          </MenubarSubContent>
        </MenubarSub>
        <MenubarSeparator />
        <MenubarItem>
          Print... <MenubarShortcut>⌘P</MenubarShortcut>
        </MenubarItem>
      </MenubarContent>
    </MenubarMenu>
    <MenubarMenu>
      <MenubarTrigger>Edit</MenubarTrigger>
      <MenubarContent>
        <MenubarItem>
          Undo <MenubarShortcut>⌘Z</MenubarShortcut>
        </MenubarItem>
        <MenubarItem>
          Redo <MenubarShortcut>⇧⌘Z</MenubarShortcut>
        </MenubarItem>
        <MenubarSeparator />
        <MenubarSub>
          <MenubarSubTrigger>Find</MenubarSubTrigger>
          <MenubarSubContent>
            <MenubarItem>Search the web</MenubarItem>
            <MenubarSeparator />
            <MenubarItem>Find...</MenubarItem>
            <MenubarItem>Find Next</MenubarItem>
            <MenubarItem>Find Previous</MenubarItem>
          </MenubarSubContent>
        </MenubarSub>
        <MenubarSeparator />
        <MenubarItem>Cut</MenubarItem>
        <MenubarItem>Copy</MenubarItem>
        <MenubarItem>Paste</MenubarItem>
      </MenubarContent>
    </MenubarMenu>
    <MenubarMenu>
      <MenubarTrigger>View</MenubarTrigger>
      <MenubarContent>
        <MenubarCheckboxItem>Always Show Bookmarks Bar</MenubarCheckboxItem>
        <MenubarCheckboxItem :model-value="true">
          Always Show Full URLs
        </MenubarCheckboxItem>
        <MenubarSeparator />
        <MenubarItem inset>
          Reload <MenubarShortcut>⌘R</MenubarShortcut>
        </MenubarItem>
        <MenubarItem disabled inset>
          Force Reload <MenubarShortcut>⇧⌘R</MenubarShortcut>
        </MenubarItem>
        <MenubarSeparator />
        <MenubarItem inset>
          Toggle Fullscreen
        </MenubarItem>
        <MenubarSeparator />
        <MenubarItem inset>
          Hide Sidebar
        </MenubarItem>
      </MenubarContent>
    </MenubarMenu>
    <MenubarMenu>
      <MenubarTrigger>Profiles</MenubarTrigger>
      <MenubarContent>
        <MenubarRadioGroup model-value="benoit">
          <MenubarRadioItem value="andy">
            Andy
          </MenubarRadioItem>
          <MenubarRadioItem value="benoit">
            Benoit
          </MenubarRadioItem>
          <MenubarRadioItem value="Luis">
            Luis
          </MenubarRadioItem>
        </MenubarRadioGroup>
        <MenubarSeparator />
        <MenubarItem inset>
          Edit...
        </MenubarItem>
        <MenubarSeparator />
        <MenubarItem inset>
          Add Profile...
        </MenubarItem>
      </MenubarContent>
    </MenubarMenu>
  </Menubar>
    </div>

</template>