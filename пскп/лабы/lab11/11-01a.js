const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

let filepath = path.join(__dirname, "upload", "10-01a.txt");

const ws = new WebSocket('ws://localhost:4000');

ws.on('open',()=>
{
    console.log('connected to server');

    fs.readFile(filepath,(err,data)=>
    {
        if(err)
        {
            console.log('read file error');
            ws.close();
        }
        else
        {
            ws.send(data);
            console.log('file sent');
        }
    });
    ws.on('message',(msg)=>
    {
        console.log(`server: ${msg}`);
    });
})