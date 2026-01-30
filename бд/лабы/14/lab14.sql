ALTER PLUGGABLE DATABASE FIO_PDB1 OPEN;
-----
alter session set nls_date_format='dd-mm-yyyy hh24:mi:ss';
alter system set JOB_QUEUE_PROCESSES = 5; 
select count(*) from dba_objects_ae where Object_Type = 'paket';

--drop table cooltable;
create table cooltable( id number, textik varchar2(50));

--drop table cooltable_two;
create table cooltable_two( id number, textik varchar2(50));

--drop table coolSTATUS;
create table coolSTATUS( status varchar2(20), error_mess varchar(200), datentime date default sysdate );--табличка с состояниями импорта/экспорта

insert into cooltable values (1, 'text1');
insert into cooltable values (2, 'text2');
insert into cooltable values (3, 'text2');
insert into cooltable values (4, 'text3');
insert into cooltable values (5, 'text4');
commit;

select * from cooltable_two;
----------------------------------2
create or replace procedure job_proc is
    cursor c_job is 
    select * from cooltable;
    v_error_mess varchar2(50);
begin
    for i in c_job
    loop 
        insert into cooltable_two values(i.id, i.textik);
    end loop;
    
    delete from cooltable;
    
    insert into coolSTATUS (status, error_mess) values ('success', v_error_mess);
    commit;
    exception 
        when others then 
            v_error_mess := sqlerrm;
            insert into coolSTATUS (status, error_mess) values ('not success :(', v_error_mess);
            commit;
end job_proc;



declare job_num user_jobs.job%type;
begin
    dbms_job.submit(job_num, 'BEGIN job_proc(); END;', sysdate, 'sysdate +7');
    commit;
    dbms_output.put_line(job_num);
end;

select * from coolSTATUS;

--------3
select job, what, last_date, last_sec, next_date, next_sec, broken from user_jobs;

--------4
begin dbms_job.run(82); end; 
begin dbms_job.remove(82); end; 

select * from coolSTATUS;

------------------6
SELECT username
FROM dba_users;

SELECT user FROM dual;
show con_name;


GRANT CREATE JOB TO ZES;
GRANT EXECUTE ON DBMS_SCHEDULER TO ZES;
GRANT MANAGE SCHEDULER TO ZES;

begin 
dbms_scheduler.create_schedule( 
    schedule_name=> 'coolsch',
    start_date => sysdate,
    repeat_interval => 'FREQ=WEEKLY',
    comments => 'schedule starts now'
);
end;

select * from user_scheduler_schedules;
-----------------
begin
    dbms_scheduler.create_program(
        program_name=> 'coolprogram',
        program_type => 'stored_procedure',
        program_action => 'job_proc',
        number_of_arguments => 0,
        enabled => true,
        comments => 'PROGRAMMA'
);
end;

select * from user_scheduler_programs;
---------------------
begin
    dbms_scheduler.create_job(
        job_name=> 'cooljob',
        program_name => 'coolprogram',
        schedule_name => 'coolsch',
        enabled => true
);
end;

select * from user_scheduler_jobs;
------------
begin dbms_scheduler.disable('cooljob'); end; --begin dbms_scheduler.enable('cooljob'); end;

begin dbms_scheduler.run_job('cooljob'); end;

begin dbms_scheduler.drop_job('cooljob'); end;

select * from coolSTATUS;




