const http = require("http");
const fs = require("fs");

const server = http.createServer(function(req, res) {
    if(req.url == "/xmlhttprequest") {

        let filepath = "xmlhttprequest.html";

        fs.access(filepath, fs.constants.R_OK, err => {
                res.setHeader("Content-Type", "text/html; charset=utf-8");
            if(err){
                res.end("oshibka");
            }
            else {
                fs.createReadStream(__dirname + '\\xmlhttprequest.html').pipe(res);
            }
        })
    }
    if(req.url == "/api/name") {
        res.end("Фамилия Имя Отчество");
    }
});

server.listen(5000, function() {
    console.log("Запускаю!");
});