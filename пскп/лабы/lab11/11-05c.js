const WebSocket = require('rpc-websockets').Client;
const ws = new WebSocket('ws://localhost:4000');

async function CallMethod(method, array) {
    return ws.call(`${method}`, array);
}

ws.on('open', async () => {
    await ws.login({'login' : 'student', 'password': 'fitfit'});
    const sq3 = await CallMethod('square', [3]);
    const sq54 = await CallMethod('square', [5, 4]);
    const mul35791113 = await CallMethod('mul', [3, 5, 7, 9, 11, 13]);
    const fib7 = await CallMethod('fib', [7]);
    const mul246 = await CallMethod('mul', [2, 4, 6]);

    const sum3parms = await CallMethod('sum',[sq3, sq54, mul35791113] );
    const fibmul = await CallMethod('mul', [fib7, mul246]);
    const sumMain = await CallMethod('sum', [sum3parms, fibmul]);

    console.log(`result OF ALL: ${sumMain}`);
});