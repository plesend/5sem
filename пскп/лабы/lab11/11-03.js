const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const wss = new WebSocket.Server({port: 4000});
let n = 0;

function checkAlive() {
    this.isAlive = true;
}

wss.on('connection', (ws) => {
    console.log('client connected');
    ws.on('pong', checkAlive);

    ws.on('close', () => {
        console.log('client disconnected');
    });
});


setInterval(() => {
    wss.clients.forEach(c => {
        if (c.readyState === WebSocket.OPEN) {
            c.send(`11-03-server: ${n++}`);
        }
    })
}, 15000);

setInterval(() => {
    let aliveClients = 0;
    wss.clients.forEach(c => {
        if (c.isAlive === false) c.terminate();
        c.isAlive = false;
        c.ping();
        aliveClients++;
    });
    console.log('alive connections : ' + aliveClients);
}, 5000);

