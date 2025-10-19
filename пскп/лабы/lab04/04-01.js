const http = require('http');
const DB = require('./DB_Module');
const url = require('url');

const db = new DB();
const port = 5000;

const server = http.createServer((req, res) => {
    res.statusCode = 200;
    res.setHeader("Content-Type", "application/json");

    const parsedUrl = url.parse(req.url, true);
    let body = '';

    req.on('data', chunk => body += chunk);
    req.on('end', () => {
        if(parsedUrl.pathname.startsWith("/api/db")) {
            switch(req.method) {
                case 'GET':
                    db.emit('GET', req, res);
                    break;

                case 'POST':
                    db.emit('POST', req, res, body);
                    break;

                case 'PUT':
                    db.emit('PUT', req, res, body);
                    break;

                case 'DELETE': {
                    const id = parseInt(parsedUrl.query.id);
                    db.emit('DELETE', req, res, id);
                    break;
                }

                default:
                    res.statusCode = 405;
                    res.end(JSON.stringify({ error: 'Method not allowed' }));
            }
        } else {
            res.statusCode = 404;
            res.end(JSON.stringify({ error: 'Not found' }));
        }
    });
});

server.listen(port, () => {
    console.log(`Сервер запущен на порту ${port}`);
});
