const http = require("http");
const fs = require("fs");

const server = http.createServer(function(request, response) {
    let filepath = "BillGates.webp";

    fs.access(filepath, fs.constants.R_OK, err => {
        if(err) { response.statusCode = 404; response.end("no pic"); }
        else { 
            if(filepath.endsWith(".webp")) {
                response.setHeader("Content-Type", "image/webp");
            }
            fs.createReadStream(filepath).pipe(response);
        }
    });
});

server.listen(5000, function() {
    console.log("Запускаю!");
});