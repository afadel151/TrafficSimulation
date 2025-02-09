<template>
    <div class="relative min-h-screen">
        <!-- Background Image with Overlay -->
        <div class="absolute inset-0 z-0 bg-cover bg-center bg-no-repeat" :style="{
            backgroundImage: `url(${bgImage})`
        }">
            <div class="absolute inset-0 bg-blue-900/25 backdrop-blur-sm"></div>
        </div>

        <!-- Content -->
        <div class="relative z-10">
            <!-- Header -->
            <header class="container mx-auto px-6 py-8">
                <nav class="flex items-center justify-between">
                    <div class="flex items-center space-x-2 text-white">
                        <CarIcon class="h-8 w-8" />
                        <span class="text-xl font-bold">TrafficSim</span>
                    </div>
                    <div class="hidden md:flex space-x-8 text-white">
                        <Button label="Features" variant="text"  severity="secondary"/>
                        <Button label="About" variant="text" severity="secondary"/>
                        <Button label="Contact" variant="text" severity="secondary"/>
                        
                    </div>
                    <header>
                        <SignedOut>
                            <SignInButton>
                                
                                <Button severity="help" rounded label="Sign in" />
                                </SignInButton>

                        </SignedOut>
                        <SignedIn>
                            <UserButton />
                        </SignedIn>
                    </header>

                </nav>
            </header>

            <!-- Hero Section -->
            <main class="container min-h-[700px] mx-auto px-6 py-40 pb-32">
                <div class="max-w-3xl">
                    <h1 class="text-5xl md:text-6xl font-bold text-gray-100 mb-6">
                        Advanced Traffic Simulation for Smart Cities
                    </h1>
                    <p class="text-xl text-gray-300 mb-8">
                        Simulate, analyze, and optimize traffic flow with our cutting-edge platform.
                        Make data-driven decisions for better urban mobility.
                    </p>
                    <div class="flex flex-col sm:flex-row gap-4">
                        
                        <SignedIn>
                            <Button severity="secondary" label="dashboard" />
                        </SignedIn>
                    </div>
                </div>
            </main>

            <!-- Features Section -->
            <section class="bg-white     py-24">
                <div class="container mx-auto px-6">
                    <h2 class="text-3xl font-bold text-white text-center mb-16">
                        Powerful Features for Complex Simulations
                    </h2>
                    <div class="grid md:grid-cols-3 gap-12">
                        <div v-for="(feature, index) in features" :key="index"
                            class="bg-gray-200 p-8 rounded-2xl hover:bg-white/10 transition">
                            <div class="text-violet-300 mb-4">
                                <component :is="feature.icon" class="h-8 w-8" />
                            </div>
                            <h3 class="text-xl font-semibold text-black mb-3">{{ feature.title }}</h3>
                            <p class="text-gray-800">{{ feature.description }}</p>
                        </div>
                    </div>
                </div>
            </section>
        </div>
    </div>
</template>

<script setup lang="ts">
import { CarIcon, ActivityIcon, BarChart3Icon, ShieldIcon } from 'lucide-vue-next'

import bgImage from "@/assets/imgs/urban-city.jpg";
const features = [
    {
        icon: ActivityIcon,
        title: "Real-time Analysis",
        description: "Monitor traffic patterns and get instant insights with our advanced analytics engine."
    },
    {
        icon: BarChart3Icon,
        title: "Predictive Modeling",
        description: "Use AI-powered predictions to forecast traffic conditions and optimize flow."
    },
    {
        icon: ShieldIcon,
        title: "Safety Metrics",
        description: "Evaluate and improve road safety with comprehensive safety analysis tools."
    }
]
</script>

