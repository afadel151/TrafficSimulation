<script setup lang="ts">
import {
  CarIcon,
  ActivityIcon,
  BarChart3Icon,
  ShieldIcon,
  UsersIcon,
  MapIcon,
  ArrowRightIcon,
  ChevronDownIcon,
} from "lucide-vue-next";
import { onMounted, ref } from "vue";
import gsap from "gsap";
import { ScrollTrigger } from "gsap/ScrollTrigger";
import { Protect } from '@clerk/vue'
gsap.registerPlugin(ScrollTrigger);

const carElement = ref(null);
const landingDiv = ref(null);
const landingTitle = ref(null);
const landingDescription = ref(null);
const landingButtons = ref(null);
const featuresSection = ref(null);
const statsSection = ref(null);

const menuItems = [
  { label: "Features", route: "#features" },
  { label: "Solutions", route: "#solutions" },
  { label: "Pricing", route: "#pricing" },
  { label: "About", route: "#about" },
];

const features = [
  {
    icon: ActivityIcon,
    title: "Real-time Analysis",
    description: "Monitor traffic patterns and get instant insights with our advanced analytics engine.",
    color: "bg-emerald-100 text-emerald-600"
  },
  {
    icon: BarChart3Icon,
    title: "Predictive Modeling",
    description: "Use AI-powered predictions to forecast traffic conditions and optimize flow.",
    color: "bg-blue-100 text-blue-600"
  },
  {
    icon: ShieldIcon,
    title: "Safety Metrics",
    description: "Evaluate and improve road safety with comprehensive safety analysis tools.",
    color: "bg-amber-100 text-amber-600"
  },
  {
    icon: MapIcon,
    title: "Urban Planning",
    description: "Design and test urban layouts before implementation with our simulation tools.",
    color: "bg-purple-100 text-purple-600"
  },
  {
    icon: UsersIcon,
    title: "Collaboration Tools",
    description: "Work seamlessly with your team on complex traffic projects.",
    color: "bg-rose-100 text-rose-600"
  },
  {
    icon: CarIcon,
    title: "Vehicle Simulation",
    description: "Simulate individual vehicle behaviors in complex traffic scenarios.",
    color: "bg-cyan-100 text-cyan-600"
  }
];

const statistics = [
  { value: 1000, suffix: "+", label: "Active Users" },
  { value: 50, suffix: "K", label: "Simulations Run" },
  { value: 95, suffix: "%", label: "Accuracy Rate" },
  { value: 30, suffix: "+", label: "City Partners" },
];

onMounted(() => {
  gsap.fromTo(
    carElement.value,
    { x: '100vw' },
    {
      x: '0',
      duration: 1.5,
      ease: 'power3.out',
      scrollTrigger: {
        trigger: carElement.value,
        start: "top bottom",
        toggleActions: "play none none none"
      }
    }
  );

  gsap.fromTo(
    landingDiv.value,
    { opacity: 0, y: 50 },
    {
      opacity: 1,
      y: 0,
      duration: 1.5,
      ease: "power3.out",
      scrollTrigger: {
        trigger: landingDiv.value,
        start: "top bottom",
        toggleActions: "play none none none"
      }
    }
  );

  gsap.fromTo(
    ".feature-card",
    {
      opacity: 0,
      y: 50
    },
    {
      opacity: 1,
      y: 0,
      duration: 0.8,
      stagger: 0.1,
      ease: "back.out(1.7)",
      scrollTrigger: {
        trigger: featuresSection.value,
        start: "top 70%",
      }
    }
  );

  gsap.fromTo(
    ".stat-item",
    {
      opacity: 0,
      scale: 0.8
    },
    {
      opacity: 1,
      scale: 1,
      duration: 0.6,
      stagger: 0.1,
      ease: "power2.out",
      scrollTrigger: {
        trigger: statsSection.value,
        start: "top 80%",
      }
    }
  );

  gsap.to(carElement.value, {
    y: 20,
    duration: 3,
    repeat: -1,
    yoyo: true,
    ease: "sine.inOut"
  });
});

const scrollToNext = () => {
  const nextSection = document.getElementById('features');
  nextSection?.scrollIntoView({ behavior: 'smooth' });
};
const user = useUser();
</script>

