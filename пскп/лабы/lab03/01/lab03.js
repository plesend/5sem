const http = require("http");

let state = "norm";

const serverFunction = function(req, res) {
    if (req.url == "/" && req.method == 'GET') {
        res.setHeader("Content-Type", "text/html; charset=utf-8;");
        res.statusCode = 200;
        res.end(`<h1>${state}</h1> `);
    }
    else {
        res.setHeader("Content-Type", "text/html; charset=utf-8;");
        res.statusCode = 404;
        res.end("<h1>Not Found</h1>");
    }
}

const server = http.createServer(serverFunction);
server.listen(5000, function() {
    console.log("Сервер запускается!");
});

process.stdin.setEncoding("utf-8");
process.stdin.on("readable", () => {
  let chunk = null;
  const states = ["norm", "stop", "test", "idle", "exit"];

  while ((chunk = process.stdin.read()) != null) {
    let trimmedInput = chunk.trim();

    if (states.includes(trimmedInput)) {
      process.stdout.write(`${state} -> ${trimmedInput}\n`);

      if (trimmedInput === "exit") {
        process.exit(0);
      } else {
        state = trimmedInput;
      }
    } else {
      process.stdout.write(`Неверное состояние: ${trimmedInput}\n`);
    }
  }
});
