use master;
go
DROP DATABASE pskp;
go

create database pskp;
USE pskp;

GO

-- AUDITORIUM_TYPE
CREATE TABLE AUDITORIUM_TYPE
(
    AUDITORIUM_TYPE NVARCHAR(100) CONSTRAINT AUDITORIUM_TYPE_PK PRIMARY KEY,
    AUDITORIUM_TYPENAME nVARCHAR(100)
);
GO

INSERT INTO dbo.AUDITORIUM_TYPE (AUDITORIUM_TYPE, AUDITORIUM_TYPENAME) VALUES
(N'ЛК',   N'Лекционная'),
(N'ЛК-1', N'Лекционная малая'),
(N'ЛК-2', N'Лекционная с мультимедийным оборудованием'),
(N'ПЗ',   N'Практические занятия'),
(N'ЛБ',   N'Лабораторные занятия');
GO


SELECT * FROM AUDITORIUM_TYPE;

-- AUDITORIUM
CREATE TABLE AUDITORIUM
(
    AUDITORIUM NVARCHAR(200) CONSTRAINT AUDITORIUM_PK PRIMARY KEY,
    AUDITORIUM_TYPE nvarCHAR(100)
        CONSTRAINT AUDITORIUM_AUDITORIUM_TYPE_FK
        FOREIGN KEY REFERENCES AUDITORIUM_TYPE(AUDITORIUM_TYPE),
    AUDITORIUM_CAPACITY INTEGER CONSTRAINT AUDITORIUM_CAPACITY_CHECK DEFAULT 1 CHECK (AUDITORIUM_CAPACITY BETWEEN 1 AND 300),
    AUDITORIUM_NAME nVARCHAR(500)
);
GO

INSERT INTO dbo.AUDITORIUM
    (AUDITORIUM, AUDITORIUM_NAME, AUDITORIUM_TYPE, AUDITORIUM_CAPACITY)
VALUES
('206-1', '206-1', N'ЛК-1', 15),
('301-1', '301-1', N'ЛК-1', 15),
('236-1', '236-1', N'ЛК',   60),
('313-1', '313-1', N'ЛК-2', 60),
('324-1', '324-1', N'ЛК-2', 50),
('413-1', '413-1', N'ЛК-1', 15),
('423-1', '423-1', N'ЛК-1', 90),
('408-2', '408-2', N'ЛК',   90);
GO


select * from dbo.AUDITORIUM;
delete AUDITORIUM where AUDITORIUM = '101';

-- FACULTY
CREATE TABLE FACULTY
(
    FACULTY NVARCHAR(100) PRIMARY KEY,
    FACULTY_NAME NVARCHAR(500) DEFAULT N'Не указано'
);
GO

INSERT INTO dbo.FACULTY (FACULTY, FACULTY_NAME) VALUES
(N'ФИТ', N'Факультет информационных технологий и компьютерных наук'),
(N'ФРЭ', N'Факультет радиофизики и электроники'),
(N'ФМФ', N'Факультет математики и механико-математических наук'),
(N'ФЭУ', N'Факультет экономики и управления'),
(N'ФФ',  N'Филологический факультет'),
(N'ФИ',  N'Факультет иностранных языков'),
(N'ФЮ',  N'Юридический факультет');
GO


-- PROFESSION
CREATE TABLE PROFESSION
(
    PROFESSION NVARCHAR(200) PRIMARY KEY,
    FACULTY NVARCHAR(100) FOREIGN KEY REFERENCES FACULTY(FACULTY),
    PROFESSION_NAME NVARCHAR(1000),
    QUALIFICATION NVARCHAR(500)
);
GO

