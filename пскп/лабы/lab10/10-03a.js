const WebSocket = require('ws');

const ws = new WebSocket('ws://localhost:4000');

ws.on('open',()=>
{
    console.log('connected');

    setInterval(() => {
        ws.send('client!!');
    }, 3000);
});
ws.on('message',(data)=>
{
    console.log(`message from server: ${data.toString()}`);
});