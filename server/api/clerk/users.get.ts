import { clerkClient } from "@clerk/nuxt/server"

export default defineEventHandler(async (event) => {
    try {
        const client = await clerkClient(event)
        const { data, totalCount } = await client.users.getUserList()
        console.log(totalCount);

        return data
    } catch (error) {
        console.log(error);
    }
})

