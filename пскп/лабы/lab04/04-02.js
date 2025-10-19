const http = require('http');
const fs = require('fs');
const DB = require('./DB_Module');
const url = require('url');

const db = new DB();
const port = 5000;

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    let body = '';

    req.on('data', chunk => body += chunk);
    req.on('end', () => {

        // Отдача HTML-страницы
        if(parsedUrl.pathname === "/") {
            fs.readFile('index.html', (err, data) => {
                if(err) {
                    res.statusCode = 500;
                    res.end("Error reading HTML file");
                    return;
                }
                res.statusCode = 200;
                res.setHeader("Content-Type", "text/html");
                res.end(data);
            });
            return;
        }

        // Работа с API
        if(parsedUrl.pathname.startsWith("/api/db")) {
            res.setHeader("Content-Type", "application/json");

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
