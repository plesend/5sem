const http = require("http");
const fs = require("fs");

const server = http.createServer(function(req, res) {
    if (req.url == "/fetch" && req.method == "GET") {
        let filepath = "fetch.html";

        fs.access(filepath, fs.constants.R_OK, err => {
            res.setHeader("Content-Type", "text/html; charset=utf-8;");
            if(err) {
                res.statusCode = 404;
                res.end("Not found");
            }
            else{
                res.statusCode = 200;
                fs.createReadStream(__dirname + '\\fetch.html').pipe(res);
            }
        });
        return;
    }
    if (req.url == "/api/name" && req.method == "GET") {
        res.setHeader("Content-Type", "text/html; charset=utf-8;");
        res.end("Фамилия Имя Отчество");
    }
});

server.listen(5000, function() {
    console.log("Запускаем!");
});