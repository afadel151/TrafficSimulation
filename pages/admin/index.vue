<script setup lang="ts">
import { Map, Users, Activity, ChevronDown, Car, CarIcon, Clock, Eye } from "lucide-vue-next"
import { createClient } from '@supabase/supabase-js'

const supabase = createClient(
    'https://rpwnfrlgbvebtitmlfqa.supabase.co',
    'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InJwd25mcmxnYnZlYnRpdG1sZnFhIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDI5MDU5NjIsImV4cCI6MjA1ODQ4MTk2Mn0.uuuxAfJ4wr8f31YU3F_kozvn92CMlk4lJG13a2L7-rY'
)

const allSimulations = ref<any[]>([])
const expandedSimulation = ref<string | null>(null)
const selectedTab = ref<'overview' | 'simulations'>('overview')
const stats = ref({
    totalSimulations: 0,
    activeUsers: 0,
    averageDuration: 0,
    mostCommonVehicleCount: 0
})

const fetchAllSimulations = async () => {
    try {
        const { data, error } = await supabase
            .from('simulations')
            .select('*')
            .order('created_at', { ascending: false })

        if (error) {
            console.error('Error fetching simulations:', error)
            return
        }

        if (!Array.isArray(data)) {
            console.error('Expected data to be an array, but got:', data)
            return
        }

        allSimulations.value = data
        stats.value.totalSimulations = data.length

        stats.value.averageDuration =
            data.reduce((acc, sim) => acc + (sim.duration || 0), 0) / data.length

        const vehicleCounts = data.reduce<Record<number, number>>((acc, sim) => {
            const count = sim.vehicles || 0
            acc[count] = (acc[count] || 0) + 1
            return acc
        }, {})

        const sortedVehicleCounts = Object.entries(vehicleCounts).sort(
            (a, b) => (b[1] as number) - (a[1] as number)
        )

        stats.value.mostCommonVehicleCount = parseInt(sortedVehicleCounts[0]?.[0]) || 0
    } catch (error) {
        console.log('Unexpected error:', error)
    }
}




const toggleSimulation = (id: string) => {
    expandedSimulation.value = expandedSimulation.value === id ? null : id
}

const formatDate = (dateString: string) => {
    return new Date(dateString).toLocaleString()
}

onMounted(() => {
    fetchAllSimulations()
})

</script>

