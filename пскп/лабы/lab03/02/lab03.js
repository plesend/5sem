const http = require('http');
const url = require("url");

const CountFactorial = function(k) {
    if(k < 0 || isNaN(k) ) {
        throw new Error("k is Invalid");
    }
    if(k === 0 || k === 1) {
        return 1;
    }
    if (k > 1) {
        return k * CountFactorial(k-1);
    }
}

const server = http.createServer(function(req, res) {
    res.setHeader("Content-Type", "application/json; charset=utf-8");
    res.statusCode = 200;

    let parsedUrl = url.parse(req.url, true);
    if (parsedUrl.pathname === "/fact") {

        let k = parseInt(parsedUrl.query.k);

        try {
            const fact = CountFactorial(k);
            const result = { k: k, factorial: fact };
            res.end(JSON.stringify(result));
        } catch (error) {
            res.statusCode = 400;
            res.end(JSON.stringify({ error: error.message }));
        }
    } else {
        res.statusode = 404;
        res.end(JSON.stringify({ error: "Not Found" }));
    }
});

server.listen(5000, function() {
    console.log("Сервер запускается! ");
});