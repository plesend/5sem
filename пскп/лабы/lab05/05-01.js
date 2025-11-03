const http = require('http');
const fs = require('fs');
const DB = require('./DB_Module');
const url = require('url');

const db = new DB();
const port = 5000;

let sdTimer = null;
let scTimer = null;
let ssTimer = null;
let isCollectingStats = false;

let reqCount = 0;
let commitsCount = 0;

let statsStartTime = '';
let statsEndTime = '';

const RegisterEventHandlers = () => {
    db.on('GET', async (req, res) => {
        //if(isCollectingStats) reqCount++;
        res.end(JSON.stringify(await db.select()));
    });

    db.on('POST', async (req, res, body) => {
        try {
            //if(isCollectingStats) reqCount++;
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
            //if(isCollectingStats) reqCount++;
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
            //if(isCollectingStats) reqCount++;
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

    db.on('COMMIT', async (req, res) => {
        commitsCount++;
        let commit = await db.commit();
        res.end(commit);
    });
};

RegisterEventHandlers();


const server = http.createServer((req, res) => {
    if(isCollectingStats) reqCount++;
    
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
        } 

        if (req.url === '/api/ss') {
            res.setHeader("Content-Type", "application/json");
            let startTime = "";
            let endTime = "";
            if (isCollectingStats !== true) {
                startTime = statsStartTime;
                endTime = statsEndTime;
            }
            let statsJson = {start: startTime, finish: endTime, request: reqCount, commit: commitsCount};
            res.end(JSON.stringify(statsJson));
        }
    });
});

server.listen(port, () => {
    console.log(`Сервер запущен на порту ${port}`);
});

process.stdin.setEncoding('utf8');

process.stdin.on('data', (data) => {
    const input = data.trim();
    const [cmd, parm] = input.split(/\s+/);
    let delay = parseInt(parm) * 1000;

    switch(cmd) {
        case 'sd': 
            if(sdTimer) {
                clearTimeout(sdTimer);
                sdTimer = null;
                console.log("sd: timer stopped");
            }
            if (parm) {
                sdTimer = setTimeout(() => {
                    console.log(`server will be shut in ${parm}`)
                    process.exit(0);
                }, delay)
            } else {
                console.log("sd: server wont shutdown");
            }
            break;

        case 'sc':
            if (scTimer){
                clearInterval(scTimer);
                scTimer = null;
                console.log("sc: timer stopped");
            }
            if (parm) {
                scTimer = setInterval(async () => {
                    await db.commit();
                    commitsCount++;
                }, delay);
                scTimer.unref();
                console.log(`commit every ${parm} seconds`);
            }
            break;

        case 'ss':
            if (ssTimer) {
                clearTimeout(ssTimer);
                ssTimer = null;
                console.log("ss: timer stopped");
            }
            if(parm) {
                isCollectingStats = true;
                reqCount= 0;
                commitsCount = 0;

                ssTimer = setTimeout(() => {
                    isCollectingStats = false;
                    console.log(`ss: for ${parm} collected ${reqCount} requests, 
                        ${commitsCount} commits`);
                    statsEndTime = new Date();
                }, delay);
                statsStartTime = new Date();
                ssTimer.unref();
                
            }
            break;

        default: console.log("wrong command: sd, sc, ss");
    }
})
