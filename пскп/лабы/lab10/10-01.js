const http = require('http');
const fs = require('fs');
const ws = require('ws');

const PORT = 3000;
const WSPORT = 4000;

const server = http.createServer((req, res) => {
    if (req.method == 'GET' && req.url == '/start'){
        res.setHeader('Content-Type', 'text/html');
        fs.createReadStream('index.html').pipe(res);
    }
    else {
        res.statusCode = 400;
        res.end('path not supported');
    }
});

server.listen(PORT, () => {
  console.log(`Server listening on http://localhost:${PORT}`);
});

const wsServer = new ws.Server({port:WSPORT});

wsServer.on('connection', (ws) => {
    let iter = 0;
    let clientNum;
    console.log('connection event');

    ws.on('message', (message)=> {
        console.log('message: ' + message);
        clientNum = message.toString().split(':')[1].trim();
    });

    setInterval(() => {
        ws.send(`10-01-server: ${clientNum}->${iter++}`);
    }, 5000);

});