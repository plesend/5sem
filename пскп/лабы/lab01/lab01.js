const http = require('http');

const server = http.createServer(function(request, response) {
    response.setHeader("Content-Type", "text/html");
    const headers = Object.entries(request.headers).map(([key,value]) => `<p>${key} : ${value}</p>`).join(" ");

    let body = '';

    request.on('data', chunk => {
        body += chunk.toString();
    });

    request.on('end',()=>
    {
        response.end(`Request stuff: method ${request.method}
                                url: ${request.url}
                                headers: ${headers},
                                version: ${request.httpVersion}
                                body: ${body}`);
    });
});
server.listen(5500, function() { console.log("Сервер запустили") });