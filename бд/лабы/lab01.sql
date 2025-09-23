select * from XXX_t;

--task1

create table XXX_t (i number(2) primary key, x number(3), s varchar(50) );

drop table XXX_t;

--task2

insert into XXX_t (i, x, s ) 
values (1, 19 , 'krytayakatya');

insert into XXX_t (i, x, s ) 
values (2, 15 , 'leha');

insert into XXX_t (i, x, s ) 
values (3, 19 , 'zavtrak');

commit;

--task2

update XXX_t
    set x = 19
where s = 'leha';

update XXX_t
    set x = 12
where s = 'zavtrak';

commit;

--task3

select count(*) from XXX_t where x = 19;

--task4

delete from XXX_t where x IN (15);

--task 5

create table XXX_t1 (i number(3), x number(3) , s varchar(50), foreign key (i) references XXX_t(i));

insert into XXX_t1 (i, x, s) values (1, 34, 'super krutaya');
insert into XXX_t1 (i, x, s) values (2, 42, 'super krutoi');

select * from XXX_t1;

--

select t.s, t.x from XXX_t t
inner join XXX_t1 t1
on t.i = t1.i;

select t.s, t.x from XXX_t t
left join XXX_t1 t1
on t.i = t1.i;

select t.s, t.x from XXX_t t
right join XXX_t1 t1
on t.i = t1.i;





