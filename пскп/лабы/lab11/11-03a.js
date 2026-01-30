const WebSocket = require('ws');
const fs = require('fs');
const path = require('path');

const ws = new WebSocket('ws://localhost:4000');

ws.on('open',()=>
{
    console.log('connected');
});

ws.on('message',(message)=>
{
    console.log(`from server: ${message}`);
});

ws.on('close',()=>
{
    console.log('disconnected');
});