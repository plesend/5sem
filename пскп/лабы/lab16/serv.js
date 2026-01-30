const { buildSchema, graphql } = require('graphql');
const sql = require('mssql');
const http = require('http');

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

const poolPromise = new sql.ConnectionPool(dbconfig)
    .connect()
    .then(pool => {
        console.log('connected');
        return pool;
    })
    .catch(err => {
        console.error('connection failed:', err.message);
    });

const schema = buildSchema(`
    type Teacher {
        Teacher: String!
        TeacherName: String!
        Pulpit: String!
    }

    type Pulpit {
        Pulpit: String!
        PulpitName: String!
        Faculty: String!
    }

    type Faculty {
        Faculty: String!
        FacultyName: String!
    }

    type Subject {
        Subject: String!
        SubjectName: String!
        Pulpit: String!
    }

    #для джоинов

    type TEACHERS_BY_FACULTY_RES {
        Faculty: String!
        Teachers: [Teacher!]!
    }

    type SUBJECT_BY_FACULTY_RES {
        Faculty: String!
        Pulpit: String!
        PulpitName: String!
        Subjects: [Subject!]!
    }

    type Mutation {
        setFaculty(faculty: String!, faculty_name: String!): Faculty
        setPulpit(pulpit: String!, pulpit_name: String!, faculty: String!): Pulpit
        setTeacher(teacher: String!, teacher_name: String!, pulpit: String!): Teacher
        setSubject(subject: String!, subject_name: String!, pulpit: String!): Subject
        delFaculty(faculty: String!): Boolean
        delPulpit(pulpit: String!): Boolean
        delTeacher(teacher: String!): Boolean
        delSubject(subject: String!): Boolean
    }
    
    type Query {
        getFaculties(faculty: String): [Faculty]
        getPulpits(pulpit: String): [Pulpit]
        getTeachers(teacher: String): [Teacher]
        getSubjects(subject: String): [Subject]

        getTeachersByFaculty(faculty: String!): TEACHERS_BY_FACULTY_RES
        getSubjectsByFaculties(faculty: String!): [SUBJECT_BY_FACULTY_RES]!
    }

`);

const root = {
    getFaculties: async ({ faculty }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();

            if (faculty) {
                request.input('faculty', sql.NVarChar, faculty);
            }

            const query = faculty
                ? `SELECT FACULTY AS Faculty, FACULTY_NAME AS FacultyName
                   FROM FACULTY WHERE FACULTY = @faculty`
                : `SELECT FACULTY AS Faculty, FACULTY_NAME AS FacultyName
                   FROM FACULTY`;

            const result = await request.query(query);
            return result.recordset;
        } catch (err) {
            console.error(err);
            return [];
        }
    },

    getTeachers: async ({ teacher }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();

            if (teacher) request.input('teacher', sql.NVarChar, teacher);

            const query = teacher
                ? 'SELECT TEACHER AS Teacher, TEACHER_NAME AS TeacherName, PULPIT AS Pulpit FROM TEACHER WHERE TEACHER = @teacher'
                : 'SELECT TEACHER AS Teacher, TEACHER_NAME AS TeacherName, PULPIT AS Pulpit FROM TEACHER';

            const result = await request.query(query);
            return result.recordset;
        } catch (err) {
            console.error('PROBLEMO getTeachers:', err);
            return [];
        }
    },
 

    getPulpits: async ({ pulpit }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();

            if (pulpit) request.input('pulpit', sql.NVarChar, pulpit);

            const query = pulpit
                ? 'SELECT PULPIT AS Pulpit, PULPIT_NAME AS PulpitName, FACULTY AS Faculty FROM PULPIT WHERE PULPIT = @pulpit'
                : 'SELECT PULPIT AS Pulpit, PULPIT_NAME AS PulpitName, FACULTY AS Faculty FROM PULPIT';

            const result = await request.query(query);
            return result.recordset;
        } catch (err) {
            console.error('PROBLEMO getPulpits:', err);
            return [];
        }
    },

