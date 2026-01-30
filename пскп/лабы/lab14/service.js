const {DB} = require('./dbmod');
const fs = require('fs');

class Service {
    constructor() {
        this.db = new DB();
    }
    getFile = (req, res) => {
        res.writeHead(200, {'Content-Type': 'text/html; charset=utf-8'});
        res.end(fs.readFileSync('./index.html'));
    }

    jsonHandler = (req, res, dbFunction, requiredFields = []) => {
        let json = '';
        req.on('data', chunk => json += chunk);
        req.on('end', () => {
            try { json = JSON.parse(json); } 
            catch { return this.errorHandler(res, 422, 'Invalid JSON'); }

            for (let f of requiredFields) {
                if (!json[f] || json[f].toString().trim() === '') {
                    return this.errorHandler(res, 422, `Missing or empty field: ${f}`);
                }
            }

            dbFunction(...requiredFields.map(f => json[f]))
                .then(() => res.end(JSON.stringify(json, null, 4)))
                .catch(err => this.errorHandler(res, 422, err.message));
        });
    }

    getHandler = (req, res, dbFunction, param = null) => {
        dbFunction(param)
            .then(records => res.end(JSON.stringify(records.recordset, null, 4)))
            .catch(err => this.errorHandler(res, 422, err.message));
    }

    deleteHandler = (req, res, dbFunction, param) => {
        dbFunction(param)
            .then(() => res.end(JSON.stringify({ key: param }, null, 4)))
            .catch(err => this.errorHandler(res, 422, err.message));
    }

    errorHandler = (res, code, msg) => {
        res.writeHead(code, {'Content-Type':'application/json; charset=utf-8'});
        res.end(JSON.stringify({ errorCode: code, errorMsg: msg }, null, 4));
    }

    faculties = {
        get: () => this.db.getFaculties(),
        insert: (FACULTY, FACULTY_NAME) => this.db.insertFaculty(FACULTY, FACULTY_NAME),
        update: (FACULTY, FACULTY_NAME) => this.db.updateFaculty(FACULTY, FACULTY_NAME),
        delete: (FACULTY) => this.db.deleteFaculty(FACULTY)
    }

    pulpits = {
        get: () => this.db.getPulpits(),
        insert: (PULPIT, PULPIT_NAME, FACULTY) => this.db.insertPulpit(PULPIT, PULPIT_NAME, FACULTY),
        update: (PULPIT, PULPIT_NAME, FACULTY) => this.db.updatePulpit(PULPIT, PULPIT_NAME, FACULTY),
        delete: (PULPIT) => this.db.deletePulpit(PULPIT)
    }

    subjects = {
        get: () => this.db.getSubjects(),
        insert: (SUBJECT, SUBJECT_NAME, PULPIT) => this.db.insertSubject(SUBJECT, SUBJECT_NAME, PULPIT),
        update: (SUBJECT, SUBJECT_NAME, PULPIT) => this.db.updateSubject(SUBJECT, SUBJECT_NAME, PULPIT),
        delete: (SUBJECT) => this.db.deleteSubject(SUBJECT)
    }

    auditoriumTypes = {
        get: () => this.db.getAuditoriumTypes(),
        insert: (AUDITORIUM_TYPE, AUDITORIUM_TYPENAME) => this.db.insertAuditoriumType(AUDITORIUM_TYPE, AUDITORIUM_TYPENAME),
        update: (AUDITORIUM_TYPE, AUDITORIUM_TYPENAME) => this.db.updateAuditoriumType(AUDITORIUM_TYPE, AUDITORIUM_TYPENAME),
        delete: (AUDITORIUM_TYPE) => this.db.deleteAuditoriumType(AUDITORIUM_TYPE)
    }

    auditoriums = {
        get: () => this.db.getAuditoriums(),
        insert: (AUDITORIUM, AUDITORIUM_NAME, AUDITORIUM_CAPACITY, AUDITORIUM_TYPE) =>
            this.db.insertAuditorium(AUDITORIUM, AUDITORIUM_NAME, AUDITORIUM_CAPACITY, AUDITORIUM_TYPE),
        update: (AUDITORIUM, AUDITORIUM_NAME, AUDITORIUM_CAPACITY, AUDITORIUM_TYPE) =>
            this.db.updateAuditorium(AUDITORIUM, AUDITORIUM_NAME, AUDITORIUM_CAPACITY, AUDITORIUM_TYPE),
        delete: (AUDITORIUM) => this.db.deleteAuditorium(AUDITORIUM)
    }
}

exports.Service = Service;