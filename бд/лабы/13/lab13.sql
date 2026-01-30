alter pluggable database FIO_PDB1 open;

ALTER SESSION SET CONTAINER = "FIO_PDB1";
alter session set nls_date_format='dd-mm-yyyy hh24:mi:ss';
GRANT CREATE TABLESPACE TO ZES;
GRANT ALTER TABLESPACE TO ZES;

select username, default_tablespace
from dba_users
where username = 'ZES';


create tablespace t1
    datafile 't1_zes.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;

create tablespace t2
    datafile 't2_zes.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;

create tablespace t3
    datafile 't3_zes.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;

create tablespace t4
    datafile 't4_zes.dbf'
    size 7 m
    autoextend on
    maxsize unlimited
    extent management local;

alter user ZES quota unlimited on t1;
alter user ZES quota unlimited on t2;
alter user ZES quota unlimited on t3;
alter user ZES quota unlimited on t4;

--1
--drop table T_RANGE;
--c диапазонным секционированием. 
--Используйте ключ секционирования типа NUMBER. 
create table T_RANGE
(
    id number,
    TIME_ID date
)
partition by range (id)
(
    partition p0 values less than (100) tablespace t1,
    partition p1 values less than (200) tablespace t2,
    partition p2 values less than (300) tablespace t3,
    partition PMAX values less than (maxvalue) tablespace t4
);


begin
    for i in 1..400
    loop
        insert into T_RANGE(id, time_id) values (i, sysdate);
    end loop;
end;
commit;

select * from T_RANGE partition(p0);
select * from T_RANGE partition(p1);
select * from T_RANGE partition(p2);
select * from T_RANGE partition(PMAX);

select TABLE_NAME, PARTITION_NAME, HIGH_VALUE, TABLESPACE_NAME
from USER_TAB_PARTITIONS
where table_name = 'T_RANGE';

--2
--drop table T_INTERVAL;
--c интервальным секционированием. 
--Используйте ключ секционирования типа DATE.
create table T_INTERVAL
(
    id number,
    time_id date
)
    partition by range (time_id)
    interval (numtoyminterval(1, 'month'))
(
    partition p0 values less than (to_date('1-12-2010', 'dd-mm-yyyy')) tablespace t1,
    partition p1 values less than (to_date('1-12-2015', 'dd-mm-yyyy')) tablespace t2,
    partition p2 values less than (to_date('1-12-2020', 'dd-mm-yyyy')) tablespace t3
);

insert into T_INTERVAL(id, time_id) values (50, '01-02-2008');
insert into T_INTERVAL(id, time_id) values (105, '01-01-2009');
insert into T_INTERVAL(id, time_id) values (105, '01-01-2007');
insert into T_INTERVAL(id, time_id) values (205, '01-01-2019');
insert into T_INTERVAL(id, time_id) values (305, '01-01-2013');
insert into T_INTERVAL(id, time_id) values (405, '01-01-2017');
insert into T_INTERVAL(id, time_id) values (505, '01-01-2022');
commit;

select * from T_INTERVAL partition (p0);
select * from T_INTERVAL partition (p1);
select * from T_INTERVAL partition (p2);
select * from T_INTERVAL partition (SYS_P448);

select TABLE_NAME, PARTITION_NAME, HIGH_VALUE, TABLESPACE_NAME
from USER_TAB_PARTITIONS
where table_name = 'T_INTERVAL';

--3
--drop table T_HASH;
--c хэш-секционированием. 
--Используйте ключ секционирования типа VARCHAR2.
create table T_HASH
(
    str varchar2(50),
    id number
)
partition by hash (str)
(
    partition k0 tablespace t1,
    partition k1 tablespace t2,
    partition k2 tablespace t3,
    partition k3 tablespace t4
);

insert into T_HASH (str, id) values ('я лечу по трасе', 1);
insert into T_HASH (str, id) values ('быстро с нарушением правил', 2);
insert into T_HASH (str, id) values ('бибибиби', 3);
insert into T_HASH (str, id) values ('пки', 4);
insert into T_HASH (str, id) values ('что такое бипки?', 7);
insert into T_HASH (str, id) values ('емае я щас так раскашлялась', 14);
insert into T_HASH (str, id) values ('с кайфом', 32);
commit;

select * from T_HASH partition (k0);
select * from T_HASH partition (k1);
select * from T_HASH partition (k2);
select * from T_HASH partition (k3);

select TABLE_NAME, PARTITION_NAME, HIGH_VALUE, TABLESPACE_NAME
from USER_TAB_PARTITIONS
where table_name = 'T_HASH';

--4
--drop table T_LIST;
--со списочным секционированием. 
--Используйте ключ секционирования типа CHAR.
create table T_LIST
(
    obj char(3)
)
partition by list(obj)
(
    partition l0 values ('a') tablespace t1,
    partition l1 values ('b') tablespace t2,
    partition l2 values ('c') tablespace t3,
    partition l3 values (default) tablespace t4
);

insert into T_list(obj) values('a');
insert into T_list(obj) values('b');
insert into T_list(obj) values('c');
insert into T_list(obj) values('d');
insert into T_list(obj) values('e');
commit;

select * from T_list partition (l0);
select * from T_list partition (l1);
select * from T_list partition (l2);
select * from T_list partition (l3);

-- 6. Продемонстрируйте для всех таблиц процесс перемещения 
--строк между секциями, 
-- при изменении (оператор UPDATE) ключа секционирования.
alter table T_RANGE enable row movement;
select * from T_RANGE partition(PMAX);
update T_RANGE set id=2 where id=40;
select * from T_RANGE partition(p0) order by id;
commit;

alter table T_INTERVAL enable row movement;
select * from T_INTERVAL partition(p0);
update T_INTERVAL set time_id=to_date('01-02-2017') where id=50;
select * from T_INTERVAL partition(p2);

alter table T_HASH enable row movement;
select * from T_HASH partition(k2);
update T_HASH set str='SWAGMASTER' where id=3;
select * from T_HASH partition(k2);

alter table T_LIST enable row movement;
select * from T_LIST partition(l0);
update T_LIST set obj='b' where obj='a';
select * from T_LIST partition(l1);

-- 7. Для одной из таблиц продемонстрируйте действие 
--оператора ALTER TABLE MERGE.
alter table T_RANGE merge partitions p1, p2 into partition p5 tablespace t4;
select * from T_RANGE partition(p5);

-- 8. Для одной из таблиц продемонстрируйте действие оператора ALTER TABLE SPLIT.
alter table T_RANGE split partition p5 at (200)
into (partition p1 tablespace t1, partition p2 tablespace t2);
select * from T_RANGE partition(p5);
select * from T_RANGE partition(p1);
select * from T_RANGE partition(p2);

-- 9. Для одной из таблиц продемонстрируйте действие оператора ALTER TABLE EXCHANGE.
drop table T_RANGE1;
create table T_RANGE1
(
    id      number,
    TIME_ID date
);
alter table T_RANGE exchange partition p0 with table T_RANGE1 without validation;
select * from T_RANGE partition (p0);
select * from T_RANGE1;