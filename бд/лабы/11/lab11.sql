alter pluggable database FIO_PDB1 open;

SELECT * FROM USER_TABLES;
select * from teacher;

--1
 -- 1. Разработайте локальную процедуру GET_TEACHERS (PCODE TEACHER.PULPIT%TYPE)
-- Процедура должна выводить список преподавателей из таблицы TEACHER (в стандартный серверный вывод), 
-- работающих на кафедре заданной кодом в параметре. 

select * from TEACHER;

create or replace procedure GET_TEACHERS(PCODE TEACHER.PULPIT%TYPE) is
begin
  for i in (select * from TEACHER where PULPIT = PCODE)
    loop
      dbms_output.put_line(i.TEACHER_NAME);
    end loop;
end;

begin
  GET_TEACHERS('ИСиТ');
end;

-- 2. Разработайте локальную функцию GET_NUM_TEACHERS (PCODE TEACHER.PULPIT%TYPE) RETURN NUMBER
-- Функция должна выводить количество преподавателей из таблицы TEACHER, 
-- работающих на кафедре заданной кодом в параметре. 
-- Разработайте анонимный блок и продемонстрируйте выполнение процедуры.

create or replace function GET_NUM_TEACHERS(PCODE TEACHER.PULPIT%TYPE) return number
  is
  num number;
begin
  select count(*) into num from TEACHER where PULPIT = PCODE;
  return num;
end;

begin
  dbms_output.put_line(GET_NUM_TEACHERS('ИСиТ'));
end;

-- 3. Разработайте процедуры:
-- GET_TEACHERS (FCODE FACULTY.FACULTY%TYPE)
-- Процедура должна выводить список преподавателей из таблицы TEACHER (в стандартный серверный вывод), 
-- работающих на факультете, заданным кодом в параметре.

-- GET_SUBJECTS (PCODE SUBJECT.PULPIT%TYPE)
-- Процедура должна выводить список дисциплин из таблицы SUBJECT, закрепленных за кафедрой,
-- заданной кодом кафедры в параметре. 
-- Разработайте анонимный блок и продемонстрируйте выполнение процедуры.

select * from TEACHER;

create or replace procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE) is
begin
  for i in (select * from TEACHER where PULPIT in (select PULPIT from PULPIT where FACULTY = FCODE))
    loop
      dbms_output.put_line(i.TEACHER_NAME);
    end loop;
end;

begin
  GET_TEACHERS('ХТиТ');
end;

create or replace procedure GET_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) is
begin
  for i in (select * from SUBJECT where PULPIT = PCODE)
    loop
      dbms_output.put_line(i.SUBJECT_NAME);
    end loop;
end;

begin
  GET_SUBJECTS('ИСиТ');
end;

--5 Функция должна выводить количество преподавателей из таблицы 
--TEACHER, работающих на факультете, заданным кодом в параметре. 
--Разработайте анонимный блок и продемонстрируйте выполнение процедуры.
declare 
    res number;
    function FGET_NUM_TEACHERS (FCODE FACULTY.FACULTY%TYPE)
    return number
    is
        result_num number;
    begin
        select count(TEACHER) into result_num 
                                from TEACHER T 
                                inner join PULPIT P 
                                on T.PULPIT = P.PULPIT 
                                where FACULTY = FCODE;
        return result_num;
    end FGET_NUM_TEACHERS;

begin
    res := FGET_NUM_TEACHERS('ЛХФ');
    DBMS_OUTPUT.PUT_LINE(res);
end;

--Функция должна выводить количество дисциплин из таблицы SUBJECT, 
--закрепленных за кафедрой, заданной кодом кафедры параметре. 
--Разработайте анонимный блок и продемонстрируйте выполнение процедуры. 
declare 
    res number;
    function GET_NUM_SUBJECTS (PCODE SUBJECT.PULPIT%TYPE) return number
    is
        result_num number;
    begin
        select count(SUBJECT) into result_num from SUBJECT where PULPIT=PCODE;
        return result_num;
    end GET_NUM_SUBJECTS;
begin
    res := GET_NUM_SUBJECTS('ИСиТ');
    DBMS_OUTPUT.PUT_LINE(res);
end;

--6

create or replace package TEACHERS as
  procedure GET_TEACHERS(FCODE FACULTY.FACULTY%type);
  procedure GET_SUBJECTS (PCODE SUBJECT.PULPIT%type);
  function GET_NUM_TEACHERS(FCODE FACULTY.FACULTY%type) return number;
  function GET_NUM_SUBJECTS(PCODE SUBJECT.PULPIT%type) return number;
end TEACHERS;

create or replace package body TEACHERS
is
    procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE)
        is
        cursor GetTeachers is
            select TEACHER, TEACHER_NAME, P.PULPIT
            from TEACHER inner join PULPIT P on P.PULPIT = TEACHER.PULPIT
            where FACULTY = FCODE;
    
        m_teacher      TEACHER.TEACHER%TYPE;
        m_teacher_name TEACHER.TEACHER_NAME%TYPE;
        m_pulpit       TEACHER.PULPIT%TYPE;
    begin
        open GetTeachers;
        fetch GetTeachers into m_teacher, m_teacher_name, m_pulpit;
    
        while (GetTeachers%found)
        loop
            DBMS_OUTPUT.PUT_LINE(m_teacher || ' ' || m_teacher_name || ' ' || m_pulpit);
            fetch GetTeachers into m_teacher, m_teacher_name, m_pulpit;
        end loop;
    
        close GetTeachers;
    end GET_TEACHERS;
    
    procedure GET_SUBJECTS (PCODE SUBJECT.PULPIT%TYPE)
    is
        cursor GetSubjects is
        select * from SUBJECT where PULPIT=PCODE;
    
        m_subject SUBJECT.SUBJECT%type;
        m_subject_name SUBJECT.SUBJECT_NAME%type;
        m_pulpit SUBJECT.PULPIT%type;
    begin
        open GetSubjects;
        fetch GetSubjects into m_subject, m_subject_name, m_pulpit;
    
        while (GetSubjects%found)
        loop
            DBMS_OUTPUT.PUT_LINE(m_subject || ' ' || m_subject_name || ' ' || m_pulpit);
            fetch GetSubjects into m_subject, m_subject_name, m_pulpit;
        end loop;
        close GetSubjects;
    
    end GET_SUBJECTS;
    
    function GET_NUM_TEACHERS (FCODE FACULTY.FACULTY%TYPE)
    return number
    is
        result_num number;
    begin
        select count(TEACHER) into result_num 
                                from TEACHER T 
                                inner join PULPIT P 
                                on T.PULPIT = P.PULPIT 
                                where FACULTY = FCODE;
        return result_num;
    end GET_NUM_TEACHERS;
    
    function GET_NUM_SUBJECTS (PCODE SUBJECT.PULPIT%TYPE) return number
    is
        result_num number;
    begin
        select count(SUBJECT) into result_num from SUBJECT where PULPIT=PCODE;
        return result_num;
    end GET_NUM_SUBJECTS;
    
end TEACHERS;
/
--7
begin
  DBMS_OUTPUT.PUT_LINE('Teachers amount at faculty: ' || TEACHERS.GET_NUM_TEACHERS('ИСиТ'));
  DBMS_OUTPUT.PUT_LINE('Teachers amount at pulpit: ' || TEACHERS.GET_NUM_SUBJECTS('ИСиТ'));
  TEACHERS.GET_TEACHERS('ЛХФ');
  DBMS_OUTPUT.PUT_LINE('');
  TEACHERS.GET_SUBJECTS('ЛХФ');
end;
