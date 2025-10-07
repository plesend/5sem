const http = require('http');
const url = require("url");
const { performance } = require('perf_hooks');

function CountFactorialAsync(k, callback) {
    if (k < 0 || isNaN(k)) {
        return process.nextTick(() => callback(new Error("k is Invalid")));
    }

    function factorial(n, acc = 1) {
        if (n <= 1) return acc;
        return factorial(n - 1, acc * n);
    }
    process.nextTick(() => {
        try {
            const result = factorial(k);
            callback(null, result);
        } catch (err) {
            callback(err);
        }
    });
}


const server = http.createServer(function(req, res) {

    let parsedUrl = url.parse(req.url, true);

    if (parsedUrl.pathname === "/") {
        res.setHeader("Content-Type", "text/html; charset=utf-8");
        res.statusCode = 200;

        res.end(`<!DOCTYPE html>
            <html lang="ru">
            <head>
                <meta charset="UTF-8">
                <title>Факториалы</title>
            </head>
            <body>
                <h1>Факториалы чисел от 1 до 20</h1>
                <div id="results"></div>

                <script>
                    const resultsDiv = document.getElementById('results');

                    async function loadFactorials() {
                        for (let x = 1; x <= 20; x++) {
                            try {
                                const response = await fetch(\`/fact?k=\${x}\`);
                                const data = await response.json();
                                const p = document.createElement('p');
                                p.className = 'result';
                                p.textContent = \`Результат: \${data.time_ms}-\${data.k}/\${data.factorial}\`;
                                resultsDiv.appendChild(p);
                            } catch (err) {
                                const p = document.createElement('p');
                                p.textContent = \`Ошибка при загрузке x=\${x}\`;
                                resultsDiv.appendChild(p);
                            }
                        }
                    }

                    loadFactorials();
                </script>
            </body>
            </html>`)
    }

    else if (parsedUrl.pathname === "/fact") {

        let k = parseInt(parsedUrl.query.k);

        res.setHeader("Content-Type", "application/json; charset=utf-8");
        res.statusCode = 200;

        const start = performance.now();
        CountFactorialAsync(k, (err, fact)=> {
            const end = performance.now();

            if (err) {
                res.statusCode = 400;
                return res.end(JSON.stringify({ error: err.message }));
            }
            
            const result = {
                k: k,
                factorial: fact,
                time_ms: Math.round((end - start) * 1000)
            };

            res.end(JSON.stringify(result));
        });

    } else {
        res.statusCode = 404;
        res.end(JSON.stringify({ error: "Not Found" }));
    }
});

server.listen(5000, function() {
    console.log("Сервер запускается! ");
});