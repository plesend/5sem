const WebSocket = require('rpc-websockets').Server;
const server = new WebSocket({
    port: 4000,
    host: 'localhost'
});

server.register("A", () => console.log("event A")).public();
server.register("B", () => console.log("event B")).public();
server.register("C", () => console.log("event C")).public();

server.on('connection', () => {
    console.log('client connected');
})

server.on('disconnection', () => {
    console.log('client disconnected');
});
