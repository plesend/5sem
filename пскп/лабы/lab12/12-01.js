const http = require('http');
const url = require('url');
const fs = require('fs');
const path = require('path');
const WebSocket = require('rpc-websockets').Server;

const wss = new WebSocket({
    port : 4000,
    host: 'localhost',
});

wss.event('postbackup');
wss.event('deletebackup');
wss.register('subscribe', () => true);

function error(res) {
    console.log('cannot read/write file');
    res.writeHead(500, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ error: 1, message: 'ошибка чтения/записи файла' }));
}

function errorId(res, err, id) {
    if (err === 2) {
        console.log('cannot find student id');
        res.writeHead(404, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ error: 2, message: `студент с id ${id} не найден` }));
    }
    else if (err === 3) {
        console.log('student id exists');
        res.writeHead(409, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ error: 3, message: `студент с id ${id} уже есть` }));
    }
}

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    const method = req.method;

    switch (method) {
        case 'GET': {
            if (req.url === '/') {
                fs.readFile('StudentList.json', 'utf8', (err, data) => {
                    if (err) return error(res);

                    res.writeHead(200, { 'Content-Type': 'application/json' });
                    res.end(data);
                });
                return;
            }

            const parts = req.url.split('/').filter(p => p !== '');
            if (parts[0] === 'backup' && parts.length === 1) {
                fs.readdir('.', (err, files) => {
                    if (err) return error(res);

                    const backups = files.filter(f => /^\d{14}_StudentList\.json$/.test(f));

                    res.writeHead(200, { 'Content-Type': 'application/json' });
                    res.end(JSON.stringify(backups));
                });
            return;
            }

            if (parts.length === 1 && !isNaN(parts[0])) {
                const id = Number(parts[0]);

                fs.readFile('StudentList.json', 'utf8', (err, data) => {
                    if (err) return error(res);

                    const students = JSON.parse(data);
                    const student = students.find(s => s.id === id);

                    if (!student) return errorId(res, 2, id);

                    res.writeHead(200, { 'Content-Type': 'application/json' });
                    res.end(JSON.stringify(student));
                });
            }



            break;
        }

        case 'POST': {
            if (req.url === '/') {
                let body = '';

                req.on('data', chunk => body += chunk);
                req.on('end', () => {
                    let newStudent;

                    try {
                        newStudent = JSON.parse(body);
                    } catch (_) {
                        return res.writeHead(400, { 'Content-Type': 'application/json' })
                            .end(JSON.stringify({ message: 'невалидный JSON' }));
                    }

                    if (!newStudent.id || !newStudent.name || !newStudent.bday || !newStudent.speciality) {
                        return res.writeHead(400, { 'Content-Type': 'application/json' })
                            .end(JSON.stringify({ message: 'нехватает полей' }));
                    }

                    const dateRegex = /^\d{4}-\d{2}-\d{2}$/;
                    if (!dateRegex.test(newStudent.bday)) {
                        return res.writeHead(400, { 'Content-Type': 'application/json' })
                            .end(JSON.stringify({ message: 'неверный формат даты' }));
                    }

                    fs.readFile('StudentList.json', 'utf8', (err, data) => {
                        if (err) return error(res);

                        const students = JSON.parse(data);

                        const existing = students.find(s => s.id === newStudent.id);
                        if (existing) return errorId(res, 3, newStudent.id);

                        students.push(newStudent);
                        students.sort((a, b) => a.id - b.id);

                        fs.writeFile('StudentList.json', JSON.stringify(students, null, 2), 'utf8', (err) => {
                            if (err) return error(res);

                            res.writeHead(201, { 'Content-Type': 'application/json' });
                            res.end(JSON.stringify(newStudent));
                        });
                    });
                });
            } else if(req.url === '/backup') {
                const now = new Date();
                const timestamp =
                    now.getFullYear().toString() +
                    String(now.getMonth() + 1).padStart(2, '0') +
                    String(now.getDate()).padStart(2, '0') +
                    String(now.getHours()).padStart(2, '0') +
                    String(now.getMinutes()).padStart(2, '0') +
                    String(now.getSeconds()).padStart(2, '0');

                const backupName = `${timestamp}_StudentList.json`;
                setTimeout(() => {
                    fs.copyFile('StudentList.json', backupName, (err) => {
                        if (err) return error(res);

                        wss.emit('postbackup', {
                            message: 'Создан новый бекап',
                        });

                        res.writeHead(200, { 'Content-Type': 'application/json' });
                        res.end(JSON.stringify({
                            message: 'backup created',
                            file: backupName
                        }));
                    });
                }, 2000);
                return;
            }

            break;
        }
        case 'PUT': {
            if (req.url === '/') {
                let body = '';
                req.on('data', chunk => {
                    body += chunk;
                });

                req.on('end', () => {
                    try {
                    putStudent = JSON.parse(body);
                } catch (e) {
                    return res.writeHead(400, { 'Content-Type': 'application/json' })
                    .end(JSON.stringify({ message: 'Невалидный JSON' }));
                }   
                if (!putStudent.id) {
                    return errorId(res, 1, putStudent.id);
                }

                fs.readFile('StudentList.json', 'utf8', (err, data) => {
                    if(err) return error(res);
                    const students = JSON.parse(data);
                    const ind = students.findIndex(s => s.id === putStudent.id);
                    if (ind === -1) {
                        return errorId(res, 2, putStudent.id);
                    }
                    students[ind] = putStudent;
                    fs.writeFile('StudentList.json', JSON.stringify(students, null, 2), 'utf8', err => {
                        if (err) return err(res);

                        res.writeHead(200, { 'Content-Type': 'application/json' });
                        res.end(JSON.stringify(putStudent));
                    })
                });
                })
                

            }
            break;
        }
        case 'DELETE':{
            const parts = req.url.split('/').filter(p => p !== '');

                if (parts[0] === 'backup' && parts.length === 2) {
                    const dateStr = parts[1];

                    if (!/^\d{8}$/.test(dateStr)) {
                        res.writeHead(400, { 'Content-Type': 'application/json' });
                        return res.end(JSON.stringify({ message: 'неверный формат даты (нужно yyyyddmm)' }));
                    }

                    const threshold = Number(dateStr);

                    fs.readdir('.', (err, files) => {
                        const backups = files.filter(f => /^\d{14}_StudentList\.json$/.test(f));
                        const toDelete = backups.filter(name => {
                            const fileDate = Number(name.substring(0, 8));
                            return fileDate < threshold;
                        });

                        if(toDelete.length === 0) {
                            res.writeHead(200, { 'Content-Type': 'application/json' });
                            return res.end(JSON.stringify({ message: 'нет файлов для удаления' }));
                        }

                        let deleted =[];

                        toDelete.forEach((file, ind) => {
                            fs.unlink(file, err => {
                                if(!err) deleted.push(file);

                                wss.emit('deletebackup', {
                                    message: 'бэкапы удалены',
                                    files: deleted
                                })

                                if (ind === toDelete.length -1) {
                                    res.writeHead(200, { 'Content-Type': 'application/json' });
                                    res.end(JSON.stringify({
                                        message: 'удалено файлов',
                                        count: deleted.length,
                                        files: deleted
                                    }));
                                }
                            });
                        })
                    });
                    return;
                }


            if (req.url.startsWith('/')) {
                const parts = req.url.split('/').filter(p => p !== '');
                if (parts.length === 1 && !isNaN(parts[0])) {
                    const id = Number(parts[0]);

                fs.readFile('StudentList.json', 'utf8', (err, data) => {
                    if (err) return error(res);

                    const students = JSON.parse(data);
                    const exists = students.some(s => s.id === id);
                    if (!exists) return errorId(res, 2, id);

                    const deletedStudent = students.find(s => s.id === id);
                    const updStudents = students.filter(s => s.id !== id);

                    fs.writeFile('StudentList.json', JSON.stringify(updStudents, null, 2), 'utf8', (err) => {
                        if(err) return error(res);

                        res.writeHead(200, { 'Content-Type': 'application/json' });
                        res.end(JSON.stringify(deletedStudent));
                    })
                });
            }
            }
            break;
        }
    }
});
const port = 3000;
server.listen(port, () => {
    console.log(`СePBeP 3anylll,eH Ha http://localhost:${port}`);
});
