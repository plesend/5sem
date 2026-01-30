const sql = require('mssql');

const dbconfig = {
    server: 'KVASIK',
    database: 'pskp',
    user: 'kvason',
    password: 'Kate2006',
    pool: {max: 10, min: 2},
    port: 58948,
    options: {
        trustServerCertificate: true,
        enableArithAbort: true
    }
};

class DB {
    pool = new sql.ConnectionPool(dbconfig).connect()
        .then(pool => {
            console.log('connected');
            return pool;
        })
        .catch(err => {
            console.log('connection failed', err.message)
        });
    //гет
    getFaculties = () => {
        return this.pool.then(pool => pool.request().query('select * from FACULTY')); 
    }
    getPulpits = () => {
        return this.pool.then(pool => pool.request().query('select * from PULPIT'));
    }
    getSubjects = () => {
        return this.pool.then(pool => pool.request().query('select * from SUBJECT'));
    }
    getAuditoriumTypes = () => {
        return this.pool.then(pool => pool.request().query('select * from AUDITORIUM_TYPE'));
    }
    getAuditoriums = () => {
        return this.pool.then(pool => pool.request().query('select * from AUDITORIUM'));
    }
    //пост добавить
    insertFaculty = async (faculty, facultyName) => {
        try {
            const pool = await this.pool;
            return await pool.request()
                .input('faculty', sql.NVarChar(100), faculty)
                .input('facultyName', sql.NVarChar(500), facultyName)
                .query('INSERT INTO FACULTY (FACULTY, FACULTY_NAME) VALUES (@faculty, @facultyName)');
        } catch (err) {
            return { error: err.message };
        }
    }
    insertPulpit = async (pulpit, pulpitName, faculty) => {
        try {
            const pool = await this.pool;
            return await pool.request()
                .input('pulpit', sql.NVarChar(200), pulpit)
                .input('pulpitName', sql.NVarChar(1000), pulpitName)
                .input('faculty', sql.NVarChar(100), faculty)
                .query('INSERT INTO PULPIT (PULPIT, PULPIT_NAME, FACULTY) VALUES (@pulpit, @pulpitName, @faculty)');
        } catch (err) {
            return { error: err.message };
        }
    }

    insertSubject = async (subject, subjectName, pulpit) => {
        try {
            const pool = await this.pool;
            return await pool.request()
                .input('subject', sql.NVarChar(100), subject)
                .input('subjectName', sql.NVarChar(1000), subjectName)
                .input('pulpit', sql.NVarChar(200), pulpit)
                .query('INSERT INTO SUBJECT (SUBJECT, SUBJECT_NAME, PULPIT) VALUES (@subject, @subjectName, @pulpit)');
        } catch (err) {
            return { error: err.message };
        }
    }
    insertAuditoriumType = async (audType, audTypeName) => {
        try {
            const pool = await this.pool;
            return await pool.request()
                .input('audType', sql.NVarChar(100), audType)
                .input('audTypeName', sql.NVarChar(100), audTypeName)
                .query('INSERT INTO AUDITORIUM_TYPE (AUDITORIUM_TYPE, AUDITORIUM_TYPENAME) VALUES (@audType, @audTypeName)');
        } catch (err) {
            return { error: err.message };
        }
    }
    insertAuditorium = async (auditorium, auditoriumName, audType, capacity) => {
        try {
            const pool = await this.pool;
            return await pool.request()
                .input('auditorium', sql.NVarChar(200), auditorium)
                .input('auditoriumName', sql.NVarChar(500), auditoriumName)
                .input('audType', sql.NVarChar(100), audType)
                .input('capacity', sql.Int, capacity)
                .query('INSERT INTO AUDITORIUM (AUDITORIUM, AUDITORIUM_NAME, AUDITORIUM_TYPE, AUDITORIUM_CAPACITY) VALUES (@auditorium, @auditoriumName, @audType, @capacity)');
        } catch (err) {
            return { error: err.message };
        }
    }

    //пут 
    updateFaculty = async (faculty, facultyName) => {
        const pool = await this.pool;
        let facultiesSQL = await this.getFaculties();
        let faculties = facultiesSQL.recordset;
        let isFound = false;
        for(let fac of faculties) {
            if(fac.FACULTY.trim() === faculty) {
                isFound = true;
                break;
            }
        }
        if (!isFound) throw new Error('faculty not found');
        return await pool.request()
            .input('faculty', sql.NChar, faculty)
            .input('facultyName', sql.NVarChar, facultyName)
            .query('update faculty set faculty_name = @facultyName where faculty = @faculty');
    }

    updatePulpit = async (pulpit, pulpitName, faculty) => {
        const pool = await this.pool;
        let pulpitsSQL = await this.getPulpits();
        let pulpits = pulpitsSQL.recordset;
        let isFound = pulpits.some(p => p.PULPIT.trim() === pulpit);
        if (!isFound) throw new Error('pulpit not found');
        return await pool.request()
            .input('pulpit', sql.NVarChar(200), pulpit)
            .input('pulpitName', sql.NVarChar(1000), pulpitName)
            .input('faculty', sql.NVarChar(100), faculty)
            .query('update PULPIT set PULPIT_NAME = @pulpitName, FACULTY = @faculty where PULPIT = @pulpit');
    }

