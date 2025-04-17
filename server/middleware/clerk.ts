import { clerkMiddleware, getAuth } from '@clerk/nuxt/server'

export default clerkMiddleware((event) => {
  const { userId, has } = getAuth(event)
  if (!userId && event.path != '/') {
    throw createError({
      statusCode: 401,
      statusMessage: 'Unauthorized: User not signed in',
    })
  }
  const isAdminRoute = event.path.startsWith('admin')
  const isAdmin = has({
    role: 'simulation_org:admin',
  })
  if (isAdminRoute && !isAdmin) {
    throw createError({
      statusCode: 403,
      statusMessage: 'Unauthorized: Admin access required',
    })
  }
})