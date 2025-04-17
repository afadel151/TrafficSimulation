<script setup lang="ts">
import { Map, RadioTower, Settings, UtilityPole } from "lucide-vue-next"
import { createClient } from '@supabase/supabase-js'
const supabase = createClient('https://rpwnfrlgbvebtitmlfqa.supabase.co', 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InJwd25mcmxnYnZlYnRpdG1sZnFhIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDI5MDU5NjIsImV4cCI6MjA1ODQ4MTk2Mn0.uuuxAfJ4wr8f31YU3F_kozvn92CMlk4lJG13a2L7-rY')

const emits = defineEmits(['openSUMO', 'loadNetworkdata', 'addRSU', 'addMBS','initOmnet']);
const simulations = ref<any[] | null>([]);
const { user } = useUser()
const fetchSimulations = async () => {
    const { data } = await supabase.from('simulations').select('*').eq('user_id', user.value?.id).order('created_at', { ascending: false });
    simulations.value = data;
}
const selectedSimulation = ref<string | null >(null);
const form = ref({
    name: 'Scenario',
    userId: '',
    userName: '',
    userEmail: '',
    duration: 4,
    vehicles: 4,
    speed: 20,
    cacheVehicle: 20,
    cacheRSU: 20,
    txPowerVehicle: 20,
    txPowerRSU: 20,
    txPowerMBS: 20
})

const openSUMO = () => {
    emits('openSUMO', form.value);
}

const selectingRSU = ref(false)
const selectingMBS = ref(false)
const addRSU = () => {
    if (!selectingRSU.value) {
        selectingRSU.value = true
        selectingMBS.value = false
        emits('addRSU', selectingRSU.value)
    } else {
        selectingRSU.value = false
    }
}
const addMBS = () => {
    if (!selectingMBS.value) {
        selectingMBS.value = true
        selectingRSU.value = false
        emits('addMBS', selectingMBS.value)
    } else {
        selectingMBS.value = false
    }
}

watch(simulations, (newSimulations) => {
    simulationsList.value = newSimulations;
});
watch(user, (newUser) => {
    form.value.userId = newUser?.id ?? '';  
    form.value.userName = newUser?.fullName ?? '';
    form.value.userEmail = newUser?.primaryEmailAddress?.emailAddress ?? '';
    fetchSimulations()
});
const simulationsList = ref<any[] | null>([]);

const loadPath = (simu : any) => {
    emits('loadNetworkdata', simu);
    selectedSimulation.value = simu.path;
}
const initOmnetpp = () => {
    emits('initOmnet');
}

</script>

<template>
    <Sidebar>
        <SidebarHeader class="flex flex-row justify-center items-center my-5 ">
            <p> {{ user?.fullName }} </p>
            <UserButton />
        </SidebarHeader>
        <SidebarContent>
            <SidebarGroup>
                <SidebarGroupLabel>Tools</SidebarGroupLabel>
                <SidebarGroupContent class="mt-3">
                    <SidebarMenu>
                        <SidebarMenuItem class="flex px-9 justify-center">
                            <Dialog>
                                <DialogTrigger as-child>
                                    <Button variant="green" class="w-full"><Map class="w-4 h-4" />Generate
                                        scenario</Button>
                                </DialogTrigger>
                                <DialogContent>
                                    <DialogHeader>
                                        <DialogTitle>Generate scenario</DialogTitle>
                                    </DialogHeader>
                                    <div class="grid gap-4 py-4 ">
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="name">
                                                Name
                                            </Label>
                                            <Input id="name" v-model="form.name" class="col-span-2" />
                                        </div>
                                        <div class="grid grid-cols-4 items-center gap-4 ">
                                            <Label for="username">
                                                Duration
                                            </Label>
                                            <Input id="numv" v-model="form.duration" class="col-span-2" />
                                            <Label>
                                                seconds
                                            </Label>
                                        </div>
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="username">
                                                Vehicles
                                            </Label>
                                            <Input id="numv" v-model="form.vehicles" class="col-span-2" />
                                            <Label>
                                                vehicle(s)
                                            </Label>
                                        </div>
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="username">
                                                Speed
                                            </Label>
                                            <Input id="numv" v-model="form.speed" class="col-span-2" />
                                            <Label>
                                                km/h
                                            </Label>
                                        </div>
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="username">
                                                Cache vehicle
                                            </Label>
                                            <Input id="numv" v-model="form.cacheVehicle" class="col-span-2" />
                                            <Label>
                                                Mb
                                            </Label>
                                        </div>
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="username">
                                                Cache RSU
                                            </Label>
                                            <Input id="numv" v-model="form.cacheRSU" class="col-span-2" />
                                            <Label>
                                                Mb
                                            </Label>
                                        </div>
                                    </div>
                                    <p class="text-center">Tx power</p>
                                    <div class="grid gap-4 py-4 ">
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="username">
                                                Vehicle
                                            </Label>
                                            <Input id="numv" v-model="form.txPowerVehicle" class="col-span-2" />
                                            <Label>
                                                mWatt
                                            </Label>
                                        </div>
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="username">
                                                RSU
                                            </Label>
                                            <Input id="numv" v-model="form.txPowerRSU" class="col-span-2" />
                                            <Label>
                                                mWatt
                                            </Label>
                                        </div>
                                        <div class="grid grid-cols-4 items-center gap-4">
                                            <Label for="username">
                                                MBS
                                            </Label>
                                            <Input id="numv" v-model="form.txPowerMBS" class="col-span-2" />
                                            <Label>
                                                mWatt
                                            </Label>
                                        </div>
                                    </div>
                                    <DialogFooter>
                                        <Button @click="openSUMO" type="submit" variant="green" class="w-full">
                                            Save changes
                                        </Button>
                                    </DialogFooter>
                                </DialogContent>
                            </Dialog>

                        </SidebarMenuItem>
                        <SidebarMenuItem class="flex px-9 mt-4 justify-center">
                            <Button @click="addRSU" :variant="selectingRSU ? 'secondary' : 'outline'" class="w-full">
                                <RadioTower class="w-4 h-4 mr-2" />Add MBS
                            </Button>
                        </SidebarMenuItem>
                        <SidebarMenuItem class="flex px-9 mt-4 justify-center">
                            <Button @click="addMBS" :variant="selectingMBS ? 'secondary' : 'outline'" class="w-full">
                                <UtilityPole class="w-4 h-4 mr-2" />Add RSU
                            </Button>
                        </SidebarMenuItem>
                        <SidebarMenuItem class="flex px-9 mt-4 justify-center">
                            <Button variant="default" @click="initOmnetpp" :disabled="selectedSimulation == null" class="w-full">
                               <Settings /> Initialize simulation
                            </Button>
                        </SidebarMenuItem>
                    </SidebarMenu>
                </SidebarGroupContent>
                <SidebarGroupLabel class="mt-5">Scenarios</SidebarGroupLabel>
                <SidebarGroupContent class="mt-3">
                    <SidebarMenu>
                        <SidebarMenuItem v-for="simu in simulationsList" :key="simu.id"
                            class="flex px-5 justify-center">
                            <Button variant="outline" class="w-full" type="button" @click="loadPath(simu)"><Map
                                    class="w-4 h-4 mr-2" />{{ simu.path }}</Button>
                        </SidebarMenuItem>
                    </SidebarMenu>
                </SidebarGroupContent>
            </SidebarGroup>
        </SidebarContent>
    </Sidebar>
</template>