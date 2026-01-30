const WebSocket = require('ws');

const wss = new WebSocket.Server({port:4000});
let n =0;
wss.on('connection', (ws)=>{
    ws.on('message', (msg) => {
        let txt = JSON.parse(msg);
        const {client, timestamp} = txt;
        if(client === undefined || timestamp === undefined) {
            ws.send('enter acceptable json');
            return;
        }

        let response = {
            server: n,
            client: client,
            timestamp: timestamp
        }

        n++;
        ws.send(JSON.stringify(response));
    });
    ws.on('close', () => {
        console.log('client disconnected');
    });
});