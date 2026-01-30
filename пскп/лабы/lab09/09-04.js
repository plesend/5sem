const http = require('http');

const payload = {
  "__commment": "abc",
  "x": "1",
  "y": "2",
  "s": "message",
  "m": ["a", "b"],
  "o": { "surname": "Иванов", "name": "Иван" }
};

const data = JSON.stringify(payload);

const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/json',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
  }
};

const req = http.request(options, res => {
  console.log('status:', res.statusCode);

  let body = ''; 

  res.on('data', chunk => {
    body += chunk;  
  });

  res.on('end', () => {
    console.log('body:', body);

    try {
      const obj = JSON.parse(body);
      console.log('parsed JSON response:', obj);
    } catch (e) {
      console.log('JSON parse error:', e.message);
    }
  });
});

req.on('error', console.error);

req.write(data);
req.end();