    updateSubject = async (subject, subjectName, pulpit) => {
        const pool = await this.pool;
        let subjectsSQL = await this.getSubjects();
        let subjects = subjectsSQL.recordset;
        let isFound = subjects.some(s => s.SUBJECT.trim() === subject);
        if (!isFound) throw new Error('subject not found');
        return await pool.request()
            .input('subject', sql.NVarChar(100), subject)
            .input('subjectName', sql.NVarChar(1000), subjectName)
            .input('pulpit', sql.NVarChar(200), pulpit)
            .query('update SUBJECT set SUBJECT_NAME = @subjectName, PULPIT = @pulpit where SUBJECT = @subject');
    }

    updateAuditoriumType = async (auditoriumType, typeName) => {
        const pool = await this.pool;
        let typesSQL = await this.getAuditoriumTypes();
        let types = typesSQL.recordset;
        let isFound = types.some(t => t.AUDITORIUM_TYPE.trim() === auditoriumType);
        if (!isFound) throw new Error('auditorium type not found');
        return await pool.request()
            .input('auditoriumType', sql.NVarChar(100), auditoriumType)
            .input('typeName', sql.NVarChar(100), typeName)
            .query('update AUDITORIUM_TYPE set AUDITORIUM_TYPENAME = @typeName where AUDITORIUM_TYPE = @auditoriumType');
    }

    updateAuditorium = async (auditorium, auditoriumName, auditoriumType, capacity) => {
        const pool = await this.pool;
        let auditoriumsSQL = await this.getAuditoriums();
        let auditoriums = auditoriumsSQL.recordset;
        let isFound = auditoriums.some(a => a.AUDITORIUM.trim() === auditorium);
        if (!isFound) throw new Error('auditorium not found');
        return await pool.request()
            .input('auditorium', sql.NVarChar(200), auditorium)
            .input('auditoriumName', sql.NVarChar(500), auditoriumName)
            .input('auditoriumType', sql.NVarChar(100), auditoriumType)
            .input('capacity', sql.Int, capacity)
            .query(`update AUDITORIUM set AUDITORIUM_NAME = @auditoriumName, AUDITORIUM_TYPE = @auditoriumType, AUDITORIUM_CAPACITY = @capacity
                    where AUDITORIUM = @auditorium`);
    }

    //делете
    deleteFaculty = async (faculty) => {
        let faculties = (await this.getFaculties()).recordset;
        let isFound = faculties.some(fac => fac.FACULTY.trim() === faculty.trim());
        if (!isFound) throw new Error('faculty not found');
        return await this.pool.then(pool => 
            pool.request()
                .input('faculty', sql.NVarChar(100), faculty)
                .query('delete from FACULTY where FACULTY = @faculty')
        );
    }

    deletePulpit = async (pulpit) => {
        let pulpits = (await this.getPulpits()).recordset;
        let isFound = pulpits.some(p => p.PULPIT.trim() === pulpit.trim());
        if (!isFound) throw new Error('pulpit not found');
        return await this.pool.then(pool => 
            pool.request()
                .input('pulpit', sql.NVarChar(200), pulpit)
                .query('delete from PULPIT where PULPIT = @pulpit')
        );
    }
    
    deleteSubject = async (subject) => {
        let subjects = (await this.getSubjects()).recordset;
        let isFound = subjects.some(s => s.SUBJECT.trim() === subject.trim());
        if (!isFound) throw new Error('subject not found');
        return await this.pool.then(pool => 
            pool.request()
                .input('subject', sql.NVarChar(100), subject)
                .query('delete from SUBJECT where SUBJECT = @subject')
        );
    }

    deleteAuditoriumType = async (auditoriumType) => {
        let types = (await this.getAuditoriumTypes()).recordset;
        let isFound = types.some(t => t.AUDITORIUM_TYPE.trim() === auditoriumType.trim());
        if (!isFound) throw new Error('auditorium type not found');
        return await this.pool.then(pool => 
            pool.request()
                .input('auditoriumType', sql.NVarChar(100), auditoriumType)
                .query('delete from AUDITORIUM_TYPE where AUDITORIUM_TYPE = @auditoriumType')
        );
    }

    deleteAuditorium = async (auditorium) => {
        let auditoriums = (await this.getAuditoriums()).recordset;
        let isFound = auditoriums.some(a => a.AUDITORIUM.trim() === auditorium.trim());
        if (!isFound) throw new Error('auditorium not found');
        return await this.pool.then(pool => 
            pool.request()
                .input('auditorium', sql.NVarChar(200), auditorium)
                .query('delete from AUDITORIUM where AUDITORIUM = @auditorium')
        );
    }
}

exports.DB = DB;