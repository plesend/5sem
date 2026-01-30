const WebSocket = require("rpc-websockets").Client;

const ws = new WebSocket('ws://localhost:4000');

async function CallMethod(method, array) {
    ws.call(`${method}`, array).then(result => {
        console.log(`${method}, ${array.toString()}: ${result}`);
    });
}

ws.on('open', async function() {
    await ws.login(
        {
            'login': 'student',
            'password': 'fitfit'
        }
    );

    await CallMethod('square', [3]);
    await CallMethod('square', [5,4]);
    await CallMethod('sum', [2]);
    await CallMethod('sum', [2, 4, 6, 8, 10]);
    await CallMethod('mul', [3]);
    await CallMethod('mul', [3, 5, 7, 9, 11, 13]);
    await CallMethod('fib', [1]);
    await CallMethod('fib', [2]);
    await CallMethod('fib', [7]);
    await CallMethod('fact', [0]);
    await CallMethod('fact', [5]);
    await CallMethod('fact', [10]);
});

ws.on('error', (error) => {
    console.log('an error occurred while calling rpc functions: ', error);
});

ws.on('close', () => {
    console.log('disconnected from servak');
});