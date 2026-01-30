const WebSocket = require('rpc-websockets').Client;
const ws = new WebSocket('ws://localhost:4000');

ws.on('open', () => {
    console.log('connected to server');
    ws.subscribe('A');
    ws.subscribe('B');
    ws.subscribe('C');
})

process.stdin.on('data', (data) => {
    const msg = data.toString().toUpperCase().trim();
    if(msg === 'A' || msg === 'B' || msg === 'C') {
        console.log('notif sent');
        ws.notify(msg);
    }
});