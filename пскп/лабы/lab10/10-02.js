const WebSocket = require('ws');

let count = 0;
const ws = new WebSocket('ws://localhost:4000');

let intervalObj;

ws.on('open', () => {
  console.log('WebSocket connected');

  intervalObj = setInterval(() => {
    const msg = `10-01-client:${count++}`;
    console.log('sent message:', msg);
    ws.send(msg);
  }, 3000);

  setTimeout(() => {
    clearInterval(intervalObj);
    console.log('ws connection aborted');
    ws.close();
  }, 25000);
});

ws.on('message', (data) => {
  console.log(`message from server: ${data}`);
});