INSERT INTO dbo.PROFESSION (FACULTY, PROFESSION, PROFESSION_NAME, QUALIFICATION) VALUES
(N'ФИТ','1-40 01 02',N'Программное обеспечение информационных технологий',N'Инженер-программист'),
(N'ФРЭ','1-36 01 08',N'Радиоэлектронные системы и комплексы',N'Инженер радиоэлектронных систем'),
(N'ФРЭ','1-36 07 01',N'Радиоэлектроника',N'Инженер-радиоэлектронщик'),
(N'ФЭУ','1-75 01 01',N'Экономика',N'Экономист'),
(N'ФЭУ','1-75 02 01',N'Менеджмент',N'Менеджер'),
(N'ФЭУ','1-89 02 02',N'Коммерция',N'Экономист-коммерсант'),
(N'ФЮ','1-25 01 07',N'Правоведение',N'Юрист'),
(N'ФЮ','1-25 01 08',N'Экономическое право',N'Юрист'),
(N'ФМФ','1-36 05 01',N'Прикладная математика',N'Математик'),
(N'ФМФ','1-46 01 01',N'Механика',N'Инженер-механик'),
(N'ФИ','1-48 01 02',N'Лингвистика',N'Лингвист'),
(N'ФИ','1-48 01 05',N'Перевод и переводоведение',N'Переводчик'),
(N'ФФ','1-54 01 03',N'Филология',N'Филолог'),
(N'ФРЭ','1-47 01 01',N'Автоматизация',N'Инженер'),
(N'ФРЭ','1-36 06 01',N'Электронные вычислительные средства',N'Инженер вычислительной техники');
GO


-- PULPIT
CREATE TABLE PULPIT
(
    PULPIT NVARCHAR(200) PRIMARY KEY,
    PULPIT_NAME NVARCHAR(1000),
    FACULTY NVARCHAR(100) FOREIGN KEY REFERENCES FACULTY(FACULTY)
);
GO

INSERT INTO dbo.PULPIT (PULPIT, PULPIT_NAME, FACULTY) VALUES
(N'КИТ1', N'Кафедра информационных технологий и программирования', N'ФИТ'),
(N'КИТ2', N'Кафедра вычислительных систем', N'ФИТ'),
(N'КРФ',  N'Кафедра радиофизики', N'ФРЭ'),
(N'КЭУ',  N'Кафедра экономики и управления', N'ФЭУ'),
(N'КФЛ',  N'Кафедра филологии', N'ФФ'),
(N'КИЯ',  N'Кафедра иностранных языков', N'ФИ'),
(N'КПР',  N'Кафедра права', N'ФЮ');
GO


-- TEACHER
CREATE TABLE TEACHER
(
    TEACHER NVARCHAR(100) PRIMARY KEY,
    TEACHER_NAME NVARCHAR(500),
    GENDER NVARCHAR(1) CHECK (GENDER IN (N'М', N'Ж')),
    PULPIT NVARCHAR(200) FOREIGN KEY REFERENCES PULPIT(PULPIT)
);
GO

INSERT INTO dbo.TEACHER (TEACHER, TEACHER_NAME, GENDER, PULPIT) VALUES
('T001',N'Иванов Иван Иванович',N'М',N'КИТ1'),
('T002',N'Петров Пётр Петрович',N'М',N'КИТ1'),
('T003',N'Сидоров Сергей Сергеевич',N'М',N'КИТ2'),
('T004',N'Кузнецова Анна Викторовна',N'Ж',N'КРФ'),
('T005',N'Смирнов Алексей Николаевич',N'М',N'КЭУ'),
('T006',N'Орлова Мария Андреевна',N'Ж',N'КФЛ'),
('T007',N'Ковалёв Дмитрий Олегович',N'М',N'КИЯ'),
('T008',N'Фёдорова Елена Павловна',N'Ж',N'КПР'),
('T009',N'Никитин Максим Игоревич',N'М',N'КПР');
GO


-- SUBJECT
CREATE TABLE SUBJECT
(
    SUBJECT NVARCHAR(100) PRIMARY KEY,
    SUBJECT_NAME NVARCHAR(1000) UNIQUE,
    PULPIT NVARCHAR(200) FOREIGN KEY REFERENCES PULPIT(PULPIT)
);
GO