<template>

  <div class="min-h-screen overflow-x-hidden bg-gradient-to-b from-gray-50 to-gray-100 text-gray-800">
    <header class="fixed w-full px-20 z-50 backdrop-blur-sm bg-white/80 border-b border-gray-200 shadow-sm">
      <nav class="container mx-auto px-6 py-4">
        <div class="flex items-center justify-between">
          <div class="flex items-center space-x-2">
            <CarIcon class="h-8 w-8 text-blue-500" />
            <span
              class="text-xl font-bold bg-clip-text text-transparent bg-gradient-to-r from-blue-500 to-cyan-400">omSET</span>
          </div>
          <div class="flex items-center space-x-4">
            <div class="hidden md:flex space-x-6">
              <Button v-for="item in menuItems" :key="item.label" variant="ghost"
                class="text-gray-600 hover:text-blue-500 transition-colors duration-300">
                {{ item.label }}
              </Button>
            </div>
            <header>
              <SignedOut>
                <SignInButton>
                  <Button class="bg-blue-500 hover:bg-blue-600 text-white transition-colors duration-300 shadow-md">
                    Sign in
                  </Button>
                </SignInButton>
              </SignedOut>
              <SignedIn>
                <UserButton />
              </SignedIn>
            </header>
          </div>
        </div>
      </nav>
    </header>

    <section class="min-h-screen mt-32 flex flex-col justify-center relative pt-20">
      <main class="container mx-auto px-6 flex flex-col items-center justify-center gap-12">
        <div ref="landingDiv" class="text-center max-w-4xl z-10">
          <h1 ref="landingTitle" class="text-5xl md:text-7xl font-bold mb-6 leading-tight">
            Setup veins simulation with
            <span class="bg-clip-text text-transparent bg-gradient-to-r from-blue-500 to-cyan-400">SUMO</span>
          </h1>
          <p ref="landingDescription" class="text-xl md:text-2xl text-gray-600 mb-10 max-w-3xl mx-auto leading-relaxed">
            Create, simulate, and optimize traffic scenarios with our advanced platform. Perfect for urban planners,
            researchers, and traffic engineers.
          </p>
          <div ref="landingButtons" class="flex gap-4 justify-center">
            <SignedIn>
              <NuxtLink href="/simulation">
                <Button
                  class="bg-blue-500 hover:bg-blue-600 px-8 py-6 text-lg font-medium text-white transition-all duration-300 hover:shadow-lg hover:shadow-blue-400/30 flex items-center gap-2">
                  Open Simulation setup
                  <ArrowRightIcon class="h-5 w-5" />
                </Button>
              </NuxtLink>
            </SignedIn>
            <SignedOut>
              <SignInButton>
                <Button
                  class="bg-blue-500 hover:bg-blue-600 px-8 py-6 text-lg font-medium text-white transition-all duration-300 hover:shadow-lg hover:shadow-blue-400/30">
                  Get Started
              
                </Button>
              </SignInButton>
            </SignedOut>
 
             <NuxtLink href="/admin">
              <Button v-if="user.user.value?.organizationMemberships[0].role == 'org:admin'"  variant="outline"
                class="px-8 py-6 text-lg font-medium border-gray-300 hover:bg-gray-100 hover:border-gray-400 text-gray-700">
                Admin page
              </Button>
             </NuxtLink>

            
          </div>
        </div>

        <div class="relative w-full max-w-6xl">
          <div class="absolute inset-0 bg-blue-100 rounded-full -z-10 blur-2xl opacity-70"></div>
          <div
            class="w-full flex justify-center items-center h-64 md:h-96 rounded-2xl border border-gray-200 shadow-xl bg-white/80 backdrop-blur-sm">

            <iframe ref="carElement" class=" h-64 md:h-96 scale-150"
              src="https://lottie.host/embed/109ce8dd-a723-490e-8de6-581a14eb0e17/kuQgh7sKK7.lottie"></iframe>
          </div>
        </div>

        <button @click="scrollToNext"
          class="absolute bottom-10 left-1/2 transform -translate-x-1/2 text-gray-400 hover:text-blue-500 transition-colors duration-300 animate-bounce">
          <ChevronDownIcon class="h-8 w-8" />
        </button>
      </main>
    </section>

    <!-- Features Section -->
    <section id="features" ref="featuresSection" class="py-24 px-20 relative bg-white">
      <div class="container mx-auto px-6">
        <div class="text-center mb-20 max-w-3xl mx-auto">
          <span class="text-blue-500 font-medium uppercase tracking-wider text-sm">Features</span>
          <h2 class="text-4xl md:text-5xl font-bold mt-4 mb-6">
            Powerful Features for <span
              class="bg-clip-text text-transparent bg-gradient-to-r from-blue-500 to-cyan-400">Complex
              Simulations</span>
          </h2>
          <p class="text-xl text-gray-600">
            Our platform offers everything you need to model, analyze, and optimize traffic flow in any environment.
          </p>
        </div>

        <div class="grid md:grid-cols-2 lg:grid-cols-3 gap-8">
          <div v-for="(feature, index) in features" :key="index"
            class="feature-card bg-white p-8 rounded-xl border border-gray-200 hover:border-blue-300 transition-all duration-300 hover:shadow-lg hover:shadow-blue-100 hover:-translate-y-2">
            <div class="mb-6 p-4 rounded-lg w-max" :class="feature.color">
              <component :is="feature.icon" class="h-8 w-8" />
            </div>
            <h3 class="text-xl font-semibold mb-3 text-gray-800">
              {{ feature.title }}
            </h3>
            <p class="text-gray-600">{{ feature.description }}</p>
            <Button variant="link" class="mt-6 px-0 text-blue-500 hover:text-blue-400 flex items-center gap-2">
              Learn more
              <ArrowRightIcon class="h-4 w-4" />
            </Button>
          </div>
        </div>
      </div>
    </section>

    <!-- Stats Section -->
    <section ref="statsSection" class="py-20 px-20 bg-gray-50 border-y border-gray-200">
      <div class="container mx-auto px-6">
        <div class="grid grid-cols-2 md:grid-cols-4 gap-8 text-center">
          <div v-for="(stat, index) in statistics" :key="index"
            class="stat-item p-6 bg-white rounded-xl border border-gray-200 hover:bg-gray-100 transition-colors duration-300 shadow-sm">
            <div class="text-4xl md:text-5xl font-bold mb-2 text-blue-500">
              {{ stat.value }}<span class="text-cyan-400">{{ stat.suffix }}</span>
            </div>
            <div class="text-gray-600 uppercase text-sm tracking-wider">
              {{ stat.label }}
            </div>
          </div>
        </div>
      </div>
    </section>

    <!-- CTA Section -->
    <section class="py-32 relative overflow-hidden bg-gradient-to-r from-blue-50 to-cyan-50">
      <div
        class="absolute inset-0 opacity-10 bg-[url('data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNjAiIGhlaWdodD0iNjAiIHZpZXdCb3g9IjAgMCA2MCA2MCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48ZyBmaWxsPSJub25lIiBmaWxsLXJ1bGU9ImV2ZW5vZGQiPjxnIGZpbGw9IiMxRTFGMjAiIGZpbGwtb3BhY2l0eT0iMC4yIj48Y2lyY2xlIGN4PSIzIiBjeT0iMyIgcj0iMyIvPjwvZz48L2c+PC9zdmc+')]">
      </div>
      <div class="container mx-auto px-6 text-center relative z-10">
        <div class="max-w-4xl mx-auto">
          <h2 class="text-3xl md:text-5xl font-bold mb-8 text-gray-800">
            Ready to transform your traffic planning?
          </h2>
          <p class="text-xl text-gray-600 mb-10">
            Join thousands of urban planners and traffic engineers using Trafficky to create smarter cities.
          </p>
          <div class="flex flex-col sm:flex-row gap-4 justify-center">
            <SignedIn>
              <NuxtLink href="/simulation">
                <Button
                  class="bg-blue-500 hover:bg-blue-600 px-8 py-6 text-lg font-medium text-white transition-all duration-300 hover:shadow-lg hover:shadow-blue-400/30">
                  Start Simulating Now
                </Button>
              </NuxtLink>
            </SignedIn>
            <SignedOut>
              <SignInButton>
                <Button
                  class="bg-blue-500 hover:bg-blue-600 px-8 py-6 text-lg font-medium text-white transition-all duration-300 hover:shadow-lg hover:shadow-blue-400/30">
                  Get Started for Free
                </Button>
              </SignInButton>
            </SignedOut>
            <Button variant="outline"
              class="px-8 py-6 text-lg font-medium border-blue-300 hover:bg-white hover:border-blue-400 text-blue-500">
              Request Demo
            </Button>
          </div>
        </div>
      </div>
    </section>

    <!-- Footer -->
    <footer class="py-12 px-20 bg-white border-t border-gray-200">
      <div class="container mx-auto px-6">
        <div class="flex flex-col md:flex-row justify-between items-center">
          <div class="flex items-center space-x-2 mb-6 md:mb-0">
            <CarIcon class="h-8 w-8 text-blue-500" />
            <span
              class="text-xl font-bold bg-clip-text text-transparent bg-gradient-to-r from-blue-500 to-cyan-400">Trafficky</span>
          </div>
          <div class="flex space-x-6">
            <Button variant="ghost" class="text-gray-600 hover:text-blue-500">Privacy</Button>
            <Button variant="ghost" class="text-gray-600 hover:text-blue-500">Terms</Button>
            <Button variant="ghost" class="text-gray-600 hover:text-blue-500">Contact</Button>
          </div>
        </div>
        <div class="mt-8 pt-8 border-t border-gray-200 text-center text-gray-500 text-sm">
          © 2023 Trafficky. All rights reserved.
        </div>
      </div>
    </footer>
  </div>
</template>

<style>
/* Smooth scrolling */
html {
  scroll-behavior: smooth;
}

/* Custom scrollbar */
::-webkit-scrollbar {
  width: 8px;
}

::-webkit-scrollbar-track {
  background: #f1f1f1;
}

::-webkit-scrollbar-thumb {
  background: #cbd5e1;
  border-radius: 4px;
}

::-webkit-scrollbar-thumb:hover {
  background: #94a3b8;
}
</style>