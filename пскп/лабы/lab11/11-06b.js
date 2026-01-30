const WebSocket = require('rpc-websockets').Client;
const ws = new WebSocket('ws://localhost:4000');

ws.on('open', () => {
    console.log('connected');
    ws.subscribe('B');
});

ws.on('B', () => {
    console.log('event B');
});



