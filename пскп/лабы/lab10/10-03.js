const WebSocket = require('ws');

let count = 0;
const wss = new WebSocket.Server({port:4000})

wss.on('connection', (ws) => {
  console.log('client connected');

  ws.on('message', (message) => {
    console.log(`Received: ${message} ${count++}`);

    wss.clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
        client.send("sending back: " + message);
      }
    });  });

  ws.on('close', () => {
    console.log('Client disconnected');
  });
});