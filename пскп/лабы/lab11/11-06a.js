const WebSocket = require('rpc-websockets').Client;
const ws = new WebSocket('ws://localhost:4000');

ws.on('open', () => {
    console.log('connected');
    ws.subscribe('A');
});

ws.on('A', () => {
    console.log('event A');
});



