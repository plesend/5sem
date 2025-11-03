const http = require('http');
const fs = require('fs');
const DB = require('./DB_Module');
const url = require('url');

const db = new DB();
const port = 5000;

const RegisterEventHandlers = () => {
    db.on('GET', async (req, res) => {
        res.end(JSON.stringify(await db.select()));
    });

    db.on('POST', async (req, res, body) => {
        try {
            let newItem = JSON.parse(body);
            console.log("Post body:", newItem);
            const inserted = await db.insert(newItem);
            res.end(JSON.stringify(inserted));
        } catch (err) {
            res.statusCode = 500;
            res.end(JSON.stringify({ error: err.message }));
        }
    });

    db.on('PUT', async (req, res, body) => {
        try {
            let newItem = JSON.parse(body);
            let updItem = await db.update(newItem);
            res.end(JSON.stringify(updItem));
        } catch (err) {
            res.statusCode = 400;
            res.end(JSON.stringify({ error: err.message }));
        }
    });

    db.on('DELETE', async (req, res, id) => {
        try {
            if (id === null || id === undefined) {
                res.statusCode = 400;
                res.setHeader('Content-Type', 'text/html');
                res.end("<h1>DELETE: id is undefined</h1>");
                return;
            }
            let delItem = await db.delete(id);
            res.end(JSON.stringify(delItem));
        } catch (err) {
            res.statusCode = 400;
            res.end(JSON.stringify({ error: err.message }));
        }
    });
};

RegisterEventHandlers();


const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    let body = '';

    req.on('data', chunk => body += chunk);
    req.on('end', () => {

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
