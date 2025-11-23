// server.js
const http = require('http');
const url = require('url');
const qs = require('querystring');
const path = require('path');
const fs = require('fs');
const { XMLParser, XMLBuilder } = require('fast-xml-parser');
const formidable = require('formidable');

const staticDirectory = path.join(__dirname, 'static');
const extensions = ["html", "css", "js", "png", "docx", "json", "xml", "mp4"];
const mimeTypes = [
  "text/html",
  "text/css",
  "text/javascript",
  "image/png",
  "application/msword",
  "application/json",
  "application/xml",
  "video/mp4",
];

const parser = new XMLParser({ ignoreAttributes: false, attributeNamePrefix: "" });
const builder = new XMLBuilder({ ignoreAttributes: false, attributeNamePrefix: "" });

const server = http.createServer((req, res) => {
  const parsedUrl = url.parse(req.url, true);
  const pathname = parsedUrl.pathname || '/';
  const query = parsedUrl.query || {};

  // Роут для /files/<имя> обрабатываем отдельно (динамический)
  if (req.method === 'GET' && pathname.startsWith('/files/')) {
    const filename = decodeURIComponent(pathname.slice('/files/'.length));
    const filepath = path.join(staticDirectory, filename);

    fs.access(filepath, fs.constants.F_OK, (err) => {
      if (err) {
        res.statusCode = 404;
        res.setHeader('Content-Type', 'text/plain; charset=utf-8');
        res.end('file not found');
        return;
      }

      const extension = path.extname(filename).slice(1).toLowerCase();
      const idx = extensions.indexOf(extension);
      const mime = idx !== -1 ? mimeTypes[idx] : 'application/octet-stream';

      res.setHeader('Content-Type', mime);
      res.setHeader('Content-Disposition', `attachment; filename="${path.basename(filename)}"`);
      res.statusCode = 200;
      fs.createReadStream(filepath).pipe(res);
    });

    return;
  }

  if (pathname === '/parameter' || pathname.startsWith('/parameter/')) {
    let x, y;
    const parts = pathname.split('/'); 

    if (parts.length === 4) {
        x = parts[2];
        y = parts[3];
    }

    if (query.x !== undefined && query.y !== undefined) {
        x = query.x;
        y = query.y;
    }

    if (x === undefined || y === undefined) {
        res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Ошибка: параметры отсутствуют');
        return;
    }

    const nx = Number(x);
    const ny = Number(y);

    if (isNaN(nx) || isNaN(ny)) {
        if (pathname.startsWith('/parameter/')) {
            res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end(`URI: ${req.url}`);
        } else {
            res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end('Ошибка: x или y не число');
        }
        return;
    }

    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end(
        `sum: ${nx + ny}\n` +
        `sub: ${nx - ny}\n` +
        `mult: ${nx * ny}\n` +
        `div: ${ny !== 0 ? nx / ny : '∞'}\n`
    );
    return;
}

  if (req.method === 'GET') {
    switch (pathname) {
      case '/': {
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end('<h1>Index page</h1><p>Server is running</p>');
        break;
      }

      case '/connection': {
        if (query.set) {
          const newValue = Number(query.set);
          if (!isNaN(newValue)) {
            server.keepAliveTimeout = newValue;
            res.writeHead(200, { 'Content-Type': 'text/plain;charset=utf-8' });
            res.end(`Установлено новое значение параметра KeepAliveTimeout=${newValue}`);
          } else {
            res.writeHead(400, { 'Content-Type': 'text/plain;charset=utf-8' });
            res.end(`Ошибка: значение set должно быть числом`);
          }
        } else {
          res.writeHead(200, { 'Content-Type': 'text/plain;charset=utf-8' });
          res.end(`Текущее значение KeepAliveTimeout=${server.keepAliveTimeout}`);
        }
        break;
      }

      case '/headers': {
        res.setHeader('X-Katusha-Header', 'I like Limp Bizkit');
        const reqHeaders = JSON.stringify(req.headers, null, 4);
        const resHeaders = JSON.stringify(res.getHeaders());

        res.writeHead(200, { 'Content-Type': 'text/plain;charset=utf-8' });
        res.end(`Все заголовки req: ${reqHeaders}\nВсе заголовки res: ${resHeaders}`);
        break;
      }

      case '/close': {
        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end("server shutdown in 10 seconds");
        setTimeout(() => process.exit(0), 10000);
        break;
      }

      case '/socket': {
        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(
          `client ip: ${req.socket.remoteAddress}, client port: ${req.socket.remotePort}, server ip: ${req.socket.localAddress}, server port: ${req.socket.localPort}`
        );
        break;
      }

      case '/req-data': {
        let body = '';
        req.on('data', chunk => { console.log("получен текст"); body += chunk; });
        req.on('end', () => {
          res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
          res.end(`Данные получены:\n${body}`);
        });
        break;
      }

      case '/resp-status': {
        const code = Number(query.code);
        const mess = query.mess || '';
        if (isNaN(code)) {
          res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
          res.end('Ошибка: параметр code должен быть числом');
          break;
        }

        res.writeHead(code, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(`Сообщение к статусу ${code}: ${mess}`);
        break;
      }


    case '/formparameter': {
        if (Object.keys(query).length === 0) {
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end(`
          <form method="GET" action="/formparameter">
            <label>Text: <input type="text" name="text"></label><br>
            <label>Number: <input type="number" name="num"></label><br>
            <label>Date: <input type="date" name="date"></label><br>
            <label>Checkbox: <input type="checkbox" name="check" value="yes"></label><br>
            <label>Radio 1: <input type="radio" name="radio" value="A"></label>
            <label>Radio 2: <input type="radio" name="radio" value="B"></label><br>
            <label>Textarea:<br><textarea name="textar"></textarea></label><br>
            <input type="submit" name="submit" value="Send1">
            <input type="submit" name="submit" value="Send2">
          </form>
        `);
        } else {
            res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
            res.write('<h1>params: </h1><br/>');
            for (const key in query) {
                const value = query[key];
                if (Array.isArray(value)) {
                    res.write(`<p>${key}: ${value.join(', ')}</p>`);
                } else {
                    res.write(`<p>${key}: ${value}</p>`);
                }
            }
            res.end();
        }
        break;
    }
      
      case '/files': {
        fs.readdir(staticDirectory, (err, files) => {
          if (err) {
            res.statusCode = 500;
            res.end('cant read static directory');
            return;
          }
          res.statusCode = 200;
          res.setHeader('Content-Type', 'text/plain; charset=utf-8');
          res.setHeader('X-static-files-count', files.length.toString());
          res.end(files.join('\n'));
        });
        break;
      }

      case '/upload': {
        // отдаём простую страницу загрузки (файл upload.html рядом с server.js или можно встроить html)
        const uploadHtmlPath = path.join(__dirname, 'upload.html');
        fs.access(uploadHtmlPath, fs.constants.F_OK, (err) => {
          if (err) {
            // если файла нет — вернуть встроенную форму
            res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
            res.end(`
              <form method="POST" action="/upload" enctype="multipart/form-data">
                <input type="file" name="uploadedFile"><br>
                <button type="submit">Upload</button>
              </form>
            `);
            return;
          }
          res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
          fs.createReadStream(uploadHtmlPath).pipe(res);
        });
        break;
      }

      default: {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Not found');
        break;
      }
    } 
    return;
  } 

  if (req.method === 'POST') {
    if (pathname === '/formparameter') {
      let body = '';
      req.on('data', chunk => {
        body += chunk;
      })

      req.on('end', ()=> {
        const params = qs.parse(body);
        console.log(params);
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.write('<h1>params: </h1><br/>');
        for (const key in params) {
            const value = params[key];
            if (Array.isArray(value)) {
                res.write(`<p>${key}: ${value.join(', ')}</p>`);
            } else {
                res.write(`<p>${key}: ${value}</p>`);
            }
        }
        res.end();
      })
    }

    if (pathname === '/json') {
      let body = '';
      req.on('data', chunk => { body += chunk; });
      req.on('end', () => {
        try {
          const data = JSON.parse(body);
          let resParts = data.__comment.split('.');
          let resPartsAfterDot = resParts[1];
          const response = {
            __comment: ` Ответ.${resPartsAfterDot}`,
            x_plus_y: data.x + data.y,
            Concatination_s_o: `${data.s}: ${data.o.surname}, ${data.o.name}`,
            Length_m: data.m.length
          };
          res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
          res.end(JSON.stringify(response, null, 2));
        } catch (e) {
          res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
          res.end('JSON parse error: ' + e.message);
        }
      });
      return;
    }

    if (pathname === '/xml') {
      let body = '';
      req.on('data', chunk => body += chunk.toString());
      req.on('end', () => {
        try {
          const parsedBody = parser.parse(body);

          if (!parsedBody.request) {
            res.statusCode = 400;
            res.setHeader('Content-Type', 'text/plain; charset=utf-8');
            res.end('xml error: no <request>');
            return;
          }

          const reqId = Number(parsedBody.request.id || 0);

          let arrX = parsedBody.request.x || [];
          let arrM = parsedBody.request.m || [];

          if (!Array.isArray(arrX)) arrX = [arrX];
          if (!Array.isArray(arrM)) arrM = [arrM];

          const sumX = arrX.reduce((acc, el) => acc + Number(el.value || 0), 0);
          const concatM = arrM.map(el => el.value || '').join('');

          const responseObj = {
            response: {
              id: (reqId + 5).toString(),
              request: reqId.toString(),
              sum: { element: "x", result: sumX.toString() },
              concat: { element: "m", result: concatM }
            }
          };

          const xmlResponse = builder.build(responseObj);

          res.statusCode = 200;
          res.setHeader('Content-Type', 'application/xml; charset=utf-8');
          res.end(xmlResponse);
        } catch (err) {
          res.statusCode = 400;
          res.setHeader('Content-Type', 'text/plain; charset=utf-8');
          res.end('xml error: ' + err.message);
        }
      });
      return;
    }

    if (pathname === '/upload') {
      const form = new formidable.IncomingForm({ multiples: false, uploadDir: staticDirectory, keepExtensions: true });
      form.parse(req, (err, fields, files) => {
        if (err) {
          res.statusCode = 400;
          res.end('file upload error: ' + err.message);
          return;
        }

        const uploaded = files.uploadedFile;
        if (!uploaded) {
          res.statusCode = 400;
          res.end('file wasnt uploaded');
          return;
        }

        const fileObj = Array.isArray(uploaded) ? uploaded[0] : uploaded;
        const newFilePath = path.join(staticDirectory, fileObj.originalFilename || path.basename(fileObj.filepath));

        fs.rename(fileObj.filepath, newFilePath, (err) => {
          if (err) {
            res.statusCode = 500;
            res.end('file rename error: ' + err.message);
            return;
          }
          res.setHeader('Content-Type', 'text/plain; charset=utf-8');
          res.statusCode = 200;
          res.end('file uploaded successfully');
        });
      });
      return;
    }

    res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end('Not found');
    return;
  } 

  res.writeHead(405, { 'Content-Type': 'text/plain; charset=utf-8' });
  res.end('Method not allowed');
});

const PORT = 5000;
server.listen(PORT, () => console.log(`Server listening on http://localhost:${PORT}`));
