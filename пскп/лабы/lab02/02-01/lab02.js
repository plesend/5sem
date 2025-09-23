const http = require('http');
const fs = require('fs');

const server = http.createServer(function(request, response) {
    console.log(`адрес: ${request.url}`);

    let filepath = "index.html";

    fs.access(filepath, fs.constants.R_OK, err => {
       if(err) {
        response.statusCode = 404;
        response.end("not found :(");
       }
       else {
        if(filepath.endsWith(".html")) {
            response.setHeader("Content-Type", "text/html; charset=utf-8");
        }
        fs.createReadStream(filepath).pipe(response);
       }
    });
})

server.listen(5000, function(){
    console.log("Запускаем сервер с портом 5000");
});