<template>
    <div class="min-h-screen bg-gray-50 text-gray-800">
        <header class="bg-white px-20 shadow-sm border-b border-gray-200">
            <div class="container mx-auto px-6 py-4">
                <div class="flex items-center justify-between">
                    <div class="flex items-center space-x-2">
                        <CarIcon class="h-8 w-8 text-blue-500" />
                        <span
                            class="text-xl font-bold bg-clip-text text-transparent bg-gradient-to-r from-blue-500 to-cyan-400">
                            omSET <span class="text-black">Admin</span>
                        </span>
                    </div>
                    <div class="flex items-center space-x-4">
                        <Button variant="ghost" class="text-gray-600 hover:text-blue-500"
                            @click="selectedTab = 'overview'"
                            :class="{ 'text-blue-500 font-medium': selectedTab === 'overview' }">
                            Overview
                        </Button>
                        <Button variant="ghost" class="text-gray-600 hover:text-blue-500"
                            @click="selectedTab = 'simulations'"
                            :class="{ 'text-blue-500 font-medium': selectedTab === 'simulations' }">
                            All Simulations
                        </Button>
                        <UserButton />
                    </div>
                </div>
            </div>
        </header>

        <main class="container mx-auto px-20 mt-5 py-8">
            <div v-if="selectedTab === 'overview'" class="space-y-8">
                <h2 class="text-2xl font-bold text-gray-800">Dashboard Overview</h2>
                <div class="grid grid-cols-1 md:grid-cols-4 gap-6">
                    <div class="bg-white p-6 rounded-xl border border-gray-200 shadow-sm">
                        <div class="flex items-center justify-between">
                            <div>
                                <p class="text-sm text-gray-500">Total Simulations</p>
                                <h3 class="text-3xl font-bold mt-2">{{ stats.totalSimulations }}</h3>
                            </div>
                            <div class="p-3 rounded-lg bg-blue-100 text-blue-500">
                                <Activity class="h-6 w-6" />
                            </div>
                        </div>
                    </div>

                    <div class="bg-white p-6 rounded-xl border border-gray-200 shadow-sm">
                        <div class="flex items-center justify-between">
                            <div>
                                <p class="text-sm text-gray-500">Active Users</p>
                                <h3 class="text-3xl font-bold mt-2">{{ stats.activeUsers }}</h3>
                            </div>
                            <div class="p-3 rounded-lg bg-emerald-100 text-emerald-500">
                                <Users class="h-6 w-6" />
                            </div>
                        </div>
                    </div>

                    <div class="bg-white p-6 rounded-xl border border-gray-200 shadow-sm">
                        <div class="flex items-center justify-between">
                            <div>
                                <p class="text-sm text-gray-500">Avg Duration</p>
                                <h3 class="text-3xl font-bold mt-2">
                                    {{ stats.averageDuration.toFixed(1) }} <span class="text-sm font-normal">sec</span>
                                </h3>
                            </div>
                            <div class="p-3 rounded-lg bg-amber-100 text-amber-500">
                                <Clock class="h-6 w-6" />
                            </div>
                        </div>
                    </div>

                    <div class="bg-white p-6 rounded-xl border border-gray-200 shadow-sm">
                        <div class="flex items-center justify-between">
                            <div>
                                <p class="text-sm text-gray-500">Common Vehicle Count</p>
                                <h3 class="text-3xl font-bold mt-2">{{ stats.mostCommonVehicleCount }}</h3>
                            </div>
                            <div class="p-3 rounded-lg bg-purple-100 text-purple-500">
                                <Car class="h-6 w-6" />
                            </div>
                        </div>
                    </div>
                </div>

                <div class="bg-white rounded-xl border border-gray-200 shadow-sm overflow-hidden">
                    <div class="p-6 border-b border-gray-200">
                        <h3 class="text-lg font-semibold">Recent Simulations</h3>
                    </div>
                    <div class="divide-y divide-gray-200">
                        <div v-for="sim in allSimulations.slice(0, 5)" :key="sim.id"
                            class="p-4 hover:bg-gray-50 transition-colors duration-200">
                            <div class="flex items-center justify-between">
                                <div>
                                    <p class="font-medium">{{ sim.name || 'Unnamed Scenario' }}</p>
                                    <p class="text-sm text-gray-500 mt-1">
                                        Created by {{ sim.user_name }} • {{ formatDate(sim.created_at) }}
                                    </p>
                                </div>
                                <div class="space-x-2">
                                    <NuxtLink :href="`/admin/simulation.${sim.id}`">
                                        <Button  size="sm" >
                                            View
                                            <Eye class="h-4 w-4 ml-2" />
                                        </Button>
                                    </NuxtLink>
                                    <Button variant="outline" size="sm" @click="toggleSimulation(sim.id)">
                                        Details
                                        <ChevronDown class="h-4 w-4 ml-2" />
                                    </Button>
                                </div>
                            </div>

                            <div v-if="expandedSimulation === sim.id" class="mt-4 p-4 bg-gray-50 rounded-lg">
                                <div class="grid grid-cols-2 md:grid-cols-3 gap-4">
                                    <div>
                                        <p class="text-sm text-gray-500">Duration</p>
                                        <p class="font-medium">{{ sim.duration }} seconds</p>
                                    </div>
                                    <div>
                                        <p class="text-sm text-gray-500">Vehicles</p>
                                        <p class="font-medium">{{ sim.vehicles }}</p>
                                    </div>
                                    <div>
                                        <p class="text-sm text-gray-500">Speed</p>
                                        <p class="font-medium">{{ sim.speed }} km/h</p>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <div v-if="selectedTab === 'simulations'" class="space-y-6">
                <div class="flex items-center justify-between">
                    <h2 class="text-2xl font-bold text-gray-800">All User Simulations</h2>
                    <Button variant="outline" @click="fetchAllSimulations">
                        Refresh Data
                    </Button>
                </div>

                <div class="bg-white rounded-xl border border-gray-200 shadow-sm overflow-hidden">
                    <div class="overflow-x-auto">
                        <table class="min-w-full divide-y divide-gray-200">
                            <thead class="bg-gray-50">
                                <tr>
                                    <th scope="col"
                                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                                        Scenario
                                    </th>
                                    <th scope="col"
                                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                                        User
                                    </th>
                                    <th scope="col"
                                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                                        Vehicles
                                    </th>
                                    <th scope="col"
                                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                                        Duration
                                    </th>
                                    <th scope="col"
                                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                                        Created
                                    </th>
                                    <th scope="col"
                                        class="px-6 py-3 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                                        Actions
                                    </th>
                                </tr>
                            </thead>
                            <tbody class="bg-white divide-y divide-gray-200">
                                <tr v-for="sim in allSimulations" :key="sim.id"
                                    class="hover:bg-gray-50 transition-colors duration-150">
                                    <td class="px-6 py-4 whitespace-nowrap">
                                        <div class="flex items-center">
                                            <Map class="h-5 w-5 text-blue-500 mr-2" />
                                            <div>
                                                <p class="font-medium">{{ sim.name || 'Unnamed Scenario' }}</p>
                                                <p class="text-sm text-gray-500">{{ sim.path }}</p>
                                            </div>
                                        </div>
                                    </td>
                                    <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                                        {{ sim.user_name }}
                                    </td>
                                    <td class="px-6 py-4 whitespace-nowrap">
                                        <span
                                            class="px-2 inline-flex text-xs leading-5 font-semibold rounded-full bg-green-100 text-green-800">
                                            {{ sim.vehicles }} vehicles
                                        </span>
                                    </td>
                                    <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                                        {{ sim.duration }} sec
                                    </td>
                                    <td class="px-6 py-4 whitespace-nowrap text-sm text-gray-500">
                                        {{ formatDate(sim.created_at) }}
                                    </td>
                                    <td class="px-6 py-4 whitespace-nowrap text-sm font-medium">
                                        <Button variant="ghost" size="sm" @click="toggleSimulation(sim.id)">
                                            {{ expandedSimulation === sim.id ? 'Hide' : 'View' }} details
                                        </Button>
                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>

                <div v-if="expandedSimulation" class="bg-white rounded-xl border border-gray-200 shadow-sm p-6">
                    <div class="flex items-center justify-between mb-4">
                        <h3 class="text-lg font-semibold">
                            Simulation Details
                        </h3>
                        <Button variant="ghost" size="sm" @click="expandedSimulation = null">
                            Close
                        </Button>
                    </div>

                    <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                        <div>
                            <h4 class="font-medium text-gray-700 mb-3">Basic Parameters</h4>
                            <div class="space-y-3">
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">Scenario Name</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.name || 'N/A'}}
                                    </span>
                                </div>
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">Duration</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.duration || 'N/A'}}
                                        seconds
                                    </span>
                                </div>
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">Vehicles</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.vehicles || 'N/A'}}
                                    </span>
                                </div>
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">Speed</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.speed || 'N/A'}} km/h
                                    </span>
                                </div>
                            </div>
                        </div>

                        <div>
                            <h4 class="font-medium text-gray-700 mb-3">Network Parameters</h4>
                            <div class="space-y-3">
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">Vehicle Cache</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.cache_v || 'N/A'
                                        }} MB
                                    </span>
                                </div>
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">RSU Cache</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.cache_rsu || 'N/A'}}
                                        MB
                                    </span>
                                </div>
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">Vehicle TX Power</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.tx_v ||
                                            'N/A'}} mW
                                    </span>
                                </div>
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">RSU TX Power</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.tx_rsu || 'N/A'}}
                                        mW
                                    </span>
                                </div>
                                <div class="flex justify-between border-b border-gray-100 pb-2">
                                    <span class="text-gray-500">MBS TX Power</span>
                                    <span class="font-medium">
                                        {{allSimulations.find(s => s.id === expandedSimulation)?.tx_mbs || 'N/A'}}
                                        mW
                                    </span>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </main>
    </div>
</template>
