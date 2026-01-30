const http = require('http');
const url = require('url');
const { Service } = require('./service'); 
const PORT = 3000;

const service = new Service();

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    const pathname = parsedUrl.pathname;

    if (req.method === 'GET' && pathname === '/') {
        return service.getFile(req, res);
    }

    if (req.method === 'GET') {
        switch (pathname) {
            case '/api/faculties': return service.getHandler(req, res, service.faculties.get);
            case '/api/pulpits': return service.getHandler(req, res, service.pulpits.get);
            case '/api/subjects': return service.getHandler(req, res, service.subjects.get);
            case '/api/auditoriumstypes': return service.getHandler(req, res, service.auditoriumTypes.get);
            case '/api/auditoriums': return service.getHandler(req, res, service.auditoriums.get);
        }
    }

    if (req.method === 'POST') {
        switch (pathname) {
            case '/api/faculties': return service.jsonHandler(req, res, service.faculties.insert, ['FACULTY','FACULTY_NAME']);
            case '/api/pulpits': return service.jsonHandler(req, res, service.pulpits.insert, ['PULPIT','PULPIT_NAME','FACULTY']);
            case '/api/subjects': return service.jsonHandler(req, res, service.subjects.insert, ['SUBJECT','SUBJECT_NAME','PULPIT']);
            case '/api/auditoriumstypes': return service.jsonHandler(req, res, service.auditoriumTypes.insert, ['AUDITORIUM_TYPE','AUDITORIUM_TYPENAME']);
            case '/api/auditoriums': return service.jsonHandler(req, res, service.auditoriums.insert, ['AUDITORIUM','AUDITORIUM_NAME','AUDITORIUM_TYPE','AUDITORIUM_CAPACITY']);
        }
    }

    if (req.method === 'PUT') {
        switch (pathname) {
            case '/api/faculties': return service.jsonHandler(req, res, service.faculties.update, ['FACULTY','FACULTY_NAME']);
            case '/api/pulpits': return service.jsonHandler(req, res, service.pulpits.update, ['PULPIT','PULPIT_NAME','FACULTY']);
            case '/api/subjects': return service.jsonHandler(req, res, service.subjects.update, ['SUBJECT','SUBJECT_NAME','PULPIT']);
            case '/api/auditoriumstypes': return service.jsonHandler(req, res, service.auditoriumTypes.update, ['AUDITORIUM_TYPE','AUDITORIUM_TYPENAME']);
            case '/api/auditoriums': return service.jsonHandler(req, res, service.auditoriums.update, ['AUDITORIUM','AUDITORIUM_NAME','AUDITORIUM_TYPE','AUDITORIUM_CAPACITY']);
        }
    }

    if (req.method === 'DELETE') {
        const parts = pathname.split('/');
        if (parts.length === 4 && parts[1] === 'api') {
            const entity = parts[2];
            const code = decodeURIComponent(parts[3]);

            switch(entity) {
                case 'faculties': return service.deleteHandler(req, res, service.faculties.delete, code);
                case 'pulpits': return service.deleteHandler(req, res, service.pulpits.delete, code);
                case 'subjects': return service.deleteHandler(req, res, service.subjects.delete, code);
                case 'auditoriumstypes': return service.deleteHandler(req, res, service.auditoriumTypes.delete, code);
                case 'auditoriums': return service.deleteHandler(req, res, service.auditoriums.delete, code);
            }
        }
    }

    res.writeHead(404, {'Content-Type':'application/json; charset=utf-8'});
    res.end(JSON.stringify({ error: 'Not found' }, null, 4));
});

server.listen(PORT, () => {
    console.log(`Server listening on http://localhost:${PORT}`);
});
