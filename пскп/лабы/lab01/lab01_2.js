const http = require('http');

const server = http.createServer(function(request, response){
    response.setHeader('Content-Type', "text/html");
    response.end(`<h1>Hello world!</h1>`);
});

server.listen(5500, function() { console.log("Сервер запустили") });