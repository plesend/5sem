const WebSocket = new require('rpc-websockets').Client;
const ws = new WebSocket('ws://localhost:4000');

ws.on('open', () =>{
    console.log('connected');
    ws.subscribe('postbackup');
        ws.subscribe('deletebackup');
} );

ws.on('postbackup', (data) => {
    console.log('POST backup:', data);
});

ws.on('deletebackup', (data) => {
    console.log('DELETE backup:', data);
});
