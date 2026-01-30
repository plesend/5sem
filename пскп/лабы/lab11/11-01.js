const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

let fileDir = path.join(__dirname,"upload");

const wss = new  WebSocket.Server({port:4000});

wss.on('connection',(ws)=>
{
    console.log('connection');

    ws.on('message',(data)=>
    {
        const filename = `toserv_${Date.now()}`;
        const filepath = path.join(fileDir,filename);

        fs.writeFile(filepath,data,err=>
        {
            if(err)
            {
                console.log(err);
                ws.send('write file error');
            }
            else
            {
                console.log('file saved');
                ws.send('write file successfull');
            }
        });
    });
    ws.on('close',()=>
    {
        console.log('client disconnected');
    })
});