const http = require("http");
const fs = require("fs");

const server = http.createServer(function(req, res) {
    if (req.url == "/jquery" && req.method == "GET") {

        let filepath = "jquery.html";

        fs.access(filepath, fs.constants.R_OK, err => {
            if(err) {
                res.setHeader("Content-Type", "text/html; charset:utf-8;");
                res.statusCode = 404;
                res.end("no file like this");
            }
            else {
                res.setHeader("Content-Type", "text/html; charset:utf-8;");
                res.statusCode = 200;
                fs.createReadStream(__dirname + "\\jquery.html").pipe(res);
            }
        });
    }
    if(req.url == "/api/name" && req.method == "GET") {
        res.setHeader("Content-Type", "text/html; charset:utf-8;");
        res.end("Фамилия Имя Отчество");
    }
});