getSubjects: async ({ subject }) => {
    try {
        const pool = await poolPromise;
        const request = pool.request();

        if (subject) request.input('subject', sql.NVarChar, subject);

        const query = `
            SELECT 
                SUBJECT AS Subject, 
                SUBJECT_NAME AS SubjectName, 
                PULPIT AS Pulpit
            FROM SUBJECT
            ${subject ? 'WHERE SUBJECT = @subject' : ''}
        `;

        const result = await request.query(query);
        return result.recordset;
    } catch (err) {
        console.error('PROBLEMO getSubjects:', err);
        return [];
    }
},

    setFaculty: async ({ faculty, faculty_name }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();

            request.input('faculty', sql.NVarChar, faculty);
            request.input('faculty_name', sql.NVarChar, faculty_name);

            const query = 'IF EXISTS (SELECT 1 FROM FACULTY WHERE FACULTY = @faculty) UPDATE FACULTY SET FACULTY_NAME = @faculty_name WHERE FACULTY = @faculty ELSE INSERT INTO FACULTY(FACULTY, FACULTY_NAME) VALUES(@faculty, @faculty_name) SELECT FACULTY AS Faculty, FACULTY_NAME AS FacultyName FROM FACULTY WHERE FACULTY = @faculty';

            const result = await request.query(query);
            return result.recordset[0];
        } catch (err) {
            console.error('PROBLEMO setFaculty:', err);
            return null;
        }
    },

    setPulpit: async ({ pulpit, pulpit_name, faculty }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();

            request.input('pulpit', sql.NVarChar, pulpit);
            request.input('pulpit_name', sql.NVarChar, pulpit_name);
            request.input('faculty', sql.NVarChar, faculty);

            const query = 'IF EXISTS (SELECT 1 FROM PULPIT WHERE PULPIT = @pulpit) UPDATE PULPIT SET PULPIT_NAME = @pulpit_name, FACULTY = @faculty WHERE PULPIT = @pulpit ELSE INSERT INTO PULPIT(PULPIT, PULPIT_NAME, FACULTY) VALUES(@pulpit, @pulpit_name, @faculty) SELECT PULPIT AS Pulpit, PULPIT_NAME AS PulpitName, FACULTY AS Faculty FROM PULPIT WHERE PULPIT = @pulpit';

            const result = await request.query(query);
            return result.recordset[0];
        } catch (err) {
            console.error('PROBLEMO setPulpit:', err);
            return null;
        }
    },

    
    setTeacher: async ({ teacher, teacher_name, pulpit }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();
        
            request.input('teacher', sql.NVarChar, teacher);
            request.input('teacher_name', sql.NVarChar, teacher_name);
            request.input('pulpit', sql.NVarChar, pulpit);
        
            const query = 'IF EXISTS (SELECT 1 FROM TEACHER WHERE TEACHER = @teacher) UPDATE TEACHER SET TEACHER_NAME = @teacher_name, PULPIT = @pulpit WHERE TEACHER = @teacher ELSE INSERT INTO TEACHER(TEACHER, TEACHER_NAME, PULPIT) VALUES(@teacher, @teacher_name, @pulpit) SELECT TEACHER AS Teacher, TEACHER_NAME AS TeacherName, PULPIT AS Pulpit FROM TEACHER WHERE TEACHER = @teacher';
        
            const result = await request.query(query);
            return result.recordset[0];
        } catch (err) {
            console.error('PROBLEMO setTeacher:', err);
            return null;
        }
    },
    
    setSubject: async ({ subject, subject_name, pulpit }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();
        
            request.input('subject', sql.NVarChar, subject);
            request.input('subject_name', sql.NVarChar, subject_name);
            request.input('pulpit', sql.NVarChar, pulpit);
        
            const query = 'IF EXISTS (SELECT 1 FROM SUBJECT WHERE SUBJECT = @subject) UPDATE SUBJECT SET SUBJECT_NAME = @subject_name, PULPIT = @pulpit WHERE SUBJECT = @subject ELSE INSERT INTO SUBJECT(SUBJECT, SUBJECT_NAME, PULPIT) VALUES(@subject, @subject_name, @pulpit) SELECT SUBJECT AS Subject, SUBJECT_NAME AS SubjectName, PULPIT AS Pulpit FROM SUBJECT WHERE SUBJECT = @subject';
        
            const result = await request.query(query);
            return result.recordset[0];
        } catch (err) {
            console.error('PROBLEMO setSubject:', err);
            return null;
        }
    },
    
    
    delFaculty: async ({ faculty }) => {
        try {
            console.log(faculty);
            const pool = await poolPromise;
            const result = await pool.request()
                .input('faculty', sql.NVarChar, faculty) 
                .query('DELETE FROM FACULTY WHERE FACULTY = @faculty');

            return result.rowsAffected[0] > 0;
        } catch (err) {
            console.error('PROBLEMO delFaculty:', err);
            return false;
        }
    },

    delPulpit: async ({ pulpit }) => {
        try {
            const pool = await poolPromise;
            const result = await pool.request()
                .input('pulpit', sql.NVarChar, pulpit)
                .query('DELETE FROM PULPIT WHERE PULPIT = @pulpit');

            return result.rowsAffected[0] > 0;
        } catch (err) {
            console.error('PROBLEMO delPulpit:', err);
            return false;
        }
    },

    delTeacher: async ({ teacher }) => {
        try {
            const pool = await poolPromise;
            const result = await pool.request()
                .input('teacher', sql.NVarChar, teacher)
                .query('DELETE FROM TEACHER WHERE TEACHER = @teacher');

            return result.rowsAffected[0] > 0;
        } catch (err) {
            console.error('PROBLEMO delTeacher:', err);
            return false;
        }
    },

    delSubject: async ({ subject }) => {
        try {
            const pool = await poolPromise;
            const result = await pool.request()
                .input('subject', sql.NVarChar, subject)
                .query('DELETE FROM SUBJECT WHERE SUBJECT = @subject');

            return result.rowsAffected[0] > 0;
        } catch (err) {
            console.error('PROBLEMO delSubject:', err);
            return false;
        }
    },

    getTeachersByFaculty: async ({ faculty }) => {
        try {
            const pool = await poolPromise;
            const request = pool.request();
            request.input('faculty', sql.NVarChar, faculty);

            const result = await request.query(`SELECT t.TEACHER AS Teacher, t.TEACHER_NAME AS TeacherName, t.PULPIT AS Pulpit, 
                f.FACULTY AS Faculty 
                FROM TEACHER t JOIN PULPIT p 
                ON t.PULPIT = p.PULPIT JOIN FACULTY f 
                ON p.FACULTY = f.FACULTY 
                WHERE f.FACULTY = @faculty`);


            return {
                Faculty: faculty,
                Teachers: result.recordset
            };
        } catch (err) {
            console.error('PROBLEMO getTeachersByFaculty:', err);
            return { Faculty: faculty, Teachers: [] };
        }
    },

    getSubjectsByFaculties: async ({ faculty }) => {
    try {
        const pool = await poolPromise;
        const request = pool.request();
        request.input('faculty', sql.NVarChar, faculty);

        const result = await request.query(`
    SELECT p.PULPIT AS Pulpit, 
           p.PULPIT_NAME AS PulpitName, 
           f.FACULTY AS Faculty, 
           s.SUBJECT AS Subject, 
           s.SUBJECT_NAME AS SubjectName 
    FROM SUBJECT s
    JOIN PULPIT p ON s.PULPIT = p.PULPIT
    JOIN FACULTY f ON p.FACULTY = f.FACULTY
    WHERE f.FACULTY = @faculty
`);

        const pulpitsMap = {};
result.recordset.forEach(row => {
    if (!pulpitsMap[row.Pulpit]) {   
        pulpitsMap[row.Pulpit] = {
            Faculty: row.Faculty,
            Pulpit: row.Pulpit,
            PulpitName: row.PulpitName,
            Subjects: []
        };
    }
    pulpitsMap[row.Pulpit].Subjects.push({
        Subject: row.Subject,
        SubjectName: row.SubjectName,
        Pulpit: row.Pulpit
    });
});

        if (Object.keys(pulpitsMap).length === 0) {
            return [{
                Faculty: faculty,
                Pulpit: "",
                PulpitName: "",
                Subjects: []
            }];
        }

        return Object.values(pulpitsMap);
    } catch (err) {
        console.error('PROBLEMO getSubjectsByFaculties:', err);
        return [{
            Faculty: faculty,
            Pulpit: "",
            PulpitName: "",
            Subjects: []
        }];
    }
}

};

const port = 3000;

const server = http.createServer((req, res) => {
    res.setHeader('Content-Type', 'application/json');
    if (req.method === 'POST') {
        let body = '';

        req.on('data', chunk => {
            body += chunk;
        });

        req.on('end', async () => {
            try {
                const { query, variables } = JSON.parse(body);

                const result = await graphql({
                    schema: schema,
                    source: query,
                    rootValue: root,
                    variableValues: variables
                });

                res.statusCode = 200;
                res.end(JSON.stringify(result));

            } catch (error) {
                res.statusCode = 400;
                res.end(JSON.stringify({ error: 'Invalid JSON or Request', details: error.message }));
            }
        });
    } else {
        res.statusCode = 405;
        res.end(JSON.stringify({ error: 'Method Not Allowed. Use POST with JSON body.' }));
    }
});

server.listen(port, () => {
    console.log('Server running at http://localhost:3000/');
});