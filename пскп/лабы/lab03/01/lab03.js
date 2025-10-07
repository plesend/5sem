const http = require("http");
const readln = require("readline");

let state = "norm";

const serverFunction = function(req, res) {
    if (req.url == "/" && req.method == 'GET') {
        res.setHeader("Content-Type", "text/html; charset=utf-8;");
        res.statusCode = 200;
        res.end(`<!DOCTYPE html>
                <html lang="en">
                <head>
                    <meta charset="UTF-8">
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <title>Document</title>
                </head>
                <body>
                    <h1>${state}</h1>
                    </body>
                </html>`);
    }
    else {
        res.setHeader("Content-Type", "text/html; charset=utf-8;");
        res.statusCode = 404;
        res.end("<h1>Not Found</h1>");
    }
}

const server = http.createServer(serverFunction);
server.listen(5000, function() {
    console.log("Сервер запускается!");
});

const rl = readln.createInterface({
    input: process.stdin,
    output: process.stdout
})

const HandleInput = () => {
    rl.question(`${state} --> `, (input) => {
        input = input.trim().toLowerCase();

        if(input === 'exit') {
            console.log("Exiting the program");
            rl.close();
            process.exit();
        }
        else if(["norm", "stop", "idle", "test"].includes(input)) {
            console.log(`reg = ${state}->${input}`);
            state = input;
        }
        else {
            console.log(`${state}-->${input}. Invalid state`);
        }

        HandleInput();
    });
}
HandleInput();