INSERT INTO dbo.SUBJECT (SUBJECT, SUBJECT_NAME, PULPIT) VALUES
(N'БД', N'Базы данных', N'КИТ1'),
(N'ОС', N'Операционные системы', N'КИТ1'),
(N'СХ', N'Схемотехника', N'КРФ'),
(N'МАТ',N'Математика', N'КИТ2'),
(N'ЭКН',N'Экономика', N'КЭУ'),
(N'ФИЛ',N'Филология', N'КФЛ'),
(N'ИЯ', N'Иностранный язык', N'КИЯ'),
(N'ПР', N'Право', N'КПР');
GO


-- GROUPS
CREATE TABLE GROUPS
(
    IDGROUP INT IDENTITY(1,1) PRIMARY KEY,
    FACULTY NVARCHAR(100) FOREIGN KEY REFERENCES FACULTY(FACULTY),
    PROFESSION NVARCHAR(200) FOREIGN KEY REFERENCES PROFESSION(PROFESSION),
    YEAR_FIRST SMALLINT CHECK (YEAR_FIRST <= YEAR(GETDATE()))
);
GO

INSERT INTO dbo.GROUPS (FACULTY, PROFESSION, YEAR_FIRST) VALUES
(N'ФИТ','1-40 01 02',2013),
(N'ФИТ','1-40 01 02',2012),
(N'ФИТ','1-40 01 02',2011),
(N'ФИТ','1-40 01 02',2010),

(N'ФРЭ','1-47 01 01',2013),
(N'ФРЭ','1-47 01 01',2012),
(N'ФРЭ','1-47 01 01',2011),

(N'ФРЭ','1-36 06 01',2010),
(N'ФРЭ','1-36 06 01',2013),
(N'ФРЭ','1-36 06 01',2012),
(N'ФРЭ','1-36 06 01',2011),

(N'ФРЭ','1-36 01 08',2013),
(N'ФРЭ','1-36 01 08',2012),
(N'ФРЭ','1-36 07 01',2011),
(N'ФРЭ','1-36 07 01',2010),

(N'ФИ','1-48 01 02',2012),
(N'ФИ','1-48 01 02',2011),
(N'ФИ','1-48 01 05',2013),

(N'ФФ','1-54 01 03',2012),

(N'ФЭУ','1-75 01 01',2013),
(N'ФЭУ','1-75 02 01',2012),
(N'ФЭУ','1-75 02 01',2011),
(N'ФЭУ','1-89 02 02',2012),
(N'ФЭУ','1-89 02 02',2011),

(N'ФМФ','1-36 05 01',2013),
(N'ФМФ','1-36 05 01',2012),
(N'ФМФ','1-46 01 01',2012),

(N'ФЮ','1-25 01 07',2013),
(N'ФЮ','1-25 01 07',2012),
(N'ФЮ','1-25 01 07',2010),
(N'ФЮ','1-25 01 08',2013),
(N'ФЮ','1-25 01 08',2012);
GO


-- STUDENT
CREATE TABLE STUDENT
(
    IDSTUDENT INT IDENTITY(1000,1) PRIMARY KEY,
    IDGROUP INT FOREIGN KEY REFERENCES GROUPS(IDGROUP),
    [NAME] NVARCHAR(1000),
    BDAY DATE,
    STAMP INT,
    INFO XML,
    FOTO NVARCHAR(200)
);
GO

INSERT INTO dbo.STUDENT (IDGROUP, [NAME], BDAY) VALUES
(1, N'Александров Алексей Александрович', '1995-03-11'),
(1, N'Иванова Мария Сергеевна', '1995-12-07'),
(1, N'Петров Дмитрий Андреевич', '1995-10-12'),
(1, N'Сидорова Анна Викторовна', '1995-01-08'),
(1, N'Кузнецов Илья Максимович', '1995-08-02'),

(2, N'Смирнов Павел Олегович', '1994-07-12'),
(2, N'Орлова Екатерина Дмитриевна', '1994-03-06'),
(2, N'Никитин Артём Сергеевич', '1994-11-09'),
(2, N'Васильева Дарья Игоревна', '1994-10-04'),
(2, N'Фёдоров Михаил Андреевич', '1994-01-08'),

