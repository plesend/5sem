const WebSocket = require('ws');
const ws = new WebSocket('ws://localhost:4000');

const clientName = process.argv[2];
if(!clientName)
{
    console.log('usage: node 11-04a.js {clientName}');
    return 1;
}

ws.on('open',()=>
{
    console.log('connected');

    const msg = {
        client: clientName,
        timestamp: Date.now()
    };

    ws.send(JSON.stringify(msg));
});
ws.on('message',(msg)=>
{
    let message = JSON.parse(msg);
    console.log(`server says: ${JSON.stringify(message,null,1)}`);
});
ws.on('close',()=>
{
    console.log('disconnected');
})