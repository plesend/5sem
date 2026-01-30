const WebSocket = require('rpc-websockets').Server;

var server = new WebSocket({
    port: 4000,
    host: 'localhost'
});


server.event('A');
server.event('B');
server.event('C');

server.on('connection', () => {
    console.log('client connected');
});

process.stdin.setEncoding('utf-8');
process.stdin.on('data', (data) => {
    let evt = data.toString().toLocaleUpperCase().trim();
    if (evt === 'A' || evt === 'B' || evt === 'C') {
        console.log('caught event: ', evt);
        server.emit(evt);
    } else {
        console.log('PROBLEMO');
    }
});

server.on('disconnection', () => {
    console.log('client disconnected');
})
