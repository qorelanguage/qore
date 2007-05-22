
create table family (
   family_id int not null,
   name varchar(80) not null
)

insert into family values ( 1, 'Smith' )
insert into family values ( 2, 'Jones' )
go

create table people (
   person_id int not null,
   family_id int not null,
   name varchar(250) not null,
   dob date not null
)

insert into people values ( 1, 1, 'Arnie', '1983-05-13' )
insert into people values ( 2, 1, 'Sylvia', '1994-11-10' )
insert into people values ( 3, 1, 'Carol', '2003-07-23' )
insert into people values ( 4, 1, 'Bernard', '1979-02-27' )
insert into people values ( 5, 1, 'Isaac', '2000-04-04' )
insert into people values ( 6, 2, 'Alan', '1992-06-04' )
insert into people values ( 7, 2, 'John', '1995-03-23' )
go

create table attributes (
   person_id int not null,
   attribute varchar(80) not null,
   value varchar(160) not null
)

insert into attributes values ( 1, 'hair', 'blond' )
insert into attributes values ( 1, 'eyes', 'brown' )
insert into attributes values ( 2, 'hair', 'blond' )
insert into attributes values ( 2, 'eyes', 'blue')
insert into attributes values ( 3, 'hair', 'brown' )
insert into attributes values ( 3, 'eyes', 'green')
insert into attributes values ( 4, 'hair', 'brown' )
insert into attributes values ( 4, 'eyes', 'brown')
insert into attributes values ( 5, 'hair', 'red' )
insert into attributes values ( 5, 'eyes', 'green')
insert into attributes values ( 6, 'hair', 'black' )
insert into attributes values ( 6, 'eyes', 'blue')
insert into attributes values ( 7, 'hair', 'brown' )
insert into attributes values ( 7, 'eyes', 'brown')
go