(3, N'Зайцев Роман Евгеньевич', '1993-08-02'),
(3, N'Ковалёва Алина Олеговна', '1993-12-07'),
(3, N'Морозов Денис Ильич', '1993-12-02'),

(4, N'Попова Татьяна Сергеевна', '1992-03-08'),
(4, N'Лебедев Кирилл Николаевич', '1992-06-02'),
(4, N'Соколова Юлия Андреевна', '1992-12-11'),
(4, N'Волков Максим Романович', '1992-05-11'),
(4, N'Семёнова Ирина Павловна', '1992-11-09'),
(4, N'Егоров Владислав Денисович', '1992-11-01'),

(5, N'Панова Ольга Михайловна', '1995-07-08'),
(5, N'Беляев Арсений Петрович', '1995-11-02'),
(5, N'Гусева Наталья Викторовна', '1995-05-07'),
(5, N'Мельников Тимофей Сергеевич', '1995-08-04'),

(6, N'Романова Ксения Андреевна', '1994-11-08'),
(6, N'Данилов Игорь Алексеевич', '1994-03-02'),
(6, N'Тарасов Никита Дмитриевич', '1994-06-04'),
(6, N'Козлова Полина Сергеевна', '1994-11-09'),
(6, N'Фролов Андрей Максимович', '1994-07-04'),

(7, N'Жуков Вячеслав Олегович', '1993-01-03'),
(7, N'Киселёва Марина Игоревна', '1993-09-12'),
(7, N'Макаров Артём Владимирович', '1993-06-12'),
(7, N'Новикова Анастасия Романовна', '1993-02-09'),
(7, N'Комаров Денис Павлович', '1993-07-04'),

(8, N'Захаров Егор Михайлович', '1992-01-08'),
(8, N'Борисова Светлана Андреевна', '1992-05-12'),
(8, N'Громов Илья Сергеевич', '1992-11-08'),
(8, N'Широкова Валерия Олеговна', '1992-03-12'),

(9, N'Антонов Максим Игоревич', '1995-08-10'),
(9, N'Кузьмина Елена Павловна', '1995-05-02'),
(9, N'Осипов Дмитрий Николаевич', '1995-01-08'),
(9, N'Рябова Оксана Сергеевна', '1995-09-11'),

(10, N'Гаврилов Сергей Викторович', '1994-01-08'),
(10, N'Полякова Алина Дмитриевна', '1994-09-11'),
(10, N'Карпов Владислав Андреевич', '1994-04-06'),
(10, N'Тихонова Мария Олеговна', '1994-08-12');
GO



-- PROGRESS
CREATE TABLE PROGRESS
(
    SUBJECT NVARCHAR(100) FOREIGN KEY REFERENCES SUBJECT(SUBJECT),
    IDSTUDENT INT FOREIGN KEY REFERENCES STUDENT(IDSTUDENT),
    PDATE DATE,
    NOTE INT CHECK (NOTE BETWEEN 1 AND 10)
);
GO

INSERT INTO dbo.PROGRESS (SUBJECT, IDSTUDENT, PDATE, NOTE) VALUES
(N'БД', 1001, '2013-10-01', 8),
(N'БД', 1002, '2013-10-01', 7),
(N'БД', 1003, '2013-10-01', 5),
(N'БД', 1005, '2013-10-01', 4),

(N'БД', 1014, '2013-12-01', 5),
(N'БД', 1015, '2013-12-01', 9),
(N'БД', 1016, '2013-12-01', 5),
(N'БД', 1017, '2013-12-01', 4),

(N'БД', 1018, '2013-05-06', 4),
(N'БД', 1019, '2013-05-06', 7),
(N'БД', 1020, '2013-05-06', 7),
(N'БД', 1021, '2013-05-06', 9),
(N'БД', 1022, '2013-05-06', 5),
(N'БД', 1023, '2013-05-06', 6);
GO




