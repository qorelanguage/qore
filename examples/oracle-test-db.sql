
create table family (
   family_id int not null,
   name varchar2(80) not null
);

insert into family values ( 1, 'Smith' );
insert into family values ( 2, 'Jones' );

create table people (
   person_id int not null,
   family_id int not null,
   name varchar2(250) not null,
   dob date not null
);

insert into people values ( 1, 1, 'Arnie', to_date('1983-05-13', 'YYYY-MM-DD') );
insert into people values ( 2, 1, 'Sylvia', to_date('1994-11-10', 'YYYY-MM-DD') );
insert into people values ( 3, 1, 'Carol', to_date('2003-07-23', 'YYYY-MM-DD') );
insert into people values ( 4, 1, 'Bernard', to_date('1979-02-27', 'YYYY-MM-DD') );
insert into people values ( 5, 1, 'Isaac', to_date('2000-04-04', 'YYYY-MM-DD') );
insert into people values ( 6, 2, 'Alan', to_date('1992-06-04', 'YYYY-MM-DD') );
insert into people values ( 7, 2, 'John', to_date('1995-03-23', 'YYYY-MM-DD') );

create table attributes (
   person_id int not null,
   attribute varchar2(80) not null,
   value varchar2(160) not null
);

insert into attributes values ( 1, 'hair', 'blond' );
insert into attributes values ( 1, 'eyes', 'brown' );
insert into attributes values ( 2, 'hair', 'blond' );
insert into attributes values ( 2, 'eyes', 'blue');
insert into attributes values ( 3, 'hair', 'brown' );
insert into attributes values ( 3, 'eyes', 'green');
insert into attributes values ( 4, 'hair', 'brown' );
insert into attributes values ( 4, 'eyes', 'brown');
insert into attributes values ( 5, 'hair', 'red' );
insert into attributes values ( 5, 'eyes', 'green');
insert into attributes values ( 6, 'hair', 'black' );
insert into attributes values ( 6, 'eyes', 'blue');
insert into attributes values ( 7, 'hair', 'brown' );
insert into attributes values ( 7, 'eyes', 'brown');
