const WebSocket = require("rpc-websockets").Client;
const async = require('async');
const ws = new WebSocket('ws://localhost:4000');

ws.on('open', async function() {
    ws.login(
        {
            'login': 'student',
            'password': 'fitfit'
        }
    ).then(() => {
        async.parallel({
            square1: (cllbck) => {
                ws.call('square', [3])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },
            square2: (cllbck) => {
                ws.call('square', [5, 4])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },

            sum1: (cllbck) => {
                ws.call('sum', [2])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },
            sum2: (cllbck) => {
                ws.call('sum', [2, 4, 6, 8, 10])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },

            mul1: (cllbck) => {
                ws.call('mul', [3])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },
            mul2: (cllbck) => {
                ws.call('mul', [3, 5, 7, 9, 11, 13])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },

            fib1: (cllbck) => {
                ws.call('fib', [1])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },
            fib2: (cllbck) => {
                ws.call('fib', [2])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },
            fib3: (cllbck) => {
                ws.call('fib', [7])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },

            fact1: (cllbck) => {
                ws.call('fact', [0])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },
            fact2: (cllbck) => {
                ws.call('fact', [2])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            },
            fact3: (cllbck) => {
                ws.call('fact', [10])
                    .then(res => cllbck(null, res))
                    .catch(e => cllbck(e));
            }
        }, (e, res) => {
            if (e) {
                console.log('CALL PROBLEMO');
            }
            else {
                console.log('here u are: ');
                for(let key in res) {
                    console.log(`${key}: ${JSON.stringify(res[key])}`);
                }
            }
        }
    )
}).catch(() => {console.log('AUTH PROBLEMO')});
});
