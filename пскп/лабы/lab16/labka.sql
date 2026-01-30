use master;
go
DROP DATABASE pskp;
go

create database pskp;
USE pskp;

GO

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
    PULPIT NVARCHAR(200) FOREIGN KEY REFERENCES PULPIT(PULPIT)
);
GO

INSERT INTO dbo.TEACHER (TEACHER, TEACHER_NAME, GENDER, PULPIT) VALUES
('T001',N'Иванов Иван Иванович',N'КИТ1'),
('T002',N'Петров Пётр Петрович',N'КИТ1'),
('T003',N'Сидоров Сергей Сергеевич', N'КИТ2'),
('T004',N'Кузнецова Анна Викторовна',N'КРФ'),
('T005',N'Смирнов Алексей Николаевич',N'КЭУ'),
('T006',N'Орлова Мария Андреевна',N'КФЛ'),
('T007',N'Ковалёв Дмитрий Олегович',N'КИЯ'),
('T008',N'Фёдорова Елена Павловна',N'КПР'),
('T009',N'Никитин Максим Игоревич', N'КПР');
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
