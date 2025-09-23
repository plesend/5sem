const http = require("http");
const readln = require("readline");

let state = "norm";

const server = http.createServer(function(req, res){
    res.setHeader("Content-Type", "text/html; charset=utf-8;");
    res.end(`${state}`);
});