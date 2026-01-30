const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

let filepath = path.join(__dirname,"upload");

const ws = new WebSocket('ws://localhost:4000');

ws.on('open',()=>
{
    console.log('connected to server');
    ws.send("lab07.png");

    ws.on('message',(msg)=>
    {
        fs.writeFile(path.join(filepath,"lab07.png"),msg,err=>
        {
            if(err)
            {
                console.log('error writing file');
                ws.close();
            }
            else
            {
                console.log('file written succesfully');
                ws.close();
            }
        });
    });
})