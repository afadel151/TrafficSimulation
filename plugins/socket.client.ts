import { Store } from 'vuex';

export default ({ store }: { store: Store<any> }, inject: (arg0: string, arg1: WebSocket) => void) => {
    const socket = new WebSocket('ws://simulation:3001')
    
    socket.onmessage = (event) => {
      const data = JSON.parse(event.data)
      store.commit('updatePositions', data)
    }
    
    inject('socket', socket)
  }