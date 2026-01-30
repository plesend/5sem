const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

let fileDir = path.join(__dirname,"download");

const wss =new  WebSocket.Server({port:4000});

wss.on('connection',(ws)=>
{
    console.log('connection');

    ws.on('message',(data)=>
    {
        const filepath = path.join(fileDir,data.toString());

        fs.readFile(filepath,(err,filedata)=>
        {
            if(err)
            {
                console.log(err);
                ws.send(`${data} does not exists`);
            }
            else
            {
                console.log('file sent');
                ws.send(filedata);
            }
        });
    });
    ws.on('close',()=>
    {
        console.log('client disconnected');
    })
});