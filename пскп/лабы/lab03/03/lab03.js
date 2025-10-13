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

    let parsedUrl = url.parse(req.url, true);

    if (parsedUrl.pathname === "/") {
        res.setHeader("Content-Type", "text/html; charset=utf-8");
        res.statusCode = 200;

        res.end(`<h1>Факториалы чисел от 1 до 20</h1>
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
                </script>`)
    }

    else if (parsedUrl.pathname === "/fact") {

        let k = parseInt(parsedUrl.query.k);

        res.setHeader("Content-Type", "application/json; charset=utf-8");
        res.statusCode = 200;

        try {
            const start = performance.now();
            const fact = CountFactorial(k);
            const end = performance.now();

            const result = {
                k: k,
                factorial: fact,
                time_ms: (end - start).toFixed(4) * 10000
            };

            res.end(JSON.stringify(result));
        } catch (error) {
            res.statusCode = 400;
            res.end(JSON.stringify({ error: error.message }));
        }
    } else {
        res.statusCode = 404;
        res.end(JSON.stringify({ error: "Not Found" }));
    }
});

server.listen(5000, function() {
    console.log("Сервер запускается! ");
});