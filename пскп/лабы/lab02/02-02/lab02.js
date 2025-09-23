const http = require("http");
const fs = require("fs");

const server = http.createServer(function(request, response) {
    let filepath = "pic.png";

    fs.access(filepath, fs.constants.R_OK, err => {
        if(err) { response.statusCode = 404; response.end("no pic"); }
        else { 
            if(filepath.endsWith(".png")) {
                response.setHeader("Content-Type", "image/png");
            }
            fs.createReadStream(filepath).pipe(response);
        }
    });
});

server.listen(5000, function() {
    console.log("Запускаю!");
});