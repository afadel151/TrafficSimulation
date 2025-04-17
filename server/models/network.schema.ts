import { defineMongooseModel } from '#nuxt/mongoose'

export const NetworkSchema = defineMongooseModel({
  name: 'Network',
  schema: {
  name: {
    type: 'string',
    required: true
  },
  slug: {
    type: 'string',
    required: true,
    unique: true
  }
},
})
