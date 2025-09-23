const http = require("http");

const server = http.createServer(function(req, res) {

    res.setHeader("Content-Type", "text/plain; charset=utf-8");  

    if(req.method = "GET" || req.url == "/api/name") { 
        res.end("Фамилия Имя Отчество");
    }
    else {
        res.statusCode = 404;
        res.end("нот фаунд");
    }
});

server.listen(5000, function() {
    console.log("Запускаю!")
});