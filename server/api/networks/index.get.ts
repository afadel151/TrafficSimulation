export default defineEventHandler(async (event) => {
  try {
    return await NetworkSchema.find({})
  }
  catch (error) {
    return error
  }